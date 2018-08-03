#pragma once

#include <algorithm>
#include <array>
#include <forward_list>
#include <functional>
#include <list>
#include <vector>
#include <Debug.h>

namespace range {

template <typename T>
using Range = std::vector<T>;

template <typename T>
class copy_if
{
    bool (*m_pred)(T);

public:
    template <typename UnaryPredicate>
    explicit copy_if(UnaryPredicate pred)
        : m_pred(pred)
    {
    }

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
    explicit transform(UnaryFunction op)
        : m_op(op)
    {
    }

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

namespace RangeTest {

inline void RunRangeTest()
{
    using IntRange = range::Range<int>;

    auto Mult = [](int x) { return x * 3; };
    auto IsEven = [](int x) { return x % 2 == 0; };

    IntRange Input1 = { 4, 1, 2, 6, 5, 3 };
    IntRange Output = Input1 | range::sort() | range::reverse() | range::transform<int>(Mult) | range::copy_if<int>(IsEven);

    printf("done");
}
}  // namespace RangeTest
}  // namespace range

// clang-format off
template <typename T>
struct is_sequence_container
{
    static const bool value = false;
};

template <typename T, typename Alloc>
struct is_sequence_container<std::vector<T, Alloc>>
{
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_sequence_container<std::deque<T, Alloc>>
{
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_sequence_container<std::list<T, Alloc>>
{
    static const bool value = true;
};
// clang-format on


namespace range2 {

template <typename T>
class Range
{
    //static_assert(is_sequence_container<T>::value, "T must be a sequence container");

    using value_type = T;
    using container_type = std::vector<value_type>;
    using iter_type = typename container_type::iterator;

public:
    Range() = default;

    // Initializer list constructor, construct a container with a copy of each element from the list
    Range(std::initializer_list<value_type> il)
        : m_c(il.begin(), il.end())
    {
    }

    Range& operator=(const Range& rng)
    {
        m_c = rng.m_c;
        return *this;
    }

    Range(const Range& rng) : m_c(rng.m_c)
    {
    }

    Range(const Range&& rng) noexcept : m_c(rng.m_c)
    {
    }

    iter_type begin() { return m_c.begin(); }
    iter_type end() { return m_c.end(); }

private:
    container_type m_c;
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

template <typename T>
Range<T> operator|(Range<T> lhs, reverse op)
{
    return op(lhs);
}

template <typename T>
Range<T> make_range()
{
    return Range<T>{ { 1, 2, 3, 4, 5 } };
}

inline void Range2Test()
{
    Range<int> intRange{ { 1, 2, 3, 4, 5 } };

    auto r1 = intRange;

    Range<int> r2;
    r2 = r1;

    Range<int> r3 = make_range<int>();

    auto Mult = [](int x) { return x * 3; };
    auto IsEven = [](int x) { return x % 2 == 0; };

    auto Output = intRange | range2::reverse();

    for (auto i = Output.begin(); i != Output.end(); ++i)
    {
        Mercury::Trace("%d\n", *i);
    }

    printf("done");
}

}  // namespace range2
