#pragma once
#include "IteratorTraits.h"
#include "Range.h"

// clang-format off
namespace Range
{
// clang-format on

template <typename NullaryOperation>
class generate_iterator final
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = typename std::result_of<NullaryOperation()>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = const value_type&;

    explicit generate_iterator(NullaryOperation op, bool end)
        : m_op(op)
        , m_bEnd(end)
    {
    }

    // Iterator requires CopyConstructible, CopyAssignable, Destructible
    ~generate_iterator() = default;
    generate_iterator(const generate_iterator&) = default;
    generate_iterator& operator=(const generate_iterator&) = default;

    generate_iterator& operator++()
    {
        // invalidate the cache on increment
        m_bValid = false;
        return *this;
    }

    // Required by InputIterator
    value_type operator*()
    {
        // apply the operation when invalid and not at the end
        if (!m_bValid && !m_bEnd)
        {
            m_Result = m_op();
            m_bValid = true;
        }
        return m_Result;
    }

    bool operator==(const generate_iterator&) { return false; }
    bool operator!=(const generate_iterator& rhs) { return !(m_bEnd && rhs.m_bEnd); }

    // Required by ForwardIterator
    generate_iterator() = default;

    generate_iterator operator++(int)
    {
        generate_iterator temp(*this);
        ++(*this);
        return temp;
    }

private:
    NullaryOperation m_op{};

    // cache the current result to guarantee operator* returns same result across calls
    value_type m_Result{};

    // cache validation flag
    bool m_bValid = false;

    // flag to indicate if this is the end() iterator
    bool m_bEnd = false;
};

template <typename NullaryOperation>
generate_iterator<NullaryOperation> make_generate_iterator(NullaryOperation op, bool EndSentinel)
{
    static_assert(is_iterator<generate_iterator<NullaryOperation> >::value, "is_iterator failed");
    return generate_iterator<NullaryOperation>(op, EndSentinel);
}

template <typename NullaryOperation>
auto generate(NullaryOperation op) -> IteratorRange<generate_iterator<NullaryOperation> >
{
    return make_range(make_generate_iterator(op, false), make_generate_iterator(op, true));
}

template <typename NullaryOperation>
class generate_n_iterator final
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = typename std::result_of<NullaryOperation()>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = const value_type&;

    explicit generate_n_iterator(NullaryOperation op, size_t max, size_t current)
        : m_op(op)
        , m_uMaxSize(max)
        , m_uCurrent(current)
    {
    }

    // Iterator requires CopyConstructible, CopyAssignable, Destructible
    ~generate_n_iterator() = default;
    generate_n_iterator(const generate_n_iterator&) = default;
    generate_n_iterator& operator=(const generate_n_iterator&) = default;

    generate_n_iterator& operator++()
    {
        // invalidate the cache on increment
        m_bValid = false;

        ++m_uCurrent;
        return *this;
    }

    // Required by InputIterator
    value_type operator*()
    {
        // apply the operation when invalid and not at the end
        if (!m_bValid && (m_uCurrent < m_uMaxSize))
        {
            m_Result = m_op();
            m_bValid = true;
        }
        return m_Result;
    }

    bool operator==(const generate_n_iterator& rhs) { return m_uCurrent == rhs.m_uCurrent; }
    bool operator!=(const generate_n_iterator& rhs) { return !(*this == rhs); }

    // Required by ForwardIterator
    generate_n_iterator() = default;

    generate_n_iterator operator++(int)
    {
        generate_n_iterator temp(*this);
        ++(*this);
        return temp;
    }

private:
    NullaryOperation m_op{};

    // cache the current result to guarantee operator* returns same result across calls
    value_type m_Result{};

    // cache validation flag
    bool m_bValid = false;

    // maximum number of elements to generate
    size_t m_uMaxSize = 0;

    // current generated element
    size_t m_uCurrent = 0;
};

template <typename NullaryOperation>
generate_n_iterator<NullaryOperation> make_generate_n_iterator(NullaryOperation op, size_t n, size_t current)
{
    static_assert(is_iterator<generate_n_iterator<NullaryOperation> >::value, "is_iterator failed");
    return generate_n_iterator<NullaryOperation>(op, n, current);
}

template <typename NullaryOperation>
auto generate_n(NullaryOperation op, size_t n) -> IteratorRange<generate_n_iterator<NullaryOperation> >
{
    return make_range(make_generate_n_iterator(op, n, 0), make_generate_n_iterator(op, n, n));
}

}  // namespace Range
