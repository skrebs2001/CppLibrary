#pragma once
#include "IteratorTraits.h"
#include "Range.h"
#include "View.h"

// clang-format off
namespace Range
{
// clang-format on

template <typename BaseIterator, typename UnaryOperation>
class transform_iterator final
{
    static_assert(is_input_iterator<BaseIterator>::value, "transform_iterator requires input iterator");

public:
    using iterator_category = typename std::iterator_traits<BaseIterator>::iterator_category;
    using value_type = typename std::result_of<UnaryOperation(typename std::iterator_traits<BaseIterator>::value_type)>::type;
    using difference_type = typename std::iterator_traits<BaseIterator>::difference_type;
    using pointer = value_type*;
    using reference = const value_type&;

    explicit transform_iterator(BaseIterator iterator, UnaryOperation op)
        : m_op(op)
        , m_iterator(iterator)
    {
    }

    // Iterator requires CopyConstructible, CopyAssignable, Destructible
    ~transform_iterator() = default;
    transform_iterator(const transform_iterator&) = default;
    transform_iterator& operator=(const transform_iterator& rhs)
    {
        m_op = rhs.m_op;
        m_iterator = rhs.m_iterator;
        m_result = rhs.m_result;
        m_valid = rhs.m_valid;

        return *this;
    }

    transform_iterator& operator++()
    {
        ++m_iterator;
        m_valid = false;
        return *this;
    }

    // Required by InputIterator
    reference operator*()
    {
        if (!m_valid)
        {
            m_result = m_op(*m_iterator);
            m_valid = true;
        }
        return m_result;
    }

    bool operator==(const transform_iterator& rhs) { return m_iterator == rhs.m_iterator; }
    bool operator!=(const transform_iterator& rhs) { return m_iterator != rhs.m_iterator; }

    // Required by ForwardIterator
    transform_iterator() = default;

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
        m_valid = false;
        return *this;
    }

    transform_iterator operator--(int)
    {
        transform_iterator temp(*this);
        --(*this);
        return temp;
    }

    // Required by RandomAccessIterator

    // r += n
    transform_iterator& operator+=(difference_type n)
    {
        m_iterator += n;
        m_valid = false;
        return *this;
    }

    // a + n
    transform_iterator operator+(difference_type n)
    {
        transform_iterator temp(*this);
        return temp += n;
    }

    // n + a
    friend transform_iterator operator+(difference_type n, const transform_iterator& rhs)
    {
        transform_iterator temp(rhs);
        return temp += n;
    }

    // r -= n
    transform_iterator& operator-=(difference_type n) { return *this += -n; }

    // a - n
    transform_iterator operator-(difference_type n)
    {
        transform_iterator temp(*this);
        return temp -= n;
    }

    // b - a
    difference_type operator-(const transform_iterator& rhs) { return m_iterator - rhs.m_iterator; }

    // a[n]
    value_type operator[](size_t n) const { return m_op(*(m_iterator + n)); }

    friend bool operator<(const transform_iterator& lhs, const transform_iterator& rhs) { return lhs.m_iterator < rhs.m_iterator; }
    friend bool operator>(const transform_iterator& lhs, const transform_iterator& rhs) { return rhs < lhs; }
    friend bool operator<=(const transform_iterator& lhs, const transform_iterator& rhs) { return !(rhs < lhs); }
    friend bool operator>=(const transform_iterator& lhs, const transform_iterator& rhs) { return !(lhs < rhs); }

private:
    UnaryOperation m_op{};

    BaseIterator m_iterator{};

    // cache the current result to allow operator* to return by reference
    value_type m_result{};

    // cache validation flag
    bool m_valid = false;
};

template <typename BaseIterator, typename UnaryOperation>
transform_iterator<BaseIterator, UnaryOperation> make_transform_iterator(BaseIterator iterator, UnaryOperation op)
{
    static_assert(is_iterator<transform_iterator<BaseIterator, UnaryOperation> >::value, "is_iterator failed");
    return transform_iterator<BaseIterator, UnaryOperation>(iterator, op);
}

// clang-format off
template <typename UnaryOperation>
class unary_operation : public detail::holder<UnaryOperation>
{
public:
    explicit unary_operation(UnaryOperation op) : detail::holder<UnaryOperation>(op) {}
};
// clang-format on

// Construct transform adaptor from the operation
template <typename UnaryOperation>
unary_operation<UnaryOperation> transform(UnaryOperation op)
{
    return unary_operation<UnaryOperation>(op);
}

// Construct view by applying transform adaptor to the range
template <typename Rng, typename UnaryOperation>
auto operator|(const Rng& range, unary_operation<UnaryOperation> transformer)
    -> IteratorRange<transform_iterator<typename Rng::const_iterator, UnaryOperation> >
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
