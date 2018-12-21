#if defined(_WIN32)
#pragma warning(disable : 4189)  // local variable is initialized but not referenced
#pragma warning(disable : 4101)  // unreferenced local variable
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4244)  // conversion possible loss of data
#endif

#include <eastl/vector.h>
#include <eastl/list.h>
#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include "CircularQueue.h"
#include "eastl/string.h"
#include <list>

namespace CircularQueueTest {

template <typename T>
struct is_numeric : std::integral_constant<bool, std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>
{
};

template <typename T, typename = typename std::enable_if<is_numeric<T>::value>::type>
static void mutate(T& t)
{
    ++t;
}

template <typename T>
static CircularQueue<T> make_queue(size_t n = 3)
{
    CircularQueue<T> q(n);
    T element = T{};

    while (!q.full())
    {
        q.push(element);
        mutate(element);
    }

    return q;
}

template <typename T>
static void process_queue(CircularQueue<T> q)
{
    q.pop();
    q.push(108);
}

template <typename T, typename = typename std::enable_if<is_numeric<T>::value>::type>
static void DoArithmeticTests()
{
    CircularQueue<T> test(5);
    assert(test.max_size() > 0);
    assert(test.begin() == test.end());
    assert(test.empty());
    assert(test.size() == 0);
    test.push(1);
    assert(test.begin() != test.end());
    assert(!test.empty());
    assert(test.size() == 1);
    test.push(2);
    assert(test.size() == 2);
    test.push(3);
    assert(!test.full());
    assert(test.size() == 3);
    test.push(4);
    assert(test.size() == 4);
    test.push(5);
    assert(test.full());
    assert(test.size() == 5);
    test.pop();
    assert(test.size() == 4);
    assert(!test.full());
    assert(test.size() == 4);
    assert(test.back() == 5);
    assert(test.front() == 2);

    // Copy construct
    auto testCopy(test);
    assert(testCopy == test);
    assert(test == testCopy);

    // Modify the copy, ensure it doesn't affect the original
    testCopy.pop();
    assert(testCopy != test);
    testCopy.push(42);
    assert(testCopy.size() == test.size());
    assert(testCopy != test);

    // Copy assign
    test = testCopy;
    assert(testCopy == test);
    assert(test == testCopy);

    // Move assign
    CircularQueue<T> testMove(3);
    testMove = std::move(test);
    assert(testMove == testCopy);

    // Move construct
    process_queue(make_queue<T>());

    CircularQueue<T> test2(7);
    test2.push(10);
    test2.push(20);
    test2.push(30);
    test2.push(40);
    test2.push(50);
    test2.push(60);
    test2.push(70);

    // Iterator decrement tests
    {
        auto i = test2.end();
        --i; assert(*i == 70);
        i--; assert(*i == 60);
        --i; assert(*i == 50);
        i--; assert(*i == 40);
        --i; assert(*i == 30);
        i--; assert(*i-- == 20);
        assert(*i == 10);
        assert(i == test2.begin());
    }

    const CircularQueue<T>& constRef = test2;
    assert(constRef.back() == 70);
    assert(constRef.front() == 10);

    assert(test2.cbegin() != test2.cend());
    assert(constRef.cbegin() != constRef.cend());
    assert(test2.cbegin() == constRef.cbegin());
    assert(test2.cend() == constRef.cend());

    assert(test2.begin() == begin(test2));
    assert(constRef.begin() == begin(constRef));
    assert(test2.end() == end(test2));
    assert(constRef.end() == end(constRef));

    for (auto i : constRef)
    {
        std::cout << "i is " << i << "\n";
    }

    for (auto& i : constRef)
    {
        std::cout << "i is " << i << "\n";
    }

    for (const auto& i : constRef)
    {
        std::cout << "i is " << i << "\n";
    }

    for (auto iByValue : test2)
    {
        std::cout << "i is " << iByValue << "\n";
    }

    for (auto& iByRef : test2)
    {
        std::cout << "i is " << iByRef << "\n";
    }

    for (const auto& iByConstRef : test2)
    {
        std::cout << "i is " << iByConstRef << "\n";
    }

    test2.pop();  // removes 10
    test2.pop();  // removes 20
    test2.push(80);
    test2.push(90);

    for (auto i : test2)
    {
        std::cout << "i is " << i << "\n";
    }

    // copy construct from empty queue
    {
        CircularQueue<T> qSource(5);
        assert(qSource.empty());
        auto qCopy(qSource);
        assert(qCopy.empty());
        assert(qSource == qCopy);
    }

    // copy assign from empty queue
    {
        CircularQueue<T> qSource(5);
        CircularQueue<T> qCopy;
        qCopy = qSource;
        assert(qSource == qCopy);
    }
}

struct TrivialType
{
    TrivialType() {}
    int i = 0;
    float f = 0.0f;
    char c = 'a';
};

bool operator==(const TrivialType& lhs, const TrivialType& rhs)
{
    return lhs.i == rhs.i && lhs.f == rhs.f && lhs.c == rhs.c;
}

bool operator!=(const TrivialType& lhs, const TrivialType& rhs)
{
    return !(lhs == rhs);
}

static void mutate(TrivialType& t) { t.i += 10; }

struct DerivedTrivialType : TrivialType
{
    double d = 3.14;
    char ch = 'f';
    long long ll = 12345678900;
};

static void mutate(DerivedTrivialType& t) { t.d += 10.0; }

static void DoTrivialObjectTests()
{
    {
        CircularQueue<TrivialType> qTrivial(3);
        assert(qTrivial.empty());

        TrivialType trivialElement;
        trivialElement.i = 42;
        trivialElement.f = 3.14f;
        trivialElement.c = 'a';
        qTrivial.push(trivialElement);
        assert(!qTrivial.empty());
        trivialElement.i = 43;
        trivialElement.f = 3.14f;
        trivialElement.c = 'b';
        qTrivial.push(trivialElement);
        trivialElement.i = 44;
        trivialElement.f = 3.14f;
        trivialElement.c = 'c';
        qTrivial.push(trivialElement);
        assert(qTrivial.full());

        // Copy construct
        auto testCopy(qTrivial);
        assert(testCopy == qTrivial);
        assert(qTrivial == testCopy);

        // Modify the copy, ensure it doesn't affect the original
        testCopy.pop();
        assert(testCopy != qTrivial);
        testCopy.push(trivialElement);
        assert(testCopy.size() == qTrivial.size());
        assert(testCopy != qTrivial);

        // Copy assign
        CircularQueue<TrivialType> qTest(5);
        qTest = testCopy;
        assert(testCopy == qTest);
        assert(qTest == testCopy);
    }

    {
        CircularQueue<DerivedTrivialType> qTrivial(3);
        DerivedTrivialType derivedTrivialElement;
        derivedTrivialElement.i = 42;
        derivedTrivialElement.f = 3.14f;
        derivedTrivialElement.c = 'a';
        derivedTrivialElement.d = 2.717;
        derivedTrivialElement.ch = 'c';
        derivedTrivialElement.ll = 4815162342;
        qTrivial.push(derivedTrivialElement);
    }
}

struct NonTrivialType
{
    NonTrivialType()
        : m_c('a')
        , m_i(0)
    {
    }
    ~NonTrivialType()
    {
        m_c = 0;
        m_i = -1;
    }

    NonTrivialType(char c, int i)
        : m_c(c)
        , m_i(i)
    {
    }

    char m_c;
    int m_i;
};

bool operator==(const NonTrivialType& lhs, const NonTrivialType& rhs)
{
    return lhs.m_c == rhs.m_c && lhs.m_i == rhs.m_i;
}

bool operator!=(const NonTrivialType& lhs, const NonTrivialType& rhs)
{
    return !(lhs == rhs);
}

static void mutate(NonTrivialType& t) { ++t.m_c; t.m_i += 10; }

static void DoNonTrivialObjectTests()
{
    NonTrivialType element1('c', 42);
    NonTrivialType element2('d', 43);
    NonTrivialType element3('e', 44);

    CircularQueue<NonTrivialType> qNonTrivial(3);
    qNonTrivial.push(element1);
    qNonTrivial.push(element2);
    qNonTrivial.push(element3);

    // Copy construct
    auto testCopy(qNonTrivial);
    assert(testCopy == qNonTrivial);
    assert(qNonTrivial == testCopy);

    qNonTrivial.pop();

    auto iDummy = qNonTrivial.begin();
    auto intCopy = iDummy->m_i;
    iDummy->m_c = 'g';
}

template <typename T>
static void DoIteratorTest()
{
    CircularQueue<T> q(5);

    // Mixed equality tests
    assert(q.begin() == q.end());
    assert(q.begin() == q.cend());
    assert(q.cbegin() == q.end());
    assert(q.cbegin() == q.cend());

    T t{};
    q.push(t);
    q.push(t);
    q.push(t);

    // Mixed inequality tests
    assert(q.begin() != q.end());
    assert(q.begin() != q.cend());
    assert(q.cbegin() != q.end());
    assert(q.cbegin() != q.cend());

    // Copy assign, non-const
    typename CircularQueue<T>::iterator inonconst;
    inonconst = q.begin();

    // Copy assign, const
    typename CircularQueue<T>::const_iterator iconst;
    iconst = q.cbegin();

    // Copy assign, non-const to const
    iconst = inonconst;

    // Copy construct, non-const
    typename CircularQueue<T>::iterator inonconstcopy = inonconst;

    // Copy construct, const
    typename CircularQueue<T>::const_iterator iconstcopy = iconst;

    // Copy construct, const from non-const
    typename CircularQueue<T>::const_iterator iconstcopy2 = inonconst;

    // increment, decrement, dereference non-const iterator
    {
        auto i = q.begin();
        ++i;
        i++;
        ++i;
        assert(i == q.end());
        --i;
        i--;
        --i;
        assert(i == q.begin());
        t = *i;
        *i = t;
    }

    // increment, decrement, dereference const iterator
    {
        auto ci = q.cbegin();
        ++ci;
        ci++;
        ++ci;
        assert(ci == q.cend());
        --ci;
        ci--;
        --ci;
        assert(ci == q.cbegin());
        t = *ci;
    }
}

static void DoIteratorArrowTest()
{
    CircularQueue<TrivialType> q(5);
    TrivialType t;
    q.push(t);
    auto i = q.begin();
    *i = t;
    i->c = 'f';
}

template <template <typename T, typename Alloc = std::allocator<T> > typename SequenceContainer>
static void DoSTDContainerIteratorTest()
{
    DoIteratorTest<SequenceContainer<int> >();
    DoIteratorTest<SequenceContainer<TrivialType> >();
    DoIteratorTest<SequenceContainer<DerivedTrivialType> >();
    DoIteratorTest<SequenceContainer<NonTrivialType> >();
}

template <template <typename T, typename Allocator = EASTLAllocatorType> typename SequenceContainer>
static void DoEASTLContainerIteratorTest()
{
    DoIteratorTest<SequenceContainer<int> >();
    DoIteratorTest<SequenceContainer<TrivialType> >();
    DoIteratorTest<SequenceContainer<DerivedTrivialType> >();
    DoIteratorTest<SequenceContainer<NonTrivialType> >();
}

static void DoIteratorTests()
{
    DoIteratorTest<int8_t>();
    DoIteratorTest<uint8_t>();
    DoIteratorTest<int16_t>();
    DoIteratorTest<uint16_t>();
    DoIteratorTest<int32_t>();
    DoIteratorTest<uint32_t>();
    DoIteratorTest<int64_t>();
    DoIteratorTest<uint64_t>();
    DoIteratorTest<float>();
    DoIteratorTest<double>();
    DoIteratorTest<long double>();
    DoIteratorTest<TrivialType>();
    DoIteratorTest<DerivedTrivialType>();
    DoIteratorTest<NonTrivialType>();

    DoIteratorArrowTest();

    DoSTDContainerIteratorTest<std::vector>();
    DoSTDContainerIteratorTest<std::list>();
    DoSTDContainerIteratorTest<std::deque>();

    DoEASTLContainerIteratorTest<eastl::vector>();
    DoEASTLContainerIteratorTest<eastl::list>();
}

static void DoIteratorTraitsTests()
{
    CircularQueue<int> q(5);

    {
        auto iQ = q.begin();
        std::iterator_traits<decltype(iQ)>::value_type iiVT;
        std::iterator_traits<decltype(iQ)>::difference_type iiDT;
        std::iterator_traits<decltype(iQ)>::pointer iiPointer;
        std::iterator_traits<decltype(iQ)>::reference iiReference = iiVT;
        std::iterator_traits<decltype(iQ)>::iterator_category iiCategory;
        assert(typeid(std::iterator_traits<decltype(iQ)>::iterator_category) == typeid(std::bidirectional_iterator_tag));
    }

    {
        const auto& refQ = q;
        auto iQ = refQ.begin();
        std::iterator_traits<decltype(iQ)>::value_type iiVT;
        std::iterator_traits<decltype(iQ)>::difference_type iiDT;
        std::iterator_traits<decltype(iQ)>::pointer iiPointer;
        std::iterator_traits<decltype(iQ)>::reference iiReference = iiVT;
        std::iterator_traits<decltype(iQ)>::iterator_category iiCategory;
        assert(typeid(std::iterator_traits<decltype(iQ)>::iterator_category) == typeid(std::bidirectional_iterator_tag));
    }
}

static void DoAlgorithmTests()
{
    auto q = make_queue<int>(5);
    const auto& constRef = q;

    std::for_each(q.begin(), q.end(), [](int x) { printf("%u", x); });
    std::for_each(constRef.begin(), constRef.end(), [](int x) { printf("%u", x); });

    auto q2 = make_queue<int>(7);
    std::reverse(q2.begin(), q2.end());
}

template <typename String>
String make_string(const char* src)
{
    return String(src);
}

template <>
std::wstring make_string(const char* src)
{
    std::string temp(src);
    return std::wstring(temp.begin(), temp.end());
}

template <typename String>
static void CopyPushAndPopStrings(CircularQueue<String>& testQueue, int num)
{
    assert(testQueue.empty());

    String sTestString = make_string<String>("String data for queue copy test");
    for (int j = 0; j < num; ++j)
    {
        testQueue.push(sTestString);
    }

    for (int j = 0; j < num; ++j)
    {
        testQueue.pop();
    }

    assert(testQueue.empty());
}

template <typename String>
static void MovePushAndPopStrings(CircularQueue<String>& testQueue, int num)
{
    assert(testQueue.empty());

    for (int j = 0; j < num; ++j)
    {
        testQueue.push(make_string<String>("String data for queue move test"));
    }

    for (int j = 0; j < num; ++j)
    {
        testQueue.pop();
    }

    assert(testQueue.empty());
}

template <typename String>
static void DoStringTests()
{
    std::vector<std::string> sJunk;
    sJunk.push_back("hi there this string is too long for SSO");

    CircularQueue<std::string> sQueue(5);
    sQueue.push("hello");
    sQueue.push("there this string is too long for SSO");
    std::string sWorld = "world";
    sQueue.push(sWorld);

    CircularQueue<String> test(5);

    for (int i = 0; i < 1000; ++i)
    {
        CopyPushAndPopStrings(test, 1);
        CopyPushAndPopStrings(test, 2);
        CopyPushAndPopStrings(test, 3);
        CopyPushAndPopStrings(test, 4);
        CopyPushAndPopStrings(test, 5);
    }

    for (int i = 0; i < 1000; ++i)
    {
        MovePushAndPopStrings(test, 1);
        MovePushAndPopStrings(test, 2);
        MovePushAndPopStrings(test, 3);
        MovePushAndPopStrings(test, 4);
        MovePushAndPopStrings(test, 5);
    }
}

template <typename T>
static void DoResizeTests()
{
    // construct and destroy queue with no capacity
    {
        CircularQueue<T> test;
        assert(test.empty());
        assert(test.begin() == test.end());
    }

    // construct queue with no capacity, resize to 0 capacity, destroy
    {
        CircularQueue<T> test;
        test.set_capacity(0);
        assert(test.empty());
        assert(test.begin() == test.end());
    }

    // construct queue with no capacity, resize, destroy
    {
        CircularQueue<T> test;
        test.set_capacity(5);
        assert(test.empty());
        assert(test.begin() == test.end());
    }

    // construct queue with non-zero capacity, resize to same capacity, destroy
    {
        CircularQueue<T> test(10);
        test.set_capacity(10);
        assert(test.empty());
        assert(test.begin() == test.end());
    }

    // construct queue with non-zero capacity, increase capacity, destroy
    {
        CircularQueue<T> test(5);
        test.set_capacity(10);
        assert(test.empty());
        assert(test.begin() == test.end());
    }

    {
        CircularQueue<T> test(5);
        test.push(1);
        test.push(2);
        test.push(3);
        test.set_capacity(10);
    }

    {
        CircularQueue<std::string> test(5);
        test.push("hello");
        test.push("there");
        test.push("world");
        test.set_capacity(10);
    }

    // construct queue with non-zero capacity, decrease capacity, destroy
    {
        CircularQueue<T> test(10);
        test.set_capacity(5);
    }

    // construct queue with non-zero capacity, add elements, increase capacity, destroy
    {
        CircularQueue<T> test(5);
        test.push(10);
        test.push(20);
        test.push(30);
        test.set_capacity(10);
    }
}

static void DoSwapTests()
{
    CircularQueue<std::string> q1(5);
    q1.push("One");
    q1.push("Two");
    q1.push("Three");
    q1.push("Four");
    q1.push("Five");
    auto q1Copy(q1);
    assert(q1 == q1Copy);

    CircularQueue<std::string> q2(7);
    q2.push("A");
    q2.push("B");
    q2.push("C");
    auto q2Copy(q2);
    assert(q2 == q2Copy);

    assert(q1 != q2);
    swap(q1, q2);
    assert(q1 == q2Copy);
    assert(q2 == q1Copy);
}

// Test sequence container of queue
template <typename SequenceContainer>
static void DoGenericSequenceContainerTest()
{
    using Val = typename SequenceContainer::value_type::value_type;

    // Instantiate sequence container of queue of Val elements
    SequenceContainer cQ;
    auto q = make_queue<Val>();
    cQ.push_back(q);
    cQ.push_back(make_queue<Val>());
    cQ.push_back(q);
    auto copy_cQ{ cQ };
    assert(cQ == copy_cQ);
}

// Test queue of sequence container
template <typename Queue>
static void DoGenericQueueContainerTest()
{
    using SequenceContainer = typename Queue::value_type;

    // Instantiate queue of container of Val elements
    Queue qC(5);
    SequenceContainer c(3);
    qC.push(c);
    qC.push(SequenceContainer{ 4 });
    qC.push(c);
    auto copy_qC{ qC };
    assert(qC == copy_qC);
}

template <template <typename T, typename Alloc = std::allocator<T> > typename SequenceContainer>
static void DoSTDSequenceContainerTests()
{
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<int> > >();
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<TrivialType> > >();
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<DerivedTrivialType> > >();
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<NonTrivialType> > >();

    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<int> > >();
    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<TrivialType> > >();
    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<DerivedTrivialType> > >();
    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<NonTrivialType> > >();
}

template <template <typename T, typename Allocator = EASTLAllocatorType> typename SequenceContainer>
static void DoEASTLSequenceContainerTests()
{
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<int> > >();
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<TrivialType> > >();
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<DerivedTrivialType> > >();
    DoGenericSequenceContainerTest<SequenceContainer<CircularQueue<NonTrivialType> > >();

    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<int> > >();
    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<TrivialType> > >();
    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<DerivedTrivialType> > >();
    DoGenericQueueContainerTest<CircularQueue<SequenceContainer<NonTrivialType> > >();
}

template <typename T>
void DoNestedQueueTest()
{
    CircularQueue<CircularQueue<T> > nestedQ(5);
    auto iQ = make_queue<T>();
    nestedQ.push(iQ);
    nestedQ.push(iQ);
    nestedQ.push(iQ);

    // std  std  std
    {
        CircularQueue<std::vector<CircularQueue<std::list<std::string> > > > q(5);
        std::list<std::string> l(5);
        CircularQueue<std::list<std::string> > ql(5);
        ql.push(l);
        std::vector<CircularQueue<std::list<std::string> > > vql;
        vql.push_back(ql);
        q.push(vql);
        auto copy_q{ q };
        assert(q == copy_q);
    }

    // std  std  eastl
    {
        CircularQueue<std::vector<CircularQueue<std::list<eastl::string> > > > q(5);
        std::list<eastl::string> l(5);
        CircularQueue<std::list<eastl::string> > ql(5);
        ql.push(l);
        std::vector<CircularQueue<std::list<eastl::string> > > vql;
        vql.push_back(ql);
        q.push(vql);
        auto copy_q{ q };
        assert(q == copy_q);
    }

    // std eastl std
    {
        CircularQueue<std::vector<CircularQueue<eastl::list<std::string> > > > q(5);
        eastl::list<std::string> l(5);
        CircularQueue<eastl::list<std::string> > ql(5);
        ql.push(l);
        std::vector<CircularQueue<eastl::list<std::string> > > vql;
        vql.push_back(ql);
        q.push(vql);
        auto copy_q{ q };
        assert(q == copy_q);
    }

    // std eastl eastl
    {
        CircularQueue<std::vector<CircularQueue<eastl::list<eastl::string> > > > q(5);
        eastl::list<eastl::string> l(5);
        CircularQueue<eastl::list<eastl::string> > ql(5);
        ql.push(l);
        std::vector<CircularQueue<eastl::list<eastl::string> > > vql;
        vql.push_back(ql);
        q.push(vql);
        auto copy_q{ q };
        assert(q == copy_q);
    }

    // eastl  std  std
    //{
    //    CircularQueue<eastl::vector<CircularQueue<std::list<std::string> > > > q(5);
    //    std::list<std::string> l(5);
    //    CircularQueue<std::list<std::string> > ql(5);
    //    ql.push(l);
    //    eastl::vector<CircularQueue<std::list<std::string> > > vql;
    //    vql.push_back(ql);
    //    q.push(vql);
    //    auto copy_q{ q };
    //    assert(q == copy_q);
    //}

    // eastl  std  eastl
    //{
    //    CircularQueue<eastl::vector<CircularQueue<std::list<eastl::string> > > > q(5);
    //    std::list<eastl::string> l(5);
    //    CircularQueue<std::list<eastl::string> > ql(5);
    //    ql.push(l);
    //    eastl::vector<CircularQueue<std::list<eastl::string> > > vql;
    //    vql.push_back(ql);
    //    q.push(vql);
    //    auto copy_q{ q };
    //    assert(q == copy_q);
    //}

    // eastl eastl std
    //{
    //    CircularQueue<eastl::vector<CircularQueue<eastl::list<std::string> > > > q(5);
    //    eastl::list<std::string> l(5);
    //    CircularQueue<eastl::list<std::string> > ql(5);
    //    ql.push(l);
    //    eastl::vector<CircularQueue<eastl::list<std::string> > > vql;
    //    vql.push_back(ql);
    //    q.push(vql);
    //    auto copy_q{ q };
    //    assert(q == copy_q);
    //}

    // eastl eastl eastl
    {
        CircularQueue<eastl::vector<CircularQueue<eastl::list<eastl::string> > > > q(5);
        eastl::list<eastl::string> l(5);
        CircularQueue<eastl::list<eastl::string> > ql(5);
        ql.push(l);
        eastl::vector<CircularQueue<eastl::list<eastl::string> > > vql;
        vql.push_back(ql);
        q.push(vql);
        auto copy_q{ q };
        assert(q == copy_q);
    }
}

void RunCircularQueueTest()
{
    DoArithmeticTests<int8_t>();
    DoArithmeticTests<uint8_t>();
    DoArithmeticTests<int16_t>();
    DoArithmeticTests<uint16_t>();
    DoArithmeticTests<int32_t>();
    DoArithmeticTests<uint32_t>();
    DoArithmeticTests<int64_t>();
    DoArithmeticTests<uint64_t>();
    DoArithmeticTests<float>();
    DoArithmeticTests<double>();
    DoArithmeticTests<long double>();

    DoTrivialObjectTests();
    DoNonTrivialObjectTests();

    DoAlgorithmTests();
    DoIteratorTests();
    DoIteratorTraitsTests();

    DoStringTests<std::string>();
    DoStringTests<eastl::string>();
    DoStringTests<std::wstring>();
    DoStringTests<eastl::wstring>();

    DoSwapTests();
    DoResizeTests<int>();

    DoSTDSequenceContainerTests<std::vector>();
    DoSTDSequenceContainerTests<std::list>();
    DoSTDSequenceContainerTests<std::deque>();

    DoEASTLSequenceContainerTests<eastl::vector>();
    DoEASTLSequenceContainerTests<eastl::list>();

    DoNestedQueueTest<int>();
}

}  // namespace CircularQueueTest
