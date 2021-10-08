#include <benchmark/benchmark.h>
#include <unordered_map>
#include <sofa/helper/map_ptr_stable_compare.h>

/// 3 benchmark functions to compare insertion of pairs of pointers into different associative arrays:
/// * sofa::helper::map_ptr_stable_compare
/// * std::map
/// * std::unordered_map + a custom hash function
void BM_MapPtrStableCompare_insert(benchmark::State& state);
void BM_StdMap_insert(benchmark::State& state);
void BM_StdUnorderedMap_insert(benchmark::State& state);

void BM_MapPtrStableCompare_insert_duplicate(benchmark::State& state);

void BM_MapPtrStableCompare_iterate(benchmark::State& state);

void BM_MapPtrStableCompare_erase(benchmark::State& state);

BENCHMARK(BM_MapPtrStableCompare_insert) ->RangeMultiplier(2)->Ranges({{8, 1<<10}, {1, 2}})->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_StdMap_insert)              ->RangeMultiplier(2)->Ranges({{8, 1<<10}, {1, 2}})->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_StdUnorderedMap_insert)     ->RangeMultiplier(2)->Ranges({{8, 1<<10}, {1, 2}})->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_MapPtrStableCompare_iterate)->RangeMultiplier(2)->Range(8, 1<<10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_MapPtrStableCompare_erase)  ->RangeMultiplier(2)->Range(8, 1<<10)->Unit(benchmark::kMicrosecond);

void BM_MapPtrStableCompare_insert(benchmark::State &state)
{
    std::vector<int> integers(state.range(0));
    for (auto _ : state)
    {
        state.PauseTiming();
        sofa::helper::map_ptr_stable_compare< std::pair< int*, int* >, int* > map;
        state.ResumeTiming();

        for (int n = 0; n < state.range(1); ++n)
        {
            for (size_t i = 0; i < integers.size(); ++i)
            {
                for (size_t j = 0; j < integers.size(); ++j)
                {
                    const auto p = std::make_pair<int *, int *>(&integers[i], &integers[j]);
                    map.insert(map.end(), std::make_pair(p, static_cast<int *>(nullptr)));
                }
            }
        }
    }
}

void BM_StdMap_insert(benchmark::State &state)
{
    std::vector<int> integers(state.range(0));

    for (auto _ : state)
    {
        state.PauseTiming();
        std::map< std::pair< int*, int* >, int* > map;
        state.ResumeTiming();

        for (int n = 0; n < state.range(1); ++n)
        {
            for (size_t i = 0; i < integers.size(); ++i)
            {
                for (size_t j = 0; j < integers.size(); ++j)
                {
                    const auto p = std::make_pair<int *, int *>(&integers[i], &integers[j]);
                    map.insert(std::make_pair(p, static_cast<int *>(nullptr)));
                }
            }
        }
    }
}

void BM_StdUnorderedMap_insert(benchmark::State &state)
{
    std::vector<int> integers(state.range(0));

    const auto hash = [](const std::pair< int*, int* >& p)
    {
        return (std::hash<int*>()(p.first)) ^ (std::hash<int*>()(p.second) << 32);
    };

    for (auto _ : state)
    {
        state.PauseTiming();
        std::unordered_map< std::pair< int*, int* >, int*, decltype(hash) > map(10, hash);
        state.ResumeTiming();

        for (int n = 0; n < state.range(1); ++n)
        {
            for (size_t i = 0; i < integers.size(); ++i)
            {
                for (size_t j = 0; j < integers.size(); ++j)
                {
                    const auto p = std::make_pair<int *, int *>(&integers[i], &integers[j]);
                    map.insert(std::make_pair(p, static_cast<int *>(nullptr)));
                }
            }
        }
    }
}

void BM_MapPtrStableCompare_iterate(benchmark::State &state)
{
    sofa::helper::map_ptr_stable_compare< std::pair< int*, int* >, int* > map;

    std::vector<int> integers(state.range(0));

    for (size_t i = 0; i < integers.size(); ++i)
    {
        for (size_t j = 0; j < integers.size(); ++j)
        {
            const auto p = std::make_pair<int*, int*>(&integers[i], &integers[j]);
            map.insert(std::make_pair(p, static_cast<int*>(nullptr)));
        }
    }

    for (auto _ : state)
    {
        for (auto it = map.cbegin(); it != map.cend(); ++it)
        {
            benchmark::DoNotOptimize(*it);
        }
    }
}

void BM_MapPtrStableCompare_erase(benchmark::State &state)
{
    sofa::helper::map_ptr_stable_compare< std::pair< int*, int* >, int > map;

    std::vector<int> integers(state.range(0));

    for (size_t i = 0; i < integers.size(); ++i)
    {
        for (size_t j = 0; j < integers.size(); ++j)
        {
            const auto p = std::make_pair<int*, int*>(&integers[i], &integers[j]);
            map.insert(std::make_pair(p, i * integers.size() + j));
        }
    }

    for (auto _ : state)
    {
        for (auto it = map.cbegin(); it != map.cend();)
        {
            if (it->second % 4 == 0)
            {
                map.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }
}