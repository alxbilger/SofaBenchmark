#pragma once

#include <cstddef>
#include <random>
#include <array>

template<typename Type, std::size_t Size>
struct RandomValuePool
{
    static auto get(Type minValue = static_cast<Type>(0), Type maxValue = static_cast<Type>(100)) -> const std::array<Type, Size>&
    {
        if (!bGenerated)
        {
            std::uniform_real_distribution<float> rand(minValue, maxValue);
            std::random_device seed;
            std::mt19937 gen(seed());
            std::generate(v.begin(), v.end(), [&]() { return static_cast<Type>(rand(gen)); });
            bGenerated = true;
        }
        return v;
    }
private:
    static inline bool bGenerated{ false };
    static inline std::array<Type, Size> v{};
};
