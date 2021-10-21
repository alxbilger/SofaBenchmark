#pragma once

#include <cstddef>
#include <random>
#include <array>

template<typename Type, std::size_t Size>
struct RandomValuePool
{
    static auto get() -> const std::array<Type, Size>&
    {
        if (!bGenerated)
        {
            std::uniform_real_distribution<float> rand(0, 100);
            std::random_device seed;
            std::mt19937 gen(seed());
            std::generate(v.begin(), v.end(), [&]() { return rand(gen); });
            bGenerated = true;
        }
        return v;
    }
private:
    static inline bool bGenerated{ false };
    static inline std::array<Type, Size> v{};
};
