#pragma once

#include <benchmark/benchmark.h>

///Evaluates the overhead of a call to AdvancedTimer
static void BM_AdvancedTimer_begin_end(benchmark::State& state);
BENCHMARK(BM_AdvancedTimer_begin_end);