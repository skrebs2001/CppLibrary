#pragma once
#include <type_traits>

template <typename T>
struct is_iterator
{
    static_assert(std::is_copy_constructible<T>::value, "is_copy_constructible failed");
    static_assert(std::is_copy_assignable<T>::value, "is_copy_assignable failed");
    static_assert(std::is_destructible<T>::value, "is_destructible failed");

    static const bool value = true;
};

// Test whether T is an input iterator
template <typename T>
struct is_input_iterator
    : std::integral_constant<bool, std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>
{
};

// Test whether T is a forward iterator
template <typename T>
struct is_forward_iterator
    : std::integral_constant<bool, std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>
{
};

// Test whether T is a bidirectional iterator
template <typename T>
struct is_bidirectional_iterator
    : std::integral_constant<bool,
                             std::is_base_of<std::bidirectional_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>
{
};

// Test whether T is a random access iterator
template <typename T>
struct is_random_access_iterator
    : std::integral_constant<bool,
                             std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>
{
};

// Test whether Rng is a bidirectional range
template <typename Rng>
struct is_bidirectional_range
    : std::integral_constant<bool, std::is_base_of<std::bidirectional_iterator_tag,
                                                   typename std::iterator_traits<typename Rng::iterator>::iterator_category>::value>
{
};
