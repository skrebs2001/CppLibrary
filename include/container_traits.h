#pragma once
#include <vector>
#include <deque>
#include <list>
#include "eastl/vector.h"
#include "eastl/list.h"
#include "eastl/iterator.h"

// clang-format off
template <typename T>
struct is_std_sequence_container
{
    static const bool value = false;
};

template <typename T, typename Alloc>
struct is_std_sequence_container<std::vector<T, Alloc>>
{
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_std_sequence_container<std::deque<T, Alloc>>
{
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_std_sequence_container<std::list<T, Alloc>>
{
    static const bool value = true;
};

//-----------------------------------------

template <typename T>
struct is_eastl_sequence_container
{
    static const bool value = false;
};

template <typename T, typename Alloc>
struct is_eastl_sequence_container<eastl::vector<T, Alloc>>
{
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_eastl_sequence_container<eastl::list<T, Alloc>>
{
    static const bool value = true;
};

template <typename T, typename Alloc>
struct is_sequence_container : std::integral_constant<bool, is_std_sequence_container<T, Alloc>::value ||
                                                            is_eastl_sequence_container<T, Alloc>::value>
{
};
// clang-format on

template <typename... Ts>
struct make_void
{
    typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;
