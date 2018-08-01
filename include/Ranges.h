#pragma once

#include <algorithm>
#include <functional>

namespace range
{
    template <typename T>
    using Range = std::vector<T>;

    template <typename T>
    class copy_if
    {
        bool (*m_pred)(T);

    public:
        template <typename UnaryPredicate>
        explicit copy_if(UnaryPredicate pred) : m_pred(pred)
        {}

        Range<T> operator()(Range<T> vSrc)
        {
            Range<T> vOut;
            std::copy_if(vSrc.begin(), vSrc.end(), std::back_inserter(vOut), m_pred);
            return vOut;
        }
    };

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
    Range<T> operator|(Range<T> lhs, copy_if<T> op)
    {
        return op(lhs);
    }

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

        auto Mult = [](int x) { return x * 3; };
        auto IsEven = [](int x) { return x % 2 == 0; };

        IntRange Input1 = { 4, 1, 2, 6, 5, 3 };
        IntRange Output = Input1 | range::sort() | range::reverse() | range::transform<int>(Mult) | range::copy_if<int>(IsEven);

        printf("done");
    }
}