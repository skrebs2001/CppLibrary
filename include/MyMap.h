#pragma once

#include <map>
#include <vector>

template <class T, int N>
class Arena
{
public:
    using value_type = T;

    value_type* allocate(std::size_t n)
    {
        value_type* pNewBuffer = &m_Buffer[m_nIndex];
        m_nIndex += n;
        return pNewBuffer;
    }
    void deallocate(value_type* p, std::size_t n) {}

private:
    value_type m_Buffer[N];
    std::size_t m_nIndex = 0;
};

template <class T, std::size_t N>
class LocalAllocator
{
public:
    using value_type = T;
    using arena_type = Arena<T, N>;

private:
    arena_type& m_Arena;

public:
    LocalAllocator(const LocalAllocator&) = default;
    LocalAllocator& operator=(const LocalAllocator&) = delete;

    LocalAllocator(arena_type& a) noexcept : m_Arena(a) {}
    template <class U>
    LocalAllocator(const LocalAllocator<U, N>& a) noexcept : m_Arena(a.m_Arena) {}

    template <typename U> struct rebind { typedef LocalAllocator<U, N> other; };

    value_type* allocate(std::size_t n) { return m_Arena.allocate(n); }

    void deallocate(value_type* p, std::size_t n) { m_Arena.deallocate(p, n); }

    template <class T1, std::size_t N1, class T2, std::size_t N2>
    friend bool operator==(const LocalAllocator<T1, N1>& x, const LocalAllocator<T2, N2>& y) noexcept;

    template <class T1, std::size_t N1, class T2, std::size_t N2>
    friend bool operator!=(const LocalAllocator<T1, N1>& x, const LocalAllocator<T2, N2>& y) noexcept;

    template<class U, std::size_t M> friend class LocalAllocator;
};

template <class T1, std::size_t N1, class T2, std::size_t N2>
bool operator==(const LocalAllocator<T1, N1>& x, const LocalAllocator<T2, N2>& y) noexcept
{
    return (N1 == N2) && (&x.m_Arena == &y.m_Arena);
}

template <class T1, std::size_t N1, class T2, std::size_t N2>
bool operator!=(const LocalAllocator<T1, N1>& x, const LocalAllocator<T2, N2>& y) noexcept
{
    return !(x == y);
}

//template <class Key, class T, int N = 100>
//using MyStackMap = std::map<Key, T, std::less<Key>, StackAllocator<std::pair<const Key, T>, N> >;

template <class T, int N = 100>
using MyStackVector = std::vector<T, LocalAllocator<T, N> >;

inline void xxx()
{
    MyStackVector<int>::allocator_type::arena_type a;
    MyStackVector<int> v{a};
}
#define BUFFER 1

template <class T, int N>
class MonotonicAllocator
{
public:
    using value_type = T;

    template <typename U>
    struct rebind
    {
        typedef MonotonicAllocator<U, N> other;
    };

    MonotonicAllocator() noexcept {}

    template <class U>
    explicit MonotonicAllocator(const MonotonicAllocator<U, N>& alloc) noexcept
    {
    }

    ~MonotonicAllocator() noexcept
    {
#if BUFFER
        if (m_pBegin)
        {
            ::operator delete(static_cast<void*>(m_pBegin));
            m_pBegin = nullptr;
        }
#endif
    }

    void deallocate(value_type* p, std::size_t n)
    {
#if BUFFER
        if (m_nSize > 0)
        {
            if (p < m_pBegin || p >= m_pEnd)
            {
                DebugBreak();
            }
        }
#else
        ::operator delete(static_cast<void*>(p));
#endif
    }

    value_type* allocate(std::size_t n)
    {
#if BUFFER
        if (m_nCount + n > N)
        {
            throw std::bad_alloc();
        }

        AllocPool();
        return AllocBlock(n);
#else
        m_nCount += n;
        return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
#endif
    }

private:
    void AllocPool()
    {
        if (m_pBegin == nullptr)
        {
            m_pNext = m_pBegin = static_cast<value_type*>(::operator new(N * sizeof(value_type)));
            m_pEnd = m_pBegin + N;
        }
    }

    value_type* AllocBlock(std::size_t n)
    {
        value_type* retval = m_pNext;
        m_pNext += n;
        m_nCount += n;
        return retval;
    }

    value_type* m_pBegin = nullptr;
    value_type* m_pEnd = nullptr;
    value_type* m_pNext = nullptr;
    std::size_t m_nSize = sizeof(value_type);
    std::size_t m_nCount = 0;
};

template <class T, class U, int N>
bool operator==(MonotonicAllocator<T, N> const&, MonotonicAllocator<U, N> const&) noexcept
{
    return true;
}

template <class T, class U, int N>
bool operator!=(MonotonicAllocator<T, N> const& x, MonotonicAllocator<U, N> const& y) noexcept
{
    return !(x == y);
}

template <class Key, class T, int N = 100>
using MyMap = std::map<Key, T, std::less<Key>, MonotonicAllocator<std::pair<const Key, T>, N> >;

template <class T, int N = 100>
using MyVector = std::vector<T, MonotonicAllocator<T, N> >;

typedef MyMap<std::string, std::string> stringMap;
typedef MyMap<int, int> intMap;

template <class Map, class Key, class Value>
void insert(Map& m_Map, Key key, Value value)
{
    m_Map.insert(std::make_pair(key, value));
}

inline void MapTest()
{
    Arena<int, 50> mArena;

    //intMap mIntMap;
    //insert(mIntMap, 5, 10);
    //insert(mIntMap, 15, 20);
    //insert(mIntMap, 25, 30);

    //for (auto& i : mIntMap)
    //{
    //    Mercury::Trace("key %d  value %d\n", i.first, i.second);
    //}

    //MyVector<int> mIntVector;
    //mIntVector.push_back(10);
    //mIntVector.push_back(20);
    //mIntVector.push_back(30);

    //MyVector<int> mCopy(mIntVector);
    //mCopy.push_back(40);

    //for (auto& i : mCopy)
    //{
    //    Mercury::Trace("value %d\n", i);
    //}

    //stringMap mStringMap;
    //insert(mStringMap, "hello", "world");

    //for (auto& i : mStringMap)
    //{
    //    Mercury::Trace("key %s  value %s\n", i.first.c_str(), i.second.c_str());
    //}

    Mercury::Trace("Done\n");
}
