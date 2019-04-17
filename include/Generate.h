#pragma once
#include "IteratorTraits.h"
#include "Range.h"

// clang-format off
namespace Range
{
// clang-format on

template <typename NullaryOperation>
class generate_iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::result_of<NullaryOperation()>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = const value_type&;

    generate_iterator() = default;

    explicit generate_iterator(NullaryOperation op, bool end)
        : m_op(op)
        , m_end(end)
    {
    }

    // Required by Iterator
    generate_iterator& operator++()
    {
        m_valid = false;
        return *this;
    }

    // Required by InputIterator
    bool operator==(const generate_iterator& rhs) { return false; }
    bool operator!=(const generate_iterator& rhs) { return !(m_end && rhs.m_end); }

    value_type operator*()
    {
        if (!m_valid && !m_end)
        {
            m_result = m_op();
            m_valid = true;
        }
        return m_result;
    }

private:
    NullaryOperation m_op;
    value_type m_result{};
    bool m_valid = false;
    bool m_end = false;
};

template <typename NullaryOperation>
generate_iterator<NullaryOperation> make_generate_iterator(NullaryOperation op, bool EndSentinel)
{
    static_assert(is_iterator<generate_iterator<NullaryOperation> >::value, "is_iterator");
    return generate_iterator<NullaryOperation>(op, EndSentinel);
}

template <typename NullaryOperation>
auto generate(NullaryOperation op) -> IteratorRange<generate_iterator<NullaryOperation> >
{
    return make_range(make_generate_iterator(op, false), make_generate_iterator(op, true));
}

template <typename NullaryOperation>
class generate_n_iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::result_of<NullaryOperation()>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = const value_type&;

    generate_n_iterator() = default;

    explicit generate_n_iterator(NullaryOperation op, size_t n, size_t current)
        : m_op(op)
        , m_size(n)
        , m_current(current)
    {
    }

    // Required by Iterator
    generate_n_iterator& operator++()
    {
        m_valid = false;
        ++m_current;
        return *this;
    }

    // Required by InputIterator
    bool operator==(const generate_n_iterator& rhs) { return m_current == rhs.m_current; }
    bool operator!=(const generate_n_iterator& rhs) { return !(*this == rhs); }

    value_type operator*()
    {
        if (!m_valid && (m_current < m_size))
        {
            m_result = m_op();
            m_valid = true;
        }
        return m_result;
    }

private:
    NullaryOperation m_op;
    value_type m_result{};
    bool m_valid = false;
    size_t m_size = 0;
    size_t m_current = 0;
};

template <typename NullaryOperation>
generate_n_iterator<NullaryOperation> make_generate_n_iterator(NullaryOperation op, size_t n, size_t current)
{
    static_assert(is_iterator<generate_n_iterator<NullaryOperation> >::value, "is_iterator");
    return generate_n_iterator<NullaryOperation>(op, n, current);
}

template <typename NullaryOperation>
auto generate_n(NullaryOperation op, size_t n) -> IteratorRange<generate_n_iterator<NullaryOperation> >
{
    return make_range(make_generate_n_iterator(op, n, 0), make_generate_n_iterator(op, n, n));
}

}  // namespace Range
