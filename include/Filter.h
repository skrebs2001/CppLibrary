#pragma once
#include "Range.h"

namespace Range
{

namespace detail
{
// Empty class that matches all other types
template <typename T>
struct filter_iterator_category { };
// clang-format on

// Random access iterator tag maps to bidirectional
template <>
struct filter_iterator_category<std::random_access_iterator_tag>
{
    using type = std::bidirectional_iterator_tag;
};

// Bidirectional iterator tag maps to bidirectional
template <>
struct filter_iterator_category<std::bidirectional_iterator_tag>
{
    using type = std::bidirectional_iterator_tag;
};

// Forward iterator tag maps to forward
template <>
struct filter_iterator_category<std::forward_iterator_tag>
{
    using type = std::forward_iterator_tag;
};

// Test whether T is at least a forward iterator
template <typename T>
struct is_forward_iterator
    : std::integral_constant<bool, std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>
{
};

}  // namespace detail

template <typename BaseIterator, typename UnaryPredicate>
class filter_iterator
{
    static_assert(detail::is_forward_iterator<BaseIterator>::value, "filter_iterator requires forward iterator");

    BaseIterator m_iterator;
    BaseIterator m_first;
    BaseIterator m_last;
    UnaryPredicate m_pred;

public:
    using iterator_category =
        typename detail::filter_iterator_category<typename std::iterator_traits<BaseIterator>::iterator_category>::type;
    using value_type = typename std::iterator_traits<BaseIterator>::value_type;
    using difference_type = typename std::iterator_traits<BaseIterator>::difference_type;
    using pointer = typename std::iterator_traits<BaseIterator>::pointer;
    using reference = typename std::iterator_traits<BaseIterator>::reference;

    // operator* returns reference when base iterator returns reference, otherwise returns value_type
    // this is to accomodate applying a filter to a transform, whose operator* returns by value
    using DereferenceType = typename std::conditional<std::is_reference<decltype(*m_iterator)>::value, reference, value_type>::type;

    filter_iterator() {}

    explicit filter_iterator(BaseIterator first, BaseIterator last, UnaryPredicate pred)
        : m_iterator(first)
        , m_first(first)
        , m_last(last)
        , m_pred(pred)
    {
        increment_predicate();
    }

    // Required by Iterator
    filter_iterator& operator++()
    {
        ++m_iterator;
        increment_predicate();
        return *this;
    }

    // Required by InputIterator
    DereferenceType operator*() { return *m_iterator; }

    bool operator==(const filter_iterator& rhs) { return m_iterator == rhs.m_iterator; }
    bool operator!=(const filter_iterator& rhs) { return m_iterator != rhs.m_iterator; }

    // Required by ForwardIterator
    filter_iterator operator++(int)
    {
        filter_iterator temp(*this);
        ++(*this);
        return temp;
    }

    // Required by BidirectionalIterator
    filter_iterator& operator--()
    {
        --m_iterator;
        decrement_predicate();
        return *this;
    }

    filter_iterator operator--(int)
    {
        filter_iterator temp(*this);
        --(*this);
        return temp;
    }

private:
    void increment_predicate()
    {
        for (; m_iterator != m_last && !m_pred(*m_iterator); ++m_iterator)
        {
        }
    }

    void decrement_predicate()
    {
        for (; m_iterator != m_first && !m_pred(*m_iterator); --m_iterator)
        {
        }
    }
};

template <typename BaseIterator, typename UnaryPredicate>
filter_iterator<BaseIterator, UnaryPredicate> make_filter_iterator(BaseIterator first, BaseIterator last, UnaryPredicate pred)
{
    return filter_iterator<BaseIterator, UnaryPredicate>(first, last, pred);
}

template <typename UnaryPredicate>
class unary_predicate
{
public:
    explicit unary_predicate(UnaryPredicate pred)
        : m_pred(pred)
    {
    }

    UnaryPredicate get() const { return m_pred; }

private:
    UnaryPredicate m_pred;
};

// Construct filter adaptor from the predicate
template <typename UnaryPredicate>
unary_predicate<UnaryPredicate> filter(UnaryPredicate pred)
{
    return unary_predicate<UnaryPredicate>(pred);
}

// Construct view by applying filter adaptor to the range
template <typename Range, typename UnaryPredicate>
auto operator|(const Range& range, unary_predicate<UnaryPredicate> filter)
    -> IteratorRange<filter_iterator<typename Range::const_iterator, UnaryPredicate> >
{
    return make_range(make_filter_iterator(range.begin(), range.end(), filter.get()),
                      make_filter_iterator(range.end(), range.end(), filter.get()));
}

// Construct view by applying filter adaptor to the range
template <typename Iterator, typename UnaryPredicate>
auto operator|(const IteratorRange<Iterator>& range, unary_predicate<UnaryPredicate> filter)
    -> IteratorRange<filter_iterator<Iterator, UnaryPredicate> >
{
    return make_range(make_filter_iterator(range.begin(), range.end(), filter.get()),
                      make_filter_iterator(range.end(), range.end(), filter.get()));
}

}  // namespace Range

