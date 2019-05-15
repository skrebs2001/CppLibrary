#pragma once
#include "Range.h"

// clang-format off
namespace Range
{
struct reverser {};
inline struct reverser reverse() { return reverser{}; }
// clang-format on

// Returns a range view by applying the reverse iterators returned by the source range
template <typename Rng>
auto operator|(const Rng& range, struct reverser) -> IteratorRange<decltype(range.rbegin())>
{
    return make_range(range.rbegin(), range.rend());
}

// Returns a range view by constructing a pair of reverse iterators from the [begin, end) range
template <typename Iterator>
auto operator|(const IteratorRange<Iterator>& range, struct reverser) -> IteratorRange<std::reverse_iterator<Iterator> >
{
    return make_range(std::reverse_iterator<Iterator>(range.end()), std::reverse_iterator<Iterator>(range.begin()));
}

}  // namespace Range
