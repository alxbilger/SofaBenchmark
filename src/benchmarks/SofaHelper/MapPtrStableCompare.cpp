#include "MapPtrStableCompare.h"

#include <sofa/helper/map_ptr_stable_compare.h>

void BM_MapPtrStableCompare_insert(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        sofa::helper::map_ptr_stable_compare< std::pair< int*, int* >, int* > map;

        std::vector<int> integers(state.range(0));

        state.ResumeTiming();

        for (size_t i = 0; i < integers.size(); ++i)
        {
            for (size_t j = 0; j < integers.size(); ++j)
            {
                const auto p = std::make_pair<int*, int*>(&integers[i], &integers[j]);
                map.insert(std::make_pair(p, static_cast<int*>(nullptr)));
            }
        }
    }
}

void BM_StdMap_insert(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        std::map< std::pair< int*, int* >, int* > map;

        std::vector<int> integers(state.range(0));

        state.ResumeTiming();

        for (size_t i = 0; i < integers.size(); ++i)
        {
            for (size_t j = 0; j < integers.size(); ++j)
            {
                const auto p = std::make_pair<int*, int*>(&integers[i], &integers[j]);
                map.insert(std::make_pair(p, static_cast<int*>(nullptr)));
            }
        }
    }
}

void BM_StdUnorderedMap_insert(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        const auto hash = [](const std::pair< int*, int* >& p)
        {
            return (std::hash<int*>()(p.first)) ^ (std::hash<int*>()(p.second) << 32);
        };
        std::unordered_map< std::pair< int*, int* >, int*, decltype(hash) > map(10, hash);

        std::vector<int> integers(state.range(0));

        state.ResumeTiming();

        for (size_t i = 0; i < integers.size(); ++i)
        {
            for (size_t j = 0; j < integers.size(); ++j)
            {
                const auto p = std::make_pair<int*, int*>(&integers[i], &integers[j]);
                map.insert(std::make_pair(p, static_cast<int*>(nullptr)));
            }
        }
    }
}
