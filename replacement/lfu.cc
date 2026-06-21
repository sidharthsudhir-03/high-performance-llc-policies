#include <algorithm>
#include <cassert>
#include <map>
#include <vector>
#include <random>

#include "cache.h"

namespace
{

// 4-bit counter
std::map<CACHE*, std::vector<uint8_t>> frequency_counter;

// Random number generator for candidate eviction, if there is a tie in frequency
static thread_local std::mt19937 gen(std::random_device{}());

// Saturated counter; does not refresh once counter hits maximum value
inline uint8_t saturated_counter(uint8_t x) {
  return (x < 15) ? (x + 1) : x;
}

}

void CACHE::initialize_replacement() {

        ::frequency_counter[this] = std::vector<uint8_t>(NUM_SET * NUM_WAY);

}

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
        auto& vec = ::frequency_counter[this];
        const size_t base = static_cast<size_t>(set) * NUM_WAY;

	// Traversing through the whole set to check for minimum value, and the same non-zero frequency
        uint8_t first = vec[base];
        bool all_equal = true;
        bool all_nonzero = (first != 0);
        uint8_t min_val = first;
        for (uint32_t way = 1; way < NUM_WAY; ++way) {
                uint8_t v = vec[base + way];
                if (v != first) all_equal = false;
                if (v == 0)     all_nonzero = false;
                if (v < min_val) min_val = v;
        }

	
	// Initializing all counters to 0 if all the counters have the same non-zero frequency
        if (all_equal && all_nonzero) {
                for (uint32_t way = 0; way < NUM_WAY; ++way) vec[base+way] = 0;
                min_val = 0;
        }

	// Selecting candidates for eviction
        std::vector<uint32_t> candidates;
        for (uint32_t way = 0; way < NUM_WAY; ++way)
                if (vec[base + way] == min_val) candidates.push_back(way);
        assert(!candidates.empty());

        std::uniform_int_distribution<uint32_t> dist(0, static_cast<uint32_t>(candidates.size() - 1));
        return (candidates.size() == 1) ? candidates[0] : candidates[dist(gen)];
}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
        auto& vec = ::frequency_counter[this];
        const size_t cache_line_index = static_cast<size_t>(set) * NUM_WAY + way;
        const uint8_t current = vec[cache_line_index];
	
	// If it's a cache hit, then update the counter; else, cache line is updated and counter is reset to 0
        if (hit){
                vec[cache_line_index] = saturated_counter(current);
        } else {
                vec[cache_line_index] = 0;
        }
}

void CACHE::replacement_final_stats() {}



