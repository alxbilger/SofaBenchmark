#include <benchmark/benchmark.h>
#include <sofa/core/objectmodel/Data.h>


template<class T>
static void BM_Data_RawGetValue(benchmark::State& state)
{
    T d;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(d);
    }
}

template<class T>
static void BM_Data_SharedGetValue(benchmark::State& state)
{
    std::shared_ptr<T> d = std::make_shared<T>();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(*d);
    }
}

template<class T>
static void BM_Data_UpdateIfDirty(benchmark::State& state)
{
    sofa::core::objectmodel::Data<T> d;
    for (auto _ : state)
    {
        d.updateIfDirty();
    }
}

template<class T>
static void BM_Data_DataGetValue(benchmark::State& state)
{
    sofa::core::objectmodel::Data<T> d;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(d.getValue());
    }
}

BENCHMARK_TEMPLATE(BM_Data_RawGetValue, int);
BENCHMARK_TEMPLATE(BM_Data_RawGetValue, float);
BENCHMARK_TEMPLATE(BM_Data_RawGetValue, sofa::type::vector<double>);

BENCHMARK_TEMPLATE(BM_Data_SharedGetValue, int);
BENCHMARK_TEMPLATE(BM_Data_SharedGetValue, float);
BENCHMARK_TEMPLATE(BM_Data_SharedGetValue, sofa::type::vector<double>);

BENCHMARK_TEMPLATE(BM_Data_UpdateIfDirty, int);
BENCHMARK_TEMPLATE(BM_Data_UpdateIfDirty, float);
BENCHMARK_TEMPLATE(BM_Data_UpdateIfDirty, sofa::type::vector<double>);

BENCHMARK_TEMPLATE(BM_Data_DataGetValue, int);
BENCHMARK_TEMPLATE(BM_Data_DataGetValue, float);
BENCHMARK_TEMPLATE(BM_Data_DataGetValue, sofa::type::vector<double>);
