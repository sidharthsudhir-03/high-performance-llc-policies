#include <algorithm>
#include <cassert>
#include <map>
#include <vector>

#include "cache.h"

#define NUM_LEADER_SETS_PER_POLICY 64
#define EPSILON_INVERSE 20
#define PSEL_BITS 10
#define PSEL_MAX ((1 << PSEL_BITS) - 1)  // 1023
#define PSEL_INIT (1 << (PSEL_BITS - 1)) // 512 (mid-point)


namespace
{
std::map<CACHE*, std::vector<uint64_t>> last_used_cycles;
std::map<CACHE*, std::uint32_t> PSEL_counter;
std::map<CACHE*, std::uint32_t> BIP_counter;

enum SetType { LRU_LEADER, BIP_LEADER, FOLLOWER };

SetType identify_set_type(uint32_t set, uint32_t num_set)
{
  uint32_t num_constituencies = NUM_LEADER_SETS_PER_POLICY;
  uint32_t sets_per_constituency = num_set / num_constituencies;

  uint32_t constituency = set / sets_per_constituency;
  uint32_t offset = set % sets_per_constituency;
  uint32_t complement_offset = (~offset) & (sets_per_constituency - 1);

  // LRU leader: constituency == offset
  if (constituency == offset) {
    return LRU_LEADER;
  }
  // BIP leader: constituency == ~offset
  else if (constituency == complement_offset) {
    return BIP_LEADER;
  }
  // Follower set
  else {
    return FOLLOWER;
  }
}

void apply_lru_insertion(CACHE* cache, uint32_t set, uint32_t way, uint64_t current_cycle)
{
  ::last_used_cycles[cache].at(set * cache->NUM_WAY + way) = current_cycle;
}

void apply_bip_insertion(CACHE* cache, uint32_t set, uint32_t way, uint64_t current_cycle, uint64_t lru_timestamp)
{
  auto& BIP = ::BIP_counter[cache];
  ++BIP;

  // 5% of misses (every 20th) assigned as MRU; rest assigned as LRU
  if (BIP % EPSILON_INVERSE == 0) {
    // Insert at MRU position
    ::last_used_cycles[cache].at(set * cache->NUM_WAY + way) = current_cycle;
  } else {
    // Insert at LRU position; take minimum cycle value and decrement by 1 to avoid ties
    ::last_used_cycles[cache].at(set * cache->NUM_WAY + way) = (lru_timestamp > 0) ? (lru_timestamp - 1) : 0;
  }
}
}

void CACHE::initialize_replacement() { 
    ::last_used_cycles[this] = std::vector<uint64_t>(NUM_SET * NUM_WAY);
    ::PSEL_counter[this] = PSEL_INIT;
    ::BIP_counter[this] = 0; 
}

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    auto begin = std::next(std::begin(::last_used_cycles[this]), set * NUM_WAY);
  	auto end = std::next(begin, NUM_WAY);

  	// Find the way whose last use cycle is most distant
  	auto victim = std::min_element(begin, end);
  	assert(begin <= victim);
  	assert(victim < end);
  	return static_cast<uint32_t>(std::distance(begin, victim)); // cast protected by prior asserts
}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    ::SetType role = identify_set_type(set, NUM_SET);
    auto begin = std::next(std::begin(::last_used_cycles[this]), set * NUM_WAY);
  	auto end = std::next(begin, NUM_WAY);
	auto LRU = std::min_element(begin, end);
    auto& PSEL = ::PSEL_counter[this];

    if (hit){
        // Insert at MRU position on cache hit
        apply_lru_insertion(this, set, way, current_cycle);
    } else{
        // Apply policy based on set type on cache miss
        switch(role){

            // Perform LRU insertion if LRU Leader and increment PSEL counter
            case LRU_LEADER:
                apply_lru_insertion(this, set, way, current_cycle);
                if(PSEL < PSEL_MAX){
                    PSEL++;
                }
                break;
            
            // Perform BIP insertion if BIP Leader and decrement PSEL counter
            case BIP_LEADER:
                apply_bip_insertion(this, set, way, current_cycle, *LRU);
                if(PSEL > 0){
                    PSEL--;
                }
                break;

            case FOLLOWER:
                if(PSEL >= PSEL_INIT){
                    apply_bip_insertion(this, set, way, current_cycle, *LRU);
                } else{
                    apply_lru_insertion(this, set, way, current_cycle);
                }
                break;
        }
    }
}

void CACHE::replacement_final_stats() {}


