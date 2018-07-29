#pragma once

#include <eastl/fixed_vector.h>
#include <map>
#include "RandomEngine.h"

template <typename T, size_t Texclude_size, typename Tgenerator, typename Tcontainer>
class base_container
{
public:
    using value_type = T;
    using container_type = Tcontainer;
    using generator_type = Tgenerator;

    container_type m_vContainer;
    generator_type m_gen;

    base_container() = default;
    virtual ~base_container() = default;

    template <typename InputIterator>
    base_container(InputIterator first, InputIterator last)
        : m_vContainer(first, last)
    {
    }

    base_container(std::initializer_list<value_type> il)
        : m_vContainer(il.begin(), il.end())
    {
    }

    template <typename Val>
    void push_back(Val&& val)
    {
        m_vContainer.push_back(std::forward<Val>(val));
    }

    // Returns the exclude size
    constexpr size_t ExcludeSize() const { return Texclude_size; }

    // Returns the number of elements in the container
    virtual size_t Size() const { return m_vContainer.size(); }

    // Returns the next element in the container
    virtual value_type GetNext() = 0;
};

template <typename T, size_t Texclude_size, typename Tgenerator, typename Tcontainer>
class random_container_insert : public base_container<T, Texclude_size, Tgenerator, Tcontainer>
{
public:
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::m_gen;
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::m_vContainer;
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::ExcludeSize;
    using value_type = T;

    friend bool operator==(const random_container_insert& lhs, const random_container_insert& rhs)
    {
        return (lhs.m_vContainer == rhs.m_vContainer) && (lhs.m_vExcludeQueue == rhs.m_vExcludeQueue);
    }

    friend bool operator!=(const random_container_insert& lhs, const random_container_insert& rhs) { return !(lhs == rhs); }

    template <typename InputIterator>
    random_container_insert(InputIterator first, InputIterator last)
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>(first, last)
    {
    }

    random_container_insert(std::initializer_list<value_type> il)
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>(il)
    {
    }

    random_container_insert() = default;

    size_t Size() const override { return m_vContainer.size() + m_vExcludeQueue.size(); }

    value_type GetNext() override
    {
        assert((m_vContainer.size() > 0) || (m_vExcludeQueue.size() > ExcludeSize()));

        if (m_vExcludeQueue.size() > ExcludeSize())
        {
            // The front element has exceeded the exclude count, move it back to the container
            m_vContainer.push_back(m_vExcludeQueue.front());
            m_vExcludeQueue.erase(m_vExcludeQueue.begin());
        }

        // Chose random index of next element, get a copy to return
        size_t nextIndex = m_gen(m_vContainer.size());
        value_type nextElement = m_vContainer[nextIndex];

        // Add it to the exclude queue, then remove it from the container
        m_vExcludeQueue.push_back(nextElement);
        m_vContainer.erase(m_vContainer.begin() + nextIndex);

        return nextElement;
    }

private:
    eastl::fixed_vector<value_type, Texclude_size + 1> m_vExcludeQueue;
};

template <typename T, size_t Texclude_size, typename Tgenerator, typename Tcontainer>
class random_container_swap : public base_container<T, Texclude_size, Tgenerator, Tcontainer>
{
public:
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::m_gen;
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::m_vContainer;
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::ExcludeSize;
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::Size;
    using container_type = typename base_container<T, Texclude_size, Tgenerator, Tcontainer>::container_type;
    using value_type = T;

    friend bool operator==(const random_container_swap& lhs, const random_container_swap& rhs)
    {
        return (lhs.m_vContainer == rhs.m_vContainer) && (lhs.m_ExcludeQueue == rhs.m_ExcludeQueue);
    }

    friend bool operator!=(const random_container_swap& lhs, const random_container_swap& rhs) { return !(lhs == rhs); }

    random_container_swap()
        : base_container()
        , m_ExcludeQueue(m_vContainer, Texclude_size + 1)
    {
    }

    ~random_container_swap() = default;

    random_container_swap(const random_container_swap& src)
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>(src)
        , m_ExcludeQueue(m_vContainer, src.m_ExcludeQueue)
    {
    }

    template <typename InputIterator>
    random_container_swap(InputIterator first, InputIterator last)
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>(first, last)
        , m_ExcludeQueue(m_vContainer, Texclude_size + 1)
    {
    }

    random_container_swap(std::initializer_list<value_type> il)
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>(il)
        , m_ExcludeQueue(m_vContainer, Texclude_size + 1)
    {
    }

    value_type GetNext() override
    {
        assert(m_ExcludeQueue.Size() < Size() || m_ExcludeQueue.Full());

        if (m_ExcludeQueue.Full())
        {
            // The head element has exceeded the exclude count, consider it for the next selection
            value_type headElement = m_ExcludeQueue.Front();
            m_ExcludeQueue.Pop();
            size_t nextIndex = m_gen(m_vContainer.size() - m_ExcludeQueue.Size()) + m_ExcludeQueue.Size();

            if (nextIndex == ExcludeSize())
            {
                // We selected the previous head element, insert it back on the queue
                m_ExcludeQueue.Push(headElement);
                return headElement;
            }

            // Add selected element to the queue
            value_type selectedElement = m_vContainer[nextIndex];
            m_ExcludeQueue.Push(selectedElement);

            // Replace the selected element with the previous head, so it may be considered on the next call
            m_vContainer[nextIndex] = headElement;

            return selectedElement;
        }

        // Chose random index of next element, get a copy to return
        size_t nextIndex = m_gen(m_vContainer.size() - m_ExcludeQueue.Size()) + m_ExcludeQueue.Size();
        value_type nextElement = m_vContainer[nextIndex];

        // Swap the random element with the tail of the queue
        m_ExcludeQueue.Swap(m_vContainer[nextIndex]);

        return nextElement;
    }

private:
    class Queue
    {
    public:
        friend bool operator==(const Queue& lhs, const Queue& rhs)
        {
            return (lhs.m_nMaxSize == rhs.m_nMaxSize) && (lhs.m_nCurrentSize == rhs.m_nCurrentSize) && (lhs.m_nHead == rhs.m_nHead) &&
                   (lhs.m_nTail == rhs.m_nTail);
        }

        explicit Queue(container_type& vContainer, size_t nMaxSize)
            : m_vQueue(vContainer)
            , m_nMaxSize(nMaxSize)
            , m_nCurrentSize(0)
            , m_nHead(0)
            , m_nTail(0)
        {
        }

        explicit Queue(container_type& vContainer, const Queue& src)
            : m_vQueue(vContainer)
            , m_nMaxSize(src.m_nMaxSize)
            , m_nCurrentSize(src.m_nCurrentSize)
            , m_nHead(src.m_nHead)
            , m_nTail(src.m_nTail)
        {
        }

        Queue& operator=(const Queue& rhs)
        {
            m_nMaxSize = rhs.m_nMaxSize;
            m_nCurrentSize = rhs.m_nCurrentSize;
            m_nHead = rhs.m_nHead;
            m_nTail = rhs.m_nTail;
            return *this;
        }

        const value_type& Front() const
        {
            assert(!Empty());
            return m_vQueue[m_nHead];
        }

        void Pop()
        {
            assert(!Empty());
            m_nHead = Next(m_nHead);
            --m_nCurrentSize;
        }

        void Push(const value_type& element)
        {
            assert(!Full());
            m_vQueue[m_nTail] = element;
            m_nTail = Next(m_nTail);
            ++m_nCurrentSize;
        }

        // Add an element by swapping it with the tail of the queue
        void Swap(value_type& element)
        {
            std::swap(element, m_vQueue[m_nTail]);
            m_nTail = Next(m_nTail);
            ++m_nCurrentSize;
        }

        bool Empty() const { return m_nCurrentSize == 0; }

        bool Full() const { return m_nCurrentSize == m_nMaxSize; }

        size_t Size() const { return m_nCurrentSize; }

    private:
        size_t Next(size_t index) const { return (index + 1) % m_nMaxSize; }

        container_type& m_vQueue;
        size_t m_nMaxSize;
        size_t m_nCurrentSize;
        size_t m_nHead;
        size_t m_nTail;
    };

    Queue m_ExcludeQueue;
};

template <typename T, size_t Texclude_size, typename Tgenerator, typename Tcontainer>
class random_container_agecount : public base_container<T, Texclude_size, Tgenerator, Tcontainer>
{
public:
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::m_gen;
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::m_vContainer;
    using base_container<T, Texclude_size, Tgenerator, Tcontainer>::ExcludeSize;
    using value_type = T;

    friend bool operator==(const random_container_agecount& lhs, const random_container_agecount& rhs)
    {
        return (lhs.m_vContainer == rhs.m_vContainer) && (lhs.m_vAge == rhs.m_vAge);
    }

    friend bool operator!=(const random_container_agecount& lhs, const random_container_agecount& rhs) { return !(lhs == rhs); }

    template <typename InputIterator>
    random_container_agecount(InputIterator first, InputIterator last)
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>(first, last)
        , m_vAgeList(Texclude_size)
    {
        m_vAge.assign(m_vContainer.size(), 0);
    }

    random_container_agecount(std::initializer_list<value_type> il)
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>(il)
        , m_vAgeList(Texclude_size)
    {
        m_vAge.assign(m_vContainer.size(), 0);
    }

    random_container_agecount()
        : base_container<T, Texclude_size, Tgenerator, Tcontainer>()
        , m_vAgeList(Texclude_size)
    {
    }

    template <typename Val>
    void push_back(Val&& val)
    {
        m_vContainer.push_back(std::forward<Val>(val));
        m_vAge.push_back(0);
    }

    value_type GetNext() override
    {
        // Since elements cannot be removed, the position of each element will not change
        // Assume the age vector element will always correspond to the contained element
        assert(m_vContainer.size() == m_vAge.size());

        // Chose random index of next element
        size_t nextIndex = GetNextIndex();

        // Create a copy to return
        value_type nextElement = m_vContainer[nextIndex];

        AgeElements(nextIndex);

        return nextElement;
    }

private:
    // Get random index of next element
    size_t GetNextIndex()
    {
        size_t nextIndex = m_gen(m_vContainer.size());
        for (; m_vAge[nextIndex] != 0; nextIndex = Next(nextIndex))
        {
        }
        return nextIndex;
    }

    void AgeElements(size_t nextIndex)
    {
        // The selected element is now age 1, increment the age of all other selected elements
        m_vAge[nextIndex] = 1;

        for (auto& i : m_vAgeList)
        {
            if (i.m_uAge == ExcludeSize())
            {
                i.m_uAge = 0;
                m_vAge[i.m_uIndex] = 0;
            }
            else
            {
                ++i.m_uAge;
                ++m_vAge[i.m_uIndex];
            }
        }

        // loop from [nextIndex + 1, nextIndex), wrap for size
        for (auto i = Next(nextIndex); i != nextIndex; i = Next(i))
        {
            if (m_vAge[i] == ExcludeSize())
            {
                // element has aged-out, reset to zero
                m_vAge[i] = 0;
            }
            else if (m_vAge[i] > 0)
            {
                ++m_vAge[i];
            }
        }
    }

    size_t Next(size_t index) const { return (index + 1) % m_vAge.size(); }

    struct AgeEntry
    {
        AgeEntry() : m_uIndex(0), m_uAge(0) {}

        size_t m_uIndex;
        size_t m_uAge;
    };

    eastl::vector<AgeEntry> m_vAgeList;
    eastl::vector<size_t> m_vAge;
};

template <typename T, size_t Texclude_size = 3, typename Tgenerator = RandomDevice, typename Tcontainer = eastl::vector<T> >
using RandomContainer = random_container_insert<T, Texclude_size, Tgenerator, Tcontainer>;

template <typename T, size_t Tn, size_t Texclude_size = 3, typename Tgenerator = RandomDevice>
using FixedRandomContainer = random_container_insert<T, Texclude_size, Tgenerator, eastl::fixed_vector<T, Tn, false> >;

namespace RandomContainerTest {

template <typename Trandom_container>
void VerifyContainerTest(Trandom_container& v, size_t nCount = 10000)
{
    using ValueType = typename Trandom_container::value_type;

    std::map<ValueType, size_t> countMap;
    ValueType s;
    std::vector<ValueType> q;

    for (size_t i = 0; i < nCount; ++i)
    {
        s = v.GetNext();
        ++countMap[s];

        // make sure this element hasn't been selected in last ExcludeSize tries
        if (std::find(q.begin(), q.end(), s) != q.end())
        {
            assert(!"repeat found");
        }

        if (q.size() == v.ExcludeSize())
        {
            q.erase(q.begin());
        }
        q.push_back(s);
    }

    // Check the distribution for approximate uniformity
    size_t nPerItem = nCount / v.Size();
    for (const auto& i : countMap)
    {
        // Check for within 10% (seems like a reasonable number)
        if ((i.second < nPerItem * 0.9) || (i.second > nPerItem * 1.1))
        {
            assert(!"non-uniform result found");
        }
    }

    // dummy statement so there's something to break on
    printf("Verify complete");
}

template <typename Trandom_container>
void DoVerifyTest(Trandom_container& v)
{
    VerifyContainerTest(v);

    auto vCopy = v;
    assert(vCopy == v);

    VerifyContainerTest(v);
    assert(vCopy != v);
    VerifyContainerTest(vCopy);
    assert(vCopy != v);

    Trandom_container vCopy2;
    vCopy2 = vCopy;
    assert(vCopy2 == vCopy);
    VerifyContainerTest(vCopy2);
    assert(vCopy2 != vCopy);
}

template <typename Trandom_container>
void DoStringTest()
{
    using ValueType = typename Trandom_container::value_type;

    std::vector<ValueType> vs = { "one", "two", "three" };
    Trandom_container mv(vs.begin(), vs.end());

    mv.push_back("testing");
    auto make_element = []() { return ValueType{ "Howdy" }; };
    mv.push_back(make_element());

    ValueType ss{ "Howdy" };
    mv.push_back(ss);

    Trandom_container v = { "hi", "everyone", "hello", "there", "world", "how", "are", "you", "doing", "today" };
    DoVerifyTest(v);
}

template <typename Trandom_container>
void DoWideStringTest()
{
    using ValueType = typename Trandom_container::value_type;

    std::vector<ValueType> vs = { L"one", L"two", L"three" };
    Trandom_container mv(vs.begin(), vs.end());

    mv.push_back(L"testing");
    auto make_element = []() { return ValueType{ L"Howdy" }; };
    mv.push_back(make_element());

    ValueType ss{ L"Howdy" };
    mv.push_back(ss);

    Trandom_container v = { L"hi", L"everyone", L"hello", L"there", L"world", L"how", L"are", L"you", L"doing", L"today" };
    DoVerifyTest(v);
}

template <typename Trandom_container>
void DoPODTest()
{
    using ValueType = typename Trandom_container::value_type;

    eastl::vector<ValueType> EAv;
    EAv.push_back(1);
    EAv.push_back(2);
    EAv.push_back(3);

    // construct from eastl iterators
    Trandom_container EAmv(EAv.begin(), EAv.end());

    std::vector<ValueType> vs = { 1, 2, 3, 4 };
    // construct from std iterators
    Trandom_container mv(vs.begin(), vs.end());

    ValueType n = mv.GetNext();
    n = mv.GetNext();
    n = mv.GetNext();
    n = mv.GetNext();
    n = mv.GetNext();  // must get the newly avail element
    n = mv.GetNext();

    // push_back from rvalue reference
    auto make_element = []() { return ValueType{ 20 }; };
    mv.push_back(make_element());

    n = mv.GetNext();

    // push_back from lvalue reference
    ValueType ss{ 30 };
    mv.push_back(ss);

    n = mv.GetNext();

    // construct from initializer list
    Trandom_container v = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    DoVerifyTest(v);
}

template <template <class, size_t, class, class> typename Trandom_container>
void DoStructTest()
{
    struct SongEntry
    {
        size_t m_uOffset;          // offset to start of song track
        eastl::wstring m_wsUPI;    // timeline name of UPI animation
        eastl::wstring m_wsVideo;  // timeline name of TopScene video
    };

    using SongEntryContainer = Trandom_container<SongEntry, 3, RandomDevice, eastl::vector<SongEntry> >;

    // clang-format off
    auto make_entry = []()
    {
        SongEntry s = { 100, L"LambdaThere", L"LambdaWorld" };
        return s;
    };

    // default construct
    SongEntryContainer v;

    // copy assignment
    v =
    {
        { 100, L"UPI1", L"Video1" },
        { 200, L"UPI2", L"Video2" },
        { 300, L"UPI3", L"Video3" }
    };
    // clang-format on

    // push_back from lvalue reference
    SongEntry s = { 100, L"There", L"World" };
    v.push_back(s);

    // push_back from rvalue reference
    v.push_back(make_entry());

    // push_back from rvalue reference
    v.template push_back<SongEntry>({ 101, L"There", L"World" });
    v.template push_back<SongEntry>({ 102, L"There", L"World" });
    v.template push_back<SongEntry>({ 103, L"There", L"World" });
    v.template push_back<SongEntry>({ 104, L"There", L"World" });
    v.template push_back<SongEntry>({ 105, L"There", L"World" });
    v.template push_back<SongEntry>({ 111, L"Fail", L"Fail" });
}

template <template <class, size_t, class, class> typename Trandom_container>
void RandomContainerTest()
{
    DoPODTest<Trandom_container<int, 3, DefaultRandomEngine, std::vector<int> > >();
    DoPODTest<Trandom_container<unsigned int, 3, RandomDevice, eastl::vector<unsigned int> > >();
    DoPODTest<Trandom_container<double, 3, Rand, eastl::vector<double> > >();

    //DoStringTest<Trandom_container<std::string, 3, RandomDevice, std::vector<std::string> > >();
    //DoStringTest<Trandom_container<eastl::string, 3, RandomDevice, std::vector<eastl::string> > >();

    //DoStringTest<Trandom_container<std::string, 3, DefaultRandomEngine, std::vector<std::string> > >();
    //DoStringTest<Trandom_container<eastl::string, 3, DefaultRandomEngine, std::vector<eastl::string> > >();

    //DoWideStringTest<Trandom_container<std::wstring, 3, Rand, std::vector<std::wstring> > >();
    //DoWideStringTest<Trandom_container<eastl::wstring, 3, Rand, std::vector<eastl::wstring> > >();
}

inline void RandomContainerTests()
{
    RandomContainerTest<random_container_swap>();
    RandomContainerTest<random_container_insert>();
    RandomContainerTest<random_container_agecount>();

    DoStructTest<random_container_swap>();
    DoStructTest<random_container_insert>();
    DoStructTest<random_container_agecount>();
}

}  // namespace RandomContainerTest
