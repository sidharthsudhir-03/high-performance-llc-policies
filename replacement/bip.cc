#include <algorithm>
#include <cassert>
#include <map>
#include <vector>

#include "cache.h"

#define EPSILON_INVERSE 20 // Epsilon is 5%

namespace
{
std::map<CACHE*, std::vector<uint64_t>> last_used_cycles;
std::map<CACHE*, uint32_t> bimodal_counter;
}

void CACHE::initialize_replacement() { 
	::last_used_cycles[this] = std::vector<uint64_t>(NUM_SET * NUM_WAY);
	::bimodal_counter[this] = 0; 
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
	auto begin = std::next(std::begin(::last_used_cycles[this]), set * NUM_WAY);
  	auto end = std::next(begin, NUM_WAY);
	auto LRU = std::min_element(begin, end);
	auto& BIP = ::bimodal_counter[this];
	
	// Assign cache way as MRU element
	if (hit){ 
		::last_used_cycles[this].at(set * NUM_WAY + way) = current_cycle;
	} else{
		++BIP; // BIP counter updates every cache miss

		// 5% of misses assigned as MRU; rest assigned as LRU
		if (BIP % EPSILON_INVERSE == 0){
			::last_used_cycles[this].at(set * NUM_WAY + way) = current_cycle;			
		} else{
			::last_used_cycles[this].at(set * NUM_WAY + way) = (*LRU > 0) ? (*LRU - 1) : 0; // We are taking the least cycle value and decrementing by 1 to avoid ties
		}
	}
}

void CACHE::replacement_final_stats() {}

