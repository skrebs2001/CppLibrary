#pragma once
#include "IteratorTraits.h"
#include "Range.h"
#include "View.h"

// clang-format off
namespace Range
{
template <typename BaseIterator, typename UnaryPredicate>
class filter_iterator final
{
    static_assert(is_input_iterator<BaseIterator>::value, "filter_iterator requires input iterator");

    UnaryPredicate m_pred{};
    BaseIterator m_iterator{};
    BaseIterator m_first{};
    BaseIterator m_last{};

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

    explicit filter_iterator(BaseIterator first, BaseIterator last, UnaryPredicate pred)
        : m_pred(pred)
        , m_iterator(first)
        , m_first(first)
        , m_last(last)
    {
        increment_predicate();
    }

    // Iterator requires CopyConstructible, CopyAssignable, Destructible
    ~filter_iterator() = default;
    filter_iterator(const filter_iterator&) = default;
    filter_iterator& operator=(const filter_iterator& rhs)
    {
        m_iterator = rhs.m_iterator;
        m_first = rhs.m_first;
        m_last = rhs.m_last;
        m_pred = rhs.m_pred;
        return *this;
    }

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
    filter_iterator() = default;

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
    static_assert(is_iterator<filter_iterator<BaseIterator, UnaryPredicate> >::value, "is_iterator failed");
    return filter_iterator<BaseIterator, UnaryPredicate>(first, last, pred);
}

// clang-format off
template <typename UnaryPredicate>
class unary_predicate : public detail::holder<UnaryPredicate>
{
public:
    explicit unary_predicate(UnaryPredicate pred) : detail::holder<UnaryPredicate>(pred) {}
};
// clang-format on

// Construct filter adaptor from the predicate
template <typename UnaryPredicate>
unary_predicate<UnaryPredicate> filter(UnaryPredicate pred)
{
    return unary_predicate<UnaryPredicate>(pred);
}

// Construct view by applying filter adaptor to the range
template <typename Rng, typename UnaryPredicate>
auto operator|(const Rng& range, unary_predicate<UnaryPredicate> filter)
    -> IteratorRange<filter_iterator<typename Rng::const_iterator, UnaryPredicate> >
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
