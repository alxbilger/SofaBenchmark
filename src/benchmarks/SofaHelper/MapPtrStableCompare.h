#pragma once

#include <benchmark/benchmark.h>

/// 3 benchmark functions to compare insertion of pairs of pointers into different associative arrays:
/// * sofa::helper::map_ptr_stable_compare
/// * std::map
/// * std::unordered_map + a custom hash function


static void BM_MapPtrStableCompare_insert(benchmark::State& state);
static void BM_StdMap_insert(benchmark::State& state);
static void BM_StdUnorderedMap_insert(benchmark::State& state);

BENCHMARK(BM_MapPtrStableCompare_insert)->RangeMultiplier(2)->Range(8, 1<<10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_StdMap_insert)             ->RangeMultiplier(2)->Range(8, 1<<10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_StdUnorderedMap_insert)    ->RangeMultiplier(2)->Range(8, 1<<10)->Unit(benchmark::kMicrosecond);