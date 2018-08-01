#pragma once

#include <random>

#if defined(_WIN32)
#undef min
#undef max
#endif

template <typename Tcontext>
class UniformRandomBitGenerator
{
public:
    UniformRandomBitGenerator(const UniformRandomBitGenerator&) = delete;
    UniformRandomBitGenerator(const UniformRandomBitGenerator&&) = delete;
    UniformRandomBitGenerator& operator=(const UniformRandomBitGenerator&) = delete;
    UniformRandomBitGenerator& operator=(const UniformRandomBitGenerator&&) = delete;

    explicit UniformRandomBitGenerator(Tcontext& pGameContext)
        : m_pContext(pGameContext)
    {
    }

    using result_type = size_t;
    static constexpr result_type interval = 0x7fffffff;
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return interval; }

    result_type operator()() const
    {
        result_type uRetval = 0;
        m_pContext.GetRandomNumber(max(), uRetval);
        return uRetval;
    }

private:
    Tcontext& m_pContext;
};

class DefaultRandomEngine
{
public:
    // Returns random value between 0 and uInterval - 1
    size_t operator()(size_t uInterval) const
    {
        std::random_device rnd;
        std::default_random_engine generator{ rnd() };
        std::uniform_int_distribution<size_t> distribution(0, uInterval - 1);
        return distribution(generator);
    }
};

class RandomDevice
{
public:
    // Returns random value between 0 and uInterval - 1
    size_t operator()(size_t uInterval) const
    {
        std::random_device rnd;
        return rnd() % uInterval;
    }
};

class Rand
{
public:
    // Returns random value between 0 and uInterval - 1
    size_t operator()(size_t uInterval) const { return rand() % uInterval; }
};
