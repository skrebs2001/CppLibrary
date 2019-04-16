#pragma once
#include "Range.h"

namespace Range
{
template <typename BaseIterator, typename UnaryOperation>
class transform_iterator
{
public:
    using iterator_category = typename std::iterator_traits<BaseIterator>::iterator_category;
    using value_type = typename std::result_of<UnaryOperation(typename std::iterator_traits<BaseIterator>::value_type)>::type;
    using difference_type = typename std::iterator_traits<BaseIterator>::difference_type;
    using pointer = value_type*;
    using reference = value_type&;

    transform_iterator() {}

    explicit transform_iterator(BaseIterator iterator, UnaryOperation op)
        : m_iterator(iterator)
        , m_op(op)
    {
    }

    // Required by Iterator
    transform_iterator& operator++()
    {
        ++m_iterator;
        return *this;
    }

    // Required by InputIterator
    value_type operator*() { return m_op(*m_iterator); }

    bool operator==(const transform_iterator& rhs) { return m_iterator == rhs.m_iterator; }
    bool operator!=(const transform_iterator& rhs) { return m_iterator != rhs.m_iterator; }

    transform_iterator operator++(int)
    {
        transform_iterator temp(*this);
        ++(*this);
        return temp;
    }

    // Required by BidirectionalIterator
    transform_iterator& operator--()
    {
        --m_iterator;
        return *this;
    }

    transform_iterator operator--(int)
    {
        transform_iterator temp(*this);
        --(*this);
        return temp;
    }

    // Required by RandomAccessIterator

    // b - a
    difference_type operator-(const transform_iterator& rhs) { return m_iterator - rhs.m_iterator; }

    // a - n
    transform_iterator operator-(difference_type n) { return transform_iterator(m_iterator - n, m_op); }

    // a + n
    transform_iterator operator+(difference_type n) { return transform_iterator(m_iterator + n, m_op); }

    // n + a
    friend transform_iterator operator+(difference_type n, const transform_iterator& rhs)
    {
        return transform_iterator(rhs.m_iterator + n, rhs.m_op);
    }

    // r += n
    transform_iterator& operator+=(difference_type n)
    {
        m_iterator += n;
        return *this;
    }

    // r -= n
    transform_iterator& operator-=(difference_type n)
    {
        m_iterator -= n;
        return *this;
    }

    // a[n]
    value_type operator[](size_t n) const { return m_op(*(m_iterator + n)); }

    friend bool operator<(const transform_iterator& lhs, const transform_iterator& rhs) { return lhs.m_iterator < rhs.m_iterator; }
    friend bool operator>(const transform_iterator& lhs, const transform_iterator& rhs) { return rhs.m_iterator < lhs.m_iterator; }
    friend bool operator<=(const transform_iterator& lhs, const transform_iterator& rhs) { return !(rhs.m_iterator < lhs.m_iterator); }
    friend bool operator>=(const transform_iterator& lhs, const transform_iterator& rhs) { return !(lhs.m_iterator < rhs.m_iterator); }

private:
    BaseIterator m_iterator;
    UnaryOperation m_op;
};

template <typename BaseIterator, typename UnaryOperation>
transform_iterator<BaseIterator, UnaryOperation> make_transform_iterator(BaseIterator iterator, UnaryOperation op)
{
    return transform_iterator<BaseIterator, UnaryOperation>(iterator, op);
}

template <typename UnaryOperation>
class unary_operation
{
public:
    explicit unary_operation(UnaryOperation op)
        : m_op(op)
    {
    }

    UnaryOperation get() const { return m_op; }

private:
    UnaryOperation m_op;
};

// Construct transform adaptor from the operation
template <typename UnaryOperation>
unary_operation<UnaryOperation> transform(UnaryOperation op)
{
    return unary_operation<UnaryOperation>(op);
}

// Construct view by applying transform adaptor to the range
template <typename Range, typename UnaryOperation>
auto operator|(const Range& range, unary_operation<UnaryOperation> transformer)
    -> IteratorRange<transform_iterator<typename Range::const_iterator, UnaryOperation> >
{
    return make_range(make_transform_iterator(range.begin(), transformer.get()), make_transform_iterator(range.end(), transformer.get()));
}

// Construct view by applying transform adaptor to the range
template <typename Iterator, typename UnaryOperation>
auto operator|(const IteratorRange<Iterator>& range, unary_operation<UnaryOperation> transformer)
    -> IteratorRange<transform_iterator<Iterator, UnaryOperation> >
{
    return make_range(make_transform_iterator(range.begin(), transformer.get()), make_transform_iterator(range.end(), transformer.get()));
}

}  // namespace Range

