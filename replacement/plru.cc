#include <algorithm>
#include <cassert>
#include <map>
#include <vector>

#include "cache.h"
namespace
{
std::map<CACHE*, std::vector<uint8_t>> plru_bits;
}

void CACHE::initialize_replacement() { 
	::plru_bits[this] = std::vector<uint8_t>(NUM_SET * (NUM_WAY - 1), 0);
}

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
	auto &bits = ::plru_bits[this];
	assert((NUM_WAY & (NUM_WAY - 1)) == 0);
	const size_t base = static_cast<size_t>(set) * (NUM_WAY - 1);

	uint32_t way = 0;
  	uint32_t span = NUM_WAY;     // number of leaves in current subtree
  	uint32_t node = 0;           // index within this set's internal-node array
	
	while (span > 1) {
    		uint8_t b = bits[base + node];
    		uint32_t half = span >> 1;
    		if (b == 0) {
      			// Evict from left subtree
      			node = 2 * node + 1;
      			// way unchanged
   		 } 
		else {
      			// Evict from right subtree
      			way += half;
      			node = 2 * node + 2;
   		}
    		span = half;
  	}	

  	return way;
}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
	auto &bits = ::plru_bits[this];
	const size_t base = static_cast<size_t>(set) * (NUM_WAY - 1);

	uint32_t span  = NUM_WAY; // current subtree size
  	uint32_t node  = 0;       // current internal node index
  	uint32_t start = 0;       // first way index covered by current subtree
	
	while (span > 1) {
   		uint32_t half = span >> 1;

    		if (way < start + half) {
      			// Leaf is in left child; set bit so future evictions go to the RIGHT subtree
      			bits[base + node] = 1;
      			node = 2 * node + 1;   // descend left
      			// start unchanged
    		} 
		else {
      			// Leaf is in right child; set bit so future evictions go to the LEFT subtree
      			bits[base + node] = 0;
      			start += half;
      			node = 2 * node + 2;   // descend right
    		}
    		span = half;
  	}
}

void CACHE::replacement_final_stats() {}

