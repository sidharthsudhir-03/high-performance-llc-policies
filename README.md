# High-Performance Cache Replacement Policies for Last-Level Caches

Implementation and evaluation of multiple cache replacement policies for the Last-Level Cache (LLC) using the ChampSim microarchitecture simulator. This project explores the impact of cache replacement algorithms on processor performance by analyzing IPC, MPKI, and cache hit rates across a diverse set of benchmark workloads.

---

## Overview

Modern processors rely heavily on efficient cache hierarchies to bridge the performance gap between CPU cores and main memory. The effectiveness of a cache is strongly influenced by its replacement policy, which determines which cache line should be evicted when new data must be inserted.

This project investigates several cache replacement strategies ranging from simple baseline approaches to adaptive policies that dynamically respond to workload behavior. Each policy was implemented within the ChampSim simulation framework and evaluated using multiple benchmark traces representative of real-world applications.

Performance was analyzed using Instructions Per Cycle (IPC), Misses Per Kilo Instructions (MPKI), and Last-Level Cache hit rates.

---

## Objectives

* Implement multiple LLC replacement policies within ChampSim
* Analyze cache behavior across diverse workloads
* Compare performance using IPC, MPKI, and hit-rate metrics
* Study the trade-offs between replacement complexity and performance
* Evaluate adaptive insertion policies for modern processor cache hierarchies

---

## Implemented Policies

### Random

A baseline replacement policy that selects a victim cache line uniformly at random.

**Characteristics**

* Minimal hardware overhead
* No reuse tracking
* Serves as a performance baseline

---

### Least Frequently Used (LFU)

Tracks cache-line access frequency using saturating counters and evicts the least frequently accessed cache line.

**Characteristics**

* Captures long-term reuse patterns
* Favors frequently accessed data
* Additional metadata overhead

---

### Pseudo-Least Recently Used (pLRU)

Tree-based approximation of LRU that significantly reduces replacement-state overhead while preserving similar replacement behavior.

**Characteristics**

* Lower hardware cost than true LRU
* Efficient implementation for high associativity caches
* Widely used in commercial processors

---

### Bimodal Insertion Policy (BIP)

Inserts most incoming cache lines near the LRU position while occasionally inserting at MRU.

**Characteristics**

* Reduces cache pollution
* Improves cache residency for frequently reused data
* Simple adaptive behavior

---

### Dynamic Insertion Policy (DIP)

Uses set dueling and a global policy selector to dynamically choose between LRU-style and BIP-style insertion behavior.

**Characteristics**

* Workload-aware adaptation
* Balances recency and pollution resistance
* Representative of modern adaptive cache-management techniques

---

## Experimental Methodology

### Simulator

* ChampSim Trace-Driven Simulator

### Metrics

* Instructions Per Cycle (IPC)
* Misses Per Kilo Instructions (MPKI)
* LLC Hit Rate

### Workloads

Performance was evaluated using multiple benchmark traces including:

* astar
* gcc
* gobmk
* hmmer
* lbm
* mcf
* milc
* wrf

The use of diverse workloads enables comparison across both memory-intensive and compute-intensive applications.

---

## Performance Summary

Average performance relative to the baseline LRU policy:

| Policy | IPC Improvement |
| ------ | --------------- |
| Random | -1.40%          |
| pLRU   | -0.16%          |
| LFU    | +0.26%          |
| BIP    | +0.50%          |
| DIP    | +0.19%          |

---

## Key Findings

* Random replacement consistently underperformed due to the lack of locality awareness.
* pLRU achieved performance close to LRU while requiring significantly less replacement-state storage.
* LFU improved cache retention for frequently reused data but occasionally suffered from stale history effects.
* BIP delivered the strongest average IPC improvement by reducing cache pollution from low-reuse cache lines.
* DIP successfully adapted to varying workload characteristics and maintained strong overall performance.

---

## Technologies Used

### Computer Architecture

* Cache Hierarchies
* Last-Level Cache Design
* Cache Replacement Policies
* Adaptive Insertion Policies
* Memory-System Performance Analysis

### Software Development

* C++
* STL
* Data Structures
* Algorithm Design

### Simulation and Evaluation

* ChampSim
* Trace-Driven Simulation
* Performance Modeling
* Benchmark Analysis

---

## Repository Structure

```text
high-performance-llc-policies/
├── replacement/
│   ├── random.cc
│   ├── lfu.cc
│   ├── plru.cc
│   ├── bip.cc
│   └── dip.cc
│
├── reports/
│   ├── performance_summary.md
│   └── project_report.pdf
│
├── results/
│   ├── ipc_comparison.png
│   ├── mpki_comparison.png
│   └── hitrate_comparison.png
│
└── README.md
```

---

## Learning Outcomes

* Cache hierarchy design
* Memory-system optimization
* Replacement-policy implementation
* Adaptive hardware algorithms
* Trace-driven architectural simulation
* Performance evaluation and benchmarking
* Computer architecture research methodologies
* Quantitative performance analysis
