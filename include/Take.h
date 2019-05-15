#pragma once
#include "IteratorTraits.h"
#include "Range.h"
#include "View.h"

// clang-format off
namespace Range
{
// clang-format on

template <typename BaseIterator>
class take_iterator final
{
    static_assert(is_input_iterator<BaseIterator>::value, "take_iterator requires input iterator");

    BaseIterator m_iterator{};
    size_t m_size = 0;
    size_t m_current = 0;

public:
    // TODO: the iterator category should probably adapt to the base iterator, as filter does
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::iterator_traits<BaseIterator>::value_type;
    using difference_type = typename std::iterator_traits<BaseIterator>::difference_type;
    using pointer = typename std::iterator_traits<BaseIterator>::pointer;
    using reference = const typename std::iterator_traits<BaseIterator>::reference;

    // operator* returns reference when base iterator returns reference, otherwise returns value_type
    // this is to accomodate applying take to a transform, whose operator* returns by value
    using DereferenceType = typename std::conditional<std::is_reference<decltype(*m_iterator)>::value, reference, value_type>::type;

    explicit take_iterator(BaseIterator iter, size_t n, size_t current)
        : m_iterator(iter)
        , m_size(n)
        , m_current(current)
    {
    }

    // Iterator requires CopyConstructible, CopyAssignable, Destructible
    ~take_iterator() = default;
    take_iterator(const take_iterator&) = default;
    take_iterator& operator=(const take_iterator&) = default;

    take_iterator& operator++()
    {
        ++m_iterator;
        ++m_current;
        return *this;
    }

    // Required by InputIterator
    DereferenceType operator*() { return *m_iterator; }

    bool operator==(const take_iterator& rhs) { return (m_current == rhs.m_current) || (m_iterator == rhs.m_iterator); }
    bool operator!=(const take_iterator& rhs) { return !(*this == rhs); }

    // Required by ForwardIterator
    take_iterator() = default;

    take_iterator operator++(int)
    {
        take_iterator temp(*this);
        ++(*this);
        return temp;
    }
};

template <typename BaseIterator>
take_iterator<BaseIterator> make_take_iterator(BaseIterator iter, size_t n, size_t current)
{
    static_assert(is_iterator<take_iterator<BaseIterator> >::value, "is_iterator failed");
    return take_iterator<BaseIterator>(iter, n, current);
}

// clang-format off
class take_holder : public detail::holder<size_t>
{
public:
    explicit take_holder(size_t n) : detail::holder<size_t>(n) {}
};
// clang-format on

inline take_holder take(size_t n)
{
    return take_holder(n);
}

// Construct view by applying take adaptor to the range
template <typename Rng>
auto operator|(const Rng& range, take_holder taker) -> IteratorRange<take_iterator<typename Rng::const_iterator> >
{
    return make_range(make_take_iterator(range.begin(), taker.get(), 0), make_take_iterator(range.end(), taker.get(), taker.get()));
}

// Construct view by applying take adaptor to the range
template <typename Iterator>
auto operator|(const IteratorRange<Iterator>& range, take_holder taker) -> IteratorRange<take_iterator<Iterator> >
{
    return make_range(make_take_iterator(range.begin(), taker.get(), 0), make_take_iterator(range.end(), taker.get(), taker.get()));
}

}  // namespace Range
