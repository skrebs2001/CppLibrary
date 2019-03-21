#pragma once
#include "Utility.h"

namespace Adaptor
{
template <typename BidirectionalIterator, typename UnaryPredicate>
class filter_iterator
{
    static_assert(
        std::is_base_of<std::bidirectional_iterator_tag, typename std::iterator_traits<BidirectionalIterator>::iterator_category>::value,
        "filter_iterator requires bidirectional iterator");

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename std::iterator_traits<BidirectionalIterator>::value_type;
    using difference_type = typename std::iterator_traits<BidirectionalIterator>::difference_type;
    using pointer = typename std::iterator_traits<BidirectionalIterator>::pointer;
    using reference = typename std::iterator_traits<BidirectionalIterator>::reference;

    filter_iterator() {}

    explicit filter_iterator(BidirectionalIterator first, BidirectionalIterator last, UnaryPredicate pred)
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
    auto operator*() { return *m_iterator; }

    bool operator==(const filter_iterator& rhs) { return m_iterator == rhs.m_iterator; }
    bool operator!=(const filter_iterator& rhs) { return !(*this == rhs); }

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

    BidirectionalIterator m_iterator;
    BidirectionalIterator m_first;
    BidirectionalIterator m_last;
    UnaryPredicate m_pred;
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

template <typename UnaryPredicate>
unary_predicate<UnaryPredicate> filter(UnaryPredicate pred)
{
    return unary_predicate<UnaryPredicate>(pred);
}

template <typename Range, typename UnaryPredicate>
auto operator|(const Range& range, unary_predicate<UnaryPredicate> filter)
    -> IteratorRange<filter_iterator<typename Range::const_iterator, UnaryPredicate> >
{
    return make_range(make_filter_iterator(range.begin(), range.end(), filter.get()),
                      make_filter_iterator(range.end(), range.end(), filter.get()));
}

template <typename Iterator, typename UnaryPredicate>
auto operator|(const IteratorRange<Iterator>& range, unary_predicate<UnaryPredicate> filter)
    -> IteratorRange<filter_iterator<Iterator, UnaryPredicate> >
{
    return make_range(make_filter_iterator(range.begin(), range.end(), filter.get()),
                      make_filter_iterator(range.end(), range.end(), filter.get()));
}

//template <typename Range, typename UnaryPredicate>
//auto operator|(const Range& range, unary_predicate<UnaryPredicate> filter)
//{
//    return make_range(make_filter_iterator(range.begin(), range.end(), filter.get()),
//                      make_filter_iterator(range.end(), range.end(), filter.get()));
//}

}
