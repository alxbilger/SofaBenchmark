#include <benchmark/benchmark.h>

#include <sofa/core/MultiVecId.h>
using sofa::core::MultiVecCoordId;

#include <sofa/core/BaseState.h>

static void fooByValue(MultiVecCoordId id)
{
    id.getName();
}

static void fooByReference(const MultiVecCoordId& id)
{
    id.getName();
}

static void BM_PassMultivecidByValue(benchmark::State& state);
static void BM_PassMultivecidByReference(benchmark::State& state);

BENCHMARK(BM_PassMultivecidByValue);
BENCHMARK(BM_PassMultivecidByReference);

void BM_PassMultivecidByValue(benchmark::State& state)
{
    auto pos = sofa::core::vec_id::write_access::position;
    for (auto _ : state)
    {
        fooByValue(pos);
    }
}

void BM_PassMultivecidByReference(benchmark::State& state)
{
    auto pos = sofa::core::vec_id::write_access::position;
    for (auto _ : state)
    {
        fooByReference(pos);
    }
}
