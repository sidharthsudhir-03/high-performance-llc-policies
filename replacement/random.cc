#include <algorithm>
#include <cassert>
#include <map>
#include <vector>
#include <random>

#include "cache.h"

namespace
{
static thread_local std::mt19937 rrp(std::random_device{}());
}

void CACHE::initialize_replacement() {}

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
	static thread_local std::uniform_int_distribution<uint32_t> range(0, NUM_WAY - 1);
	return (range(rrp));
}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
	
}

void CACHE::replacement_final_stats() {}
