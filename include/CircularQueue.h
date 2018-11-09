#pragma once

#include <cassert>
#include <vector>

// Fixed size circular queue
template <typename T>
class CircularQueue
{
public:
    using value_type = T;
    using container_type = std::vector<value_type>;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;

    CircularQueue() = default;
    ~CircularQueue() = default;
    CircularQueue(const CircularQueue&) = default;
    CircularQueue(CircularQueue&&) = default;
    CircularQueue& operator=(const CircularQueue&) = default;
    CircularQueue& operator=(CircularQueue&&) = default;

    // Construct a circular queue with the given element capacity
    explicit CircularQueue(size_type capacity)
        : m_vContainer(capacity)
        , m_Tail(capacity - 1)
    {
        assert(m_vContainer.size() == capacity);
    }

    // Returns a reference to the last element in the queue
    reference back() { return const_cast<reference>(tail()); }

    // Returns a const reference to the last element in the queue
    const_reference back() const { return tail(); }

    // Returns whether the queue is empty
    bool empty() const noexcept { return m_Size == 0; }

    // Returns a reference to the first element in the queue
    reference front() { return const_cast<reference>(head()); }

    // Returns a const reference to the first element in the queue
    const_reference front() const { return head(); }

    // Returns whether the size of the queue is at maximum capacity
    bool full() const noexcept { return m_Size == m_vContainer.size(); }

    // Removes the first element, reduces queue size by one.
    void pop() noexcept
    {
        assert(!empty());
        increment(m_Head);
        --m_Size;
    }

    // Inserts a new element at the end of the queue
    void push(const value_type& val) { add_element(val); }

    // Inserts a new element at the end of the queue
    void push(value_type&& val) { add_element(std::move(val)); }

    // Change the capacity of the queue
    void set_capacity(size_type capacity)
    {
        m_vContainer.reserve(capacity);
        m_vContainer.assign(capacity, value_type{});
    }

    void swap(CircularQueue& other) noexcept
    {
        std::swap(m_vContainer, other.m_vContainer);
        std::swap(m_Head, other.m_Head);
        std::swap(m_Tail, other.m_Tail);
        std::swap(m_Size, other.m_Size);
    }

    friend bool operator==(const CircularQueue& lhs, const CircularQueue& rhs) { return lhs.compare(rhs); }

    friend bool operator!=(const CircularQueue& lhs, const CircularQueue& rhs) { return !(lhs == rhs); }

private:
    template <typename Val>
    void add_element(Val&& val)
    {
        assert(!full());
        increment(m_Tail);
        assert(m_Tail < m_vContainer.size());
        m_vContainer[m_Tail] = std::forward<Val>(val);
        ++m_Size;
    }

    void increment(size_type& val) const noexcept
    {
        ++val;
        if (val == m_vContainer.size())
        {
            val = 0;
        }
    }

    const_reference head() const
    {
        assert(!empty());
        assert(m_Head < m_vContainer.size());
        return m_vContainer[m_Head];
    }

    const_reference tail() const
    {
        assert(!empty());
        assert(m_Tail < m_vContainer.size());
        return m_vContainer[m_Tail];
    }

    bool compare(const CircularQueue& other) const
    {
        if (m_Size != other.m_Size)
        {
            return false;
        }

        // iterate over each container, compare elements
        auto thisIndex = m_Head;
        auto otherIndex = other.m_Head;

        for (; thisIndex != m_Tail; increment(thisIndex), increment(otherIndex))
        {
            if (m_vContainer[thisIndex] != other.m_vContainer[otherIndex])
            {
                return false;
            }
        }

        return true;
    }

    container_type m_vContainer;
    size_type m_Head = 0;
    size_type m_Tail = 0;
    size_type m_Size = 0;
};

// clang-format off
namespace CircularQueueTest
{
extern void RunCircularQueueTest();
}
// clang-format on
