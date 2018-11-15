#if defined(_WIN32)
#pragma warning(disable : 4189)  // local variable is initialized but not referenced
#pragma warning(disable : 4101)  // unreferenced local variable
#endif

#include <eastl/vector.h>
#include <algorithm>
#include <deque>
#include <string>
#include "CircularQueue.h"
#include "eastl/string.h"

namespace CircularQueueTest {

static void DoPODTests()
{
    CircularQueue<int> test(5);
    assert(test.begin() == test.end());
    assert(test.empty());
    assert(test.size() == 0);
    test.push(1);
    assert(test.begin() != test.end());
    assert(!test.empty());
    assert(test.size() == 1);
    test.push(2);
    test.push(3);
    assert(!test.full());
    assert(test.size() == 3);
    test.push(4);
    test.push(5);
    assert(test.full());
    assert(test.size() == 5);
    test.pop();
    assert(!test.full());
    assert(test.size() == 4);
    assert(test.back() == 5);
    assert(test.front() == 2);

    auto testCopy(test);
    assert(testCopy == test);
    assert(test == testCopy);
    testCopy.pop();
    assert(testCopy != test);
    int theValue = 42;
    testCopy.push(theValue);
    assert(testCopy != test);

    CircularQueue<int> test2(7);
    test2.push(10);
    test2.push(20);
    test2.push(30);
    test2.push(40);
    test2.push(50);
    test2.push(60);
    test2.push(70);

    const CircularQueue<int>& constRef = test2;
    assert(constRef.back() == 70);
    assert(constRef.front() == 10);

    for (auto i : constRef)
    {
        printf("i is %u\n", i);
    }

    for (auto& i : constRef)
    {
        printf("i is %u\n", i);
    }

    for (const auto& i : constRef)
    {
        printf("i is %u\n", i);
    }

    for (auto iByValue : test2)
    {
        printf("i is %u\n", iByValue);
    }

    for (auto& iByRef : test2)
    {
        iByRef = 5;
        printf("i is %u\n", iByRef);
    }

    for (const auto& iByConstRef : test2)
    {
        printf("i is %u\n", iByConstRef);
    }

    test2.pop();  // removes 10
    test2.pop();  // removes 20
    test2.push(80);
    test2.push(90);

    for (auto i : test2)
    {
        printf("i is %u\n", i);
    }

    std::for_each(test2.begin(), test2.end(), [](int x) { printf("%u", x); });
    std::for_each(constRef.begin(), constRef.end(), [](int x) { printf("%u", x); });

    struct Dummy
    {
        char m_c;
        int m_i;
    };

    Dummy element = { 'c', 42 };
    std::deque<Dummy> dequeDummy;
    dequeDummy.push_back(element);
    auto Iter = dequeDummy.begin();
    std::deque<Dummy>::iterator IterEnd;
    std::vector<int>::iterator vIter;
    eastl::vector<int>::iterator iJunk;
    int intCopy = Iter->m_i;

    std::iterator_traits<decltype(Iter)>::value_type ii;

    CircularQueue<Dummy> queueDummy(5);
    queueDummy.push(element);
    auto iDummy = queueDummy.begin();
    intCopy = iDummy->m_i;
    iDummy->m_c = 'g';

    std::iterator_traits<decltype(iDummy)>::value_type iiVT;
    std::iterator_traits<decltype(iDummy)>::difference_type iiDT;
    std::iterator_traits<decltype(iDummy)>::pointer iiPointer;
    std::iterator_traits<decltype(iDummy)>::reference iiReference = iiVT;
    std::iterator_traits<decltype(iDummy)>::iterator_category iiCategory;
    assert(typeid(std::iterator_traits<decltype(iDummy)>::iterator_category) == typeid(std::forward_iterator_tag));
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
        String sTestString = make_string<String>("String data for queue move test");
        testQueue.push(std::move(sTestString));
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

void RunCircularQueueTest()
{
    DoPODTests();
    DoStringTests<std::string>();
    DoStringTests<eastl::string>();
    DoStringTests<std::wstring>();
    DoStringTests<eastl::wstring>();
}

}  // namespace CircularQueueTest
