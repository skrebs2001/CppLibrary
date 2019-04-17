#pragma once

#include <type_traits>

template <typename T>
struct is_iterator
{
    static_assert(std::is_copy_constructible<T>::value, "is_copy_constructible");
    static_assert(std::is_copy_assignable<T>::value, "is_copy_assignable");
    static_assert(std::is_destructible<T>::value, "is_destructible");

    static const bool value = true;
};

// Test whether T is at least a forward iterator
template <typename T>
struct is_forward_iterator
    : std::integral_constant<bool, std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>
{
};
