#pragma once

#include <algorithm>
#include <functional>

namespace range
{
    template <typename T>
    using Range = std::vector<T>;

    class reverse
    {
    public:
        template <typename T>
        Range<T> operator()(Range<T> vSrc)
        {
            std::reverse(vSrc.begin(), vSrc.end());
            return vSrc;
        }
    };

    class sort
    {
    public:
        template <typename T>
        Range<T> operator()(Range<T> vSrc)
        {
            std::sort(vSrc.begin(), vSrc.end());
            return vSrc;
        }
    };

    template <typename T>
    class transform
    {
        T (*m_op)(T);

    public:
        template <typename UnaryFunction>
        explicit transform(UnaryFunction op) : m_op(op)
        {}

        Range<T> operator()(Range<T> vSrc)
        {
            std::transform(vSrc.begin(), vSrc.end(), vSrc.begin(), m_op);
            return vSrc;
        }
    };

    template <typename T>
    Range<T> operator|(Range<T> lhs, reverse op)
    {
        return op(lhs);
    }

    template <typename T>
    Range<T> operator|(Range<T> lhs, sort op)
    {
        return op(lhs);
    }

    template <typename T>
    Range<T> operator|(Range<T> lhs, transform<T> op)
    {
        return op(lhs);
    }
}

namespace RangeTest
{
    void RunRangeTest()
    {
        using IntRange = range::Range<int>;

        auto Mult = [](int x) { return x + 3; };

        IntRange Input1 = { 4, 1, 2, 6, 5, 3 };
        IntRange Output = Input1 | range::sort() | range::reverse() | range::transform<int>(Mult);

        printf("done");
    }
}