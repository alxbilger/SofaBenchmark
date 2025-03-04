#include <benchmark/benchmark.h>

#include <sofa/core/objectmodel/Data.h>

template<class T>
static void BM_Data_GetValue(benchmark::State& state);

template<class T>
static void BM_Data_ReadAccessor_ImplicitConversion(benchmark::State& state);

template<class T>
static void BM_Data_ReadAccessor_ExplicitConversion(benchmark::State& state);

template<class T>
static void BM_Data_ReadAccessor_StarOperatorConversion(benchmark::State& state);

BENCHMARK_TEMPLATE(BM_Data_GetValue, int);
BENCHMARK_TEMPLATE(BM_Data_GetValue, sofa::type::vector<int>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});
BENCHMARK_TEMPLATE(BM_Data_GetValue, sofa::type::vector<sofa::type::Vec3>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});

BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_ImplicitConversion, int);
BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_ImplicitConversion, sofa::type::vector<int>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});
BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_ImplicitConversion, sofa::type::vector<sofa::type::Vec3>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});

BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_ExplicitConversion, int);
BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_ExplicitConversion, sofa::type::vector<int>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});
BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_ExplicitConversion, sofa::type::vector<sofa::type::Vec3>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});

BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_StarOperatorConversion, int);
BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_StarOperatorConversion, sofa::type::vector<int>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});
BENCHMARK_TEMPLATE(BM_Data_ReadAccessor_StarOperatorConversion, sofa::type::vector<sofa::type::Vec3>)->RangeMultiplier(10)->Ranges({{1e4, 1e6}});

template<class T>
void BM_Data_GetValue(benchmark::State& state)
{
    sofa::core::objectmodel::Data<T> data;

    if constexpr (sofa::type::trait::is_vector<T>)
    {
        auto d = sofa::helper::getWriteOnlyAccessor(data);
        d.resize(state.range(0));
    }

    for (auto _ : state)
    {
        const T& t = data.getValue();
        benchmark::DoNotOptimize(t);
    }
}

template<class T>
void BM_Data_ReadAccessor_ImplicitConversion(benchmark::State& state)
{
    sofa::core::objectmodel::Data<T> data;

    if constexpr (sofa::type::trait::is_vector<T>)
    {
        auto wa = sofa::helper::getWriteOnlyAccessor(data);
        wa.resize(state.range(0));
    }

    for (auto _ : state)
    {
        auto ra = sofa::helper::getReadAccessor(data);
        const T& t = ra;
        benchmark::DoNotOptimize(t);
    }
}

template<class T>
void BM_Data_ReadAccessor_ExplicitConversion(benchmark::State& state)
{
    sofa::core::objectmodel::Data<T> data;

    if constexpr (sofa::type::trait::is_vector<T>)
    {
        auto wa = sofa::helper::getWriteOnlyAccessor(data);
        wa.resize(state.range(0));
    }

    for (auto _ : state)
    {
        auto ra = sofa::helper::getReadAccessor(data);
        const T& t = static_cast<const T&>(ra);
        benchmark::DoNotOptimize(t);
    }
}

template<class T>
void BM_Data_ReadAccessor_StarOperatorConversion(benchmark::State& state)
{
    sofa::core::objectmodel::Data<T> data;

    if constexpr (sofa::type::trait::is_vector<T>)
    {
        auto wa = sofa::helper::getWriteOnlyAccessor(data);
        wa.resize(state.range(0));
    }

    for (auto _ : state)
    {
        auto ra = sofa::helper::getReadAccessor(data);
        const T& t = *ra;
        benchmark::DoNotOptimize(t);
    }
}