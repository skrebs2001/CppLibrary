#pragma once

#include <random>
#include "./Server/IGameContext.h"
#include "Types.h"

#if defined(_WIN32) && defined(min)
#undef min
#endif

class UniformRandomBitGenerator
{
public:
    UniformRandomBitGenerator() = default;
    ~UniformRandomBitGenerator() = default;
    UniformRandomBitGenerator(const UniformRandomBitGenerator&) = delete;
    UniformRandomBitGenerator& operator=(const UniformRandomBitGenerator&) = delete;

    explicit UniformRandomBitGenerator(Aristocrat::GDK::EGM::Server::IGameContext& pGameContext)
        : m_pContext(pGameContext)
    {
    }

    using result_type = Aristocrat::GDK::uint32;
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
    Aristocrat::GDK::EGM::Server::IGameContext& m_pContext;
};

class DefaultRandomEngine
{
public:
    // Returns random value between 0 and uInterval - 1
    Aristocrat::GDK::uint32 operator()(Aristocrat::GDK::uint32 uInterval) const
    {
        std::random_device rnd;
        std::default_random_engine generator{ rnd() };
        std::uniform_int_distribution<Aristocrat::GDK::uint32> distribution(0, uInterval - 1);
        return distribution(generator);
    }
};

class RandomDevice
{
public:
    // Returns random value between 0 and uInterval - 1
    Aristocrat::GDK::uint32 operator()(Aristocrat::GDK::uint32 uInterval) const
    {
        std::random_device rnd;
        return rnd() % uInterval;
    }
};

class Rand
{
public:
    // Returns random value between 0 and uInterval - 1
    Aristocrat::GDK::uint32 operator()(Aristocrat::GDK::uint32 uInterval) const { return rand() % uInterval; }
};
