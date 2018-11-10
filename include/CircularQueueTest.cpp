#include "CircularQueue.h"
#include <string>
#include "eastl/string.h"

namespace CircularQueueTest {

static void DoPODTests()
{
    CircularQueue<int> test(5);

    assert(test.empty());
    assert(test.size() == 0);
    test.push(1);
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

    const CircularQueue<int>& constRef = test;
    assert(constRef.back() == 5);
    assert(constRef.front() == 2);

    auto testCopy(test);
    assert(testCopy == test);
    assert(test == testCopy);
    testCopy.pop();
    assert(testCopy != test);
    int theValue = 42;
    testCopy.push(theValue);
    assert(testCopy != test);

    CircularQueue<int> test2;
    test2.set_capacity(10);
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
