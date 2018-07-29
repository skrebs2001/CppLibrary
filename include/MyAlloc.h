#pragma once

#include <vector>
#include <map>
#include <scoped_allocator>

// create a dynamic memory arena on the heap of N bytes
template <std::size_t N>
class HeapArena
{
public:
    using value_type = uint8_t;

    HeapArena()
    {
        m_pBuffer = static_cast<value_type*>(::operator new(N));
        m_pNext = m_pBuffer;
    }
    ~HeapArena()
    {
        ::operator delete(m_pBuffer);
        m_pBuffer = nullptr;
        m_pNext = nullptr;
    }
    HeapArena(const HeapArena&) = delete;
    HeapArena& operator=(const HeapArena&) = delete;

    static constexpr std::size_t size() { return N; }

    value_type* allocate(std::size_t n)
    {
        if (m_pNext + n <= m_pBuffer + N)
        {
            value_type *p = m_pNext;
            m_pNext += n;
            return p;
        }
        throw std::bad_alloc();
    }

    void deallocate(value_type* p, std::size_t n) const {}

private:
    value_type* m_pBuffer = nullptr;
    value_type* m_pNext = nullptr;
};

// create a memory arena on the stack of N bytes
template <std::size_t N>
class StackArena
{
public:
    using value_type = uint8_t;

    StackArena() {}
    ~StackArena() {}
    StackArena(const StackArena&) = delete;
    StackArena& operator=(const StackArena&) = delete;

    static constexpr std::size_t size() { return N; }

    value_type* allocate(std::size_t n)
    {
        if (m_nIndex + n > N)
        {
            throw std::bad_alloc();
        }
        value_type* pNewBuffer = &m_Buffer[m_nIndex];
        m_nIndex += n;
        return pNewBuffer;
    }
    void deallocate(value_type* p, std::size_t n) const {}

private:
    alignas(alignof(std::max_align_t)) value_type m_Buffer[N];
    std::size_t m_nIndex = 0;
};

// local allocator of T, maximum N bytes, default to 1KB
template <class T, std::size_t N = 1024, class Arena = StackArena<N> >
class LocalAllocator
{
public:
    using value_type = T;
    static auto constexpr element_size = sizeof(T);
    using arena_type = Arena;

private:
    using data_type = typename arena_type::value_type;
    arena_type& m_Arena;

public:
    LocalAllocator(const LocalAllocator&) = default;
    LocalAllocator& operator=(const LocalAllocator&) = delete;

    LocalAllocator(arena_type& a) noexcept : m_Arena(a) {}
    template <class U>
    LocalAllocator(const LocalAllocator<U, N, Arena>& a) noexcept : m_Arena(a.m_Arena) {}

    template <typename U> struct rebind { typedef LocalAllocator<U, N, Arena> other; };

    value_type* allocate(std::size_t n) { return reinterpret_cast<value_type*>(m_Arena.allocate(n * element_size)); }

    void deallocate(value_type* p, std::size_t n) { m_Arena.deallocate(reinterpret_cast<data_type*>(p), n * element_size); }

    template <class T1, std::size_t N1, class A1, class T2, std::size_t N2, class A2>
    friend bool operator==(const LocalAllocator<T1, N1, A2>& x, const LocalAllocator<T2, N2, A2>& y) noexcept;

    template <class T1, std::size_t N1, class A1, class T2, std::size_t N2, class A2>
    friend bool operator!=(const LocalAllocator<T1, N1, A1>& x, const LocalAllocator<T2, N2, A2>& y) noexcept;

    template<class U, std::size_t M, class A> friend class LocalAllocator;
};

template <class T1, std::size_t N1, class A1, class T2, std::size_t N2, class A2>
bool operator==(const LocalAllocator<T1, N1, A1>& x, const LocalAllocator<T2, N2, A2>& y) noexcept
{
    return (N1 == N2) && (&x.m_Arena == &y.m_Arena);
}

template <class T1, std::size_t N1, class A1, class T2, std::size_t N2, class A2>
bool operator!=(const LocalAllocator<T1, N1, A1>& x, const LocalAllocator<T2, N2, A1>& y) noexcept
{
    return !(x == y);
}

// local memory monotonic allocator of T, maximum N elements
template <class T, std::size_t N = 100>
class MyVector
{
public:
    static constexpr std::size_t buffer_size = N * sizeof(T);

    using vector_type = std::vector<T, LocalAllocator<T, buffer_size, HeapArena<buffer_size> > >;
    using value_type = typename vector_type::value_type;
    using size_type = typename vector_type::size_type;

    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;
    using reference = typename vector_type::reference;
    using const_reference = typename vector_type::const_reference;

    MyVector() {}
    ~MyVector() {}

    iterator begin() { return m_Vector.begin(); }
    const_iterator begin() const { return m_Vector.begin(); }
    iterator end() { return m_Vector.end(); }
    const_iterator end() const { return m_Vector.end(); }

    reference back() { return m_Vector.back(); }
    const_reference back() const { return m_Vector.back(); }
    bool empty() const { return m_Vector.empty(); }
    void push_back(const value_type& val) { m_Vector.push_back(val); }
    void reserve(size_type n) { m_Vector.reserve(n); }

    reference operator[](size_type n) { return m_Vector[n]; }
    const_reference operator[](size_type n) const { return m_Vector[n]; }

private:
    typename vector_type::allocator_type::arena_type m_Arena;
    vector_type m_Vector{ m_Arena };
};

template <class Key, class T, int N = 1000>
using MyStackMap = std::map<Key, T, std::less<Key>, LocalAllocator<std::pair<const Key, T>, N> >;

template <class T, std::size_t N = 1000>
using MyStackVector = std::vector<T, LocalAllocator<T, N> >;

template <std::size_t N = 1000>
using MyStackString = std::basic_string<char, std::char_traits<char>, LocalAllocator<char, N> >;

using MyScopedString = std::basic_string<char, std::char_traits<char>, LocalAllocator<char> >;
using MyScopedVector = std::vector<MyScopedString, std::scoped_allocator_adaptor<LocalAllocator<MyScopedString> > >;



template<class T>
void PrintIntVector(const T& v)
{
    for (auto& i : v)
    {
        Mercury::Trace("value %d\n", i);
    }
}

template<class T>
void PrintStringVector(const T& v)
{
    for (auto& i : v)
    {
        Mercury::Trace("value %s\n", i.c_str());
    }
}

template<class T>
void PrintIntMap(const T& v)
{
    for (auto& i : v)
    {
        Mercury::Trace("key %d  value %d\n", i.first, i.second);
    }
}

template<class T>
void PrintStringMap(const T& v)
{
    for (auto& i : v)
    {
        Mercury::Trace("key %s  value %s\n", i.first.c_str(), i.second.c_str());
    }
}

inline void VectorTest()
{
    //{
    //    MyVector<int> mIntVector;

    //    mIntVector.push_back(10);
    //    mIntVector.push_back(20);
    //    mIntVector.push_back(30);
    //    PrintIntVector(mIntVector);

    //    MyStackVector<int>::allocator_type::arena_type aInt;
    //    MyStackVector<int> mIntStackVector{ aInt };

    //    mIntStackVector.push_back(10);
    //    mIntStackVector.push_back(20);
    //    mIntStackVector.push_back(30);
    //    mIntStackVector.push_back(40);
    //    mIntStackVector.push_back(50);
    //    mIntStackVector.push_back(60);
    //    PrintIntVector(mIntStackVector);
    //}

    {
        MyStackString<>::allocator_type::arena_type aStringArena;
        MyStackString<> mString{ aStringArena };
        mString = "This is a test of StackString";
        mString = "A second test of the StackString class template";

        MyStackVector<MyStackString<> >::allocator_type::arena_type aString;
        MyStackVector<MyStackString<> > mStringVector{ aString };

        //mStringVector.push_back("hello");
        //mStringVector.push_back("there");
        //mStringVector.push_back("world");
        //mStringVector.push_back("hello  there  world!");
        //mStringVector.push_back("long string number two");
        //mStringVector.push_back("and this is the end");
        //PrintStringVector(mStringVector);
    }

    //{
    //    LocalAllocator<char, 1000>::arena_type scopedArena;
    //    LocalAllocator<char, 1000> scopedAllocator{ scopedArena };
    //    MyScopedVector v{ scopedAllocator };

    //    MyScopedString mScopedString{ scopedAllocator };
    //}

    Mercury::Trace("Vector test done\n");
}

template <class Map, class Key, class Value>
void insert(Map& m_Map, Key key, Value value)
{
    m_Map.insert(std::make_pair(key, value));
}

inline void MapTest()
{
    MyStackMap<int, int>::allocator_type::arena_type a;
    MyStackMap<int, int> mIntMap{ a };
    insert(mIntMap, 5, 10);
    insert(mIntMap, 15, 20);
    insert(mIntMap, 25, 30);
    PrintIntMap(mIntMap);

    MyStackMap<std::string, std::string> mStringMap{ a };
    insert(mStringMap, "hello", "world");
    PrintStringMap(mStringMap);

    Mercury::Trace("Done\n");
}
