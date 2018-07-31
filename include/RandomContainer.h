#pragma once

#include <eastl/fixed_vector.h>
#include <map>
#include <deque>
#include "RandomEngine.h"

template <typename T, size_t Texclude_size, typename Tgenerator, typename Tcontainer>
class random_container_insert final
{
public:
    using value_type = T;
    using container_type = Tcontainer;
    using generator_type = Tgenerator;

    friend bool operator==(const random_container_insert& lhs, const random_container_insert& rhs)
    {
        return (lhs.m_vContainer == rhs.m_vContainer) && (lhs.m_vExcludeQueue == rhs.m_vExcludeQueue);
    }

    friend bool operator!=(const random_container_insert& lhs, const random_container_insert& rhs) { return !(lhs == rhs); }

    random_container_insert() = default;

    template <typename InputIterator>
    random_container_insert(InputIterator first, InputIterator last)
        : m_vContainer(first, last)
    {
    }

    random_container_insert(std::initializer_list<value_type> il)
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
    size_t Size() const { return m_vContainer.size() + m_vExcludeQueue.size(); }

    // Returns the next element in the container
    value_type GetNext()
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
    container_type m_vContainer;
    generator_type m_gen;
    eastl::fixed_vector<value_type, Texclude_size + 1> m_vExcludeQueue;
};

template <typename T, size_t Texclude_size, typename Tgenerator, typename Tcontainer>
class random_container_swap final
{
public:
    using value_type = T;
    using container_type = Tcontainer;
    using generator_type = Tgenerator;

    friend bool operator==(const random_container_swap& lhs, const random_container_swap& rhs)
    {
        return (lhs.m_vContainer == rhs.m_vContainer) &&
               (lhs.m_QCurrentSize == rhs.m_QCurrentSize) &&
               (lhs.m_QHead == rhs.m_QHead) &&
               (lhs.m_QTail == rhs.m_QTail);
    }

    friend bool operator!=(const random_container_swap& lhs, const random_container_swap& rhs) { return !(lhs == rhs); }

    random_container_swap() = default;

    template <typename InputIterator>
    random_container_swap(InputIterator first, InputIterator last)
        : m_vContainer(first, last)
    {
    }

    random_container_swap(std::initializer_list<value_type> il)
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
    size_t Size() const { return m_vContainer.size(); }

    // Returns the next element in the container
    value_type GetNext()
    {
        assert(qSize() < Size() || qFull());

        if (qFull())
        {
            // The head element has exceeded the exclude count, consider it for the next selection
            value_type headElement = qFront();
            qPop();
            size_t nextIndex = m_gen(m_vContainer.size() - qSize()) + qSize();

            if (nextIndex == ExcludeSize())
            {
                // We selected the previous head element, push it back on the exclude queue
                qPush(headElement);
                return headElement;
            }

            // Add selected element to the queue
            value_type selectedElement = m_vContainer[nextIndex];
            qPush(selectedElement);

            // Replace the selected element with the previous head, so it may be considered on the next call
            m_vContainer[nextIndex] = headElement;

            return selectedElement;
        }

        // Chose random index of next element, get a copy to return
        size_t nextIndex = m_gen(m_vContainer.size() - qSize()) + qSize();
        value_type nextElement = m_vContainer[nextIndex];

        // Swap the random element with the tail of the queue
        qSwap(m_vContainer[nextIndex]);

        return nextElement;
    }

private:

    // Return a reference to the element at the head of the queue
    const value_type& qFront() const
    {
        assert(!qEmpty());
        return m_vContainer[m_QHead];
    }

    // Remove an element from the head of the queue
    void qPop()
    {
        assert(!qEmpty());
        m_QHead = qNext(m_QHead);
        --m_QCurrentSize;
    }

    // Add an element to the tail of the queue
    void qPush(const value_type& element)
    {
        assert(!qFull());
        m_vContainer[m_QTail] = element;
        m_QTail = qNext(m_QTail);
        ++m_QCurrentSize;
    }

    // Add an element by swapping it with the tail of the queue
    void qSwap(value_type& element)
    {
        std::swap(element, m_vContainer[m_QTail]);
        m_QTail = qNext(m_QTail);
        ++m_QCurrentSize;
    }

    // Returns whether the queue is empty
    bool qEmpty() const { return m_QCurrentSize == 0; }

    // Returns whether the queue is full
    bool qFull() const { return m_QCurrentSize == ExcludeSize() + 1; }

    // Returns the number of elements in the queue
    size_t qSize() const { return m_QCurrentSize; }

    // Returns the next queue index
    size_t qNext(size_t index) const { return (index + 1) % (ExcludeSize() + 1); }

    container_type m_vContainer;
    generator_type m_gen;
    size_t m_QCurrentSize = 0;
    size_t m_QHead = 0;
    size_t m_QTail = 0;
};

template <typename T, size_t Texclude_size, typename Tgenerator, typename Tcontainer>
class random_container_agecount final
{
public:
    using value_type = T;
    using container_type = Tcontainer;
    using generator_type = Tgenerator;

    friend bool operator==(const random_container_agecount& lhs, const random_container_agecount& rhs)
    {
        return (lhs.m_vContainer == rhs.m_vContainer) && (lhs.m_vInUse == rhs.m_vInUse) && (lhs.m_vExcludeQueue == rhs.m_vExcludeQueue);
    }

    friend bool operator!=(const random_container_agecount& lhs, const random_container_agecount& rhs) { return !(lhs == rhs); }

    random_container_agecount() = default;

    template <typename InputIterator>
    random_container_agecount(InputIterator first, InputIterator last)
        : m_vContainer(first, last)
    {
        m_vInUse.assign(m_vContainer.size(), 0);
    }

    random_container_agecount(std::initializer_list<value_type> il)
        : m_vContainer(il.begin(), il.end())
    {
        m_vInUse.assign(m_vContainer.size(), 0);
    }

    template <typename Val>
    void push_back(Val&& val)
    {
        m_vContainer.push_back(std::forward<Val>(val));
        m_vInUse.push_back(0);
    }

    // Returns the exclude size
    constexpr size_t ExcludeSize() const { return Texclude_size; }

    // Returns the number of elements in the container
    size_t Size() const { return m_vContainer.size(); }

    // Returns the next element in the container
    value_type GetNext()
    {
        // Since elements cannot be removed, the position of each element will not change
        // Assume the age vector element will always correspond to the contained element
        assert(m_vContainer.size() == m_vInUse.size());
		assert(m_vContainer.size() - m_vExcludeQueue.size() > 0);

        // Chose random index of next element
        size_t nextIndex = GetNextIndex();

        // Create a copy to return
        value_type nextElement = m_vContainer[nextIndex];

        UpdateExcludeList(nextIndex);

        return nextElement;
    }

private:
    container_type m_vContainer;
    generator_type m_gen;

	std::deque<size_t> m_vExcludeQueue;
	std::vector<uint8_t> m_vInUse;

    // Get random index of next element
    size_t GetNextIndex() const
    {
		// Choose a random index, then probe the in-use vector, if its already used,
		// pick the next available element
        size_t nextIndex = m_gen(m_vContainer.size());
        for (; m_vInUse[nextIndex] != 0; nextIndex = Next(nextIndex))
        {
        }
        return nextIndex;
    }

	void UpdateExcludeList(size_t index)
	{
		// Mark the newly-chosen element as ineligible
		m_vInUse[index] = 1;
		
		// If the queue is full, make the oldest entry eligible
		if (Full())
		{
			size_t eligibleIndex = m_vExcludeQueue.front();
			m_vExcludeQueue.pop_front();
			m_vInUse[eligibleIndex] = 0;
		}

        // The new index is now ineligible
		m_vExcludeQueue.push_back(index);
	}

	bool Full() const { return m_vExcludeQueue.size() == ExcludeSize(); }

    size_t Next(size_t index) const { return (index + 1) % m_vInUse.size(); }
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
