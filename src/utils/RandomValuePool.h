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

// https://isocpp.org/blog/2017/01/cpp-weekly-episode-44-constexpr-compile-time-randomjason-turner
template<typename Type, std::size_t Size>
struct CompileTimeRandomValuePool
{

    static constexpr auto seed()
    {
        std::uint64_t shifted = 0;

        for (const auto c : __TIME__)
        {
            shifted <<= 8;
            shifted |= c;
        }

        return shifted;
    }

    struct PCG
    {
        struct pcg32_random_t { std::uint64_t state = 0;  std::uint64_t inc = seed(); };
        pcg32_random_t rng;
        typedef std::uint32_t result_type;

        constexpr result_type operator()()
        {
            return pcg32_random_r();
        }

        static result_type constexpr min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static result_type constexpr max()
        {
            return std::numeric_limits<result_type>::min();
        }

    private:
        constexpr std::uint32_t pcg32_random_r()
        {
            std::uint64_t oldstate = rng.state;
            // Advance internal state
            rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
            // Calculate output function (XSH RR), uses old state for max ILP
            std::uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
            std::uint32_t rot = oldstate >> 59u;
            return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        }

    };

    static constexpr auto get_random(int count)
    {
        PCG pcg;
        while (count > 0) {
            pcg();
            --count;
        }

        return pcg();
    }

    static constexpr auto get(Type minValue = static_cast<Type>(0), Type maxValue = static_cast<Type>(100)) -> std::array<Type, Size>
    {
        std::array<Type, Size> v{};
        for (auto i = 0; i < Size; i++)
        {
            v[i] = get_random(i);
        }
        return v;
    }
};

