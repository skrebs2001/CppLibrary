#pragma once

#include <cassert>
#include <type_traits>
#include <vector>

template <typename Queue, typename Pointer, typename Reference>
class CircularQueueIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Queue::value_type;
    using difference_type = typename Queue::difference_type;
    using pointer = Pointer;
    using reference = Reference;

    explicit CircularQueueIterator(Queue* pCircularQueue, pointer pElement)
        : m_pCircularQueue(pCircularQueue)
        , m_pElement(pElement)
    {
    }

    CircularQueueIterator() = default;
    CircularQueueIterator(const CircularQueueIterator&) = default;
    CircularQueueIterator& operator=(const CircularQueueIterator&) = default;
    ~CircularQueueIterator() = default;

    // Pre-increment operator
    CircularQueueIterator& operator++()
    {
        m_pElement = m_pCircularQueue->next(m_pElement);
        return *this;
    }

    // Post-increment operator
    CircularQueueIterator operator++(int)
    {
        CircularQueueIterator temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const { return *m_pElement; }

    pointer operator->() const { return m_pElement; }

    friend bool operator==(const CircularQueueIterator& lhs, const CircularQueueIterator& rhs) { return lhs.m_pElement == rhs.m_pElement; }

    friend bool operator!=(const CircularQueueIterator& lhs, const CircularQueueIterator& rhs) { return !(lhs == rhs); }

private:
    Queue* m_pCircularQueue = nullptr;
    pointer m_pElement = nullptr;
};

#if 1
// Fixed size circular queue
template <typename T>
class CircularQueue final
{
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using size_type = size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = CircularQueueIterator<CircularQueue<value_type>, pointer, reference>;
    using const_iterator = CircularQueueIterator<CircularQueue<value_type>, const_pointer, const_reference>;
    template <class, class, class>
    friend class CircularQueueIterator;

    CircularQueue() = default;

    // Construct a circular queue with the given element capacity
    explicit CircularQueue(size_type capacity)
        : m_pData(allocate(capacity + 1))
        , m_Head(0)
        , m_Tail(capacity)
        , m_Size(0)
        , m_Capacity(capacity + 1)
    {
    }

    CircularQueue(const CircularQueue& other)
        : m_pData(allocate(other.m_Capacity))
        , m_Head(0)
        , m_Tail(other.m_Size - 1)
        , m_Size(other.m_Size)
        , m_Capacity(other.m_Capacity)
    {
        construct_range(other.begin(), other.end(), m_pData);
    }

    CircularQueue& operator=(const CircularQueue& other)
    {
        // Potential optimization is to copy the elements from other if the current memory has the capacity
        // Would lose the strong exception guarantee since an element copy may throw, but eliminates one memory allocation and deallocation
        CircularQueue temp(other);
        swap(temp);
        return *this;
    }

    CircularQueue(CircularQueue&& other) noexcept
    {
        // Move resources from other object into new
        move_from(other);
    }

    ~CircularQueue()
    {
        deallocate();
        m_pData = nullptr;
    }

    CircularQueue& operator=(CircularQueue&& other) noexcept
    {
        // Release current resources, then move from other object
        deallocate();
        move_from(other);
        return *this;
    }

    // Returns a reference to the last element in the queue
    reference back() { return *tail(); }
    const_reference back() const { return *tail(); }

    // Returns an iterator pointing to the first element in the queue
    iterator begin() noexcept { return iterator(this, head()); }
    const_iterator begin() const noexcept { return const_iterator(const_cast<CircularQueue*>(this), head()); }

    // Returns whether the queue is empty
    bool empty() const noexcept { return size() == 0; }

    // Returns an iterator pointing to the past-the-end element in the queue
    iterator end() noexcept { return iterator(this, next(tail())); }
    const_iterator end() const noexcept { return const_iterator(const_cast<CircularQueue*>(this), next(tail())); }

    // Returns a reference to the first element in the queue
    reference front() { return *head(); }
    const_reference front() const { return *head(); }

    // Returns whether the queue is at maximum capacity
    bool full() const noexcept { return size() == capacity() - 1; }

    // Removes the head element, reduces queue size by one
    void pop()
    {
        assert(!empty());
        head()->~value_type();
        m_Head = increment(m_Head);
        --m_Size;
    }

    // Inserts a new element at the tail of the queue
    void push(const value_type& val) { add(val); }
    void push(value_type&& val) { add(std::move(val)); }

    // Returns the number of elements in the queue
    size_type size() const noexcept { return m_Size; }

    // Exchanges the contents of the queue with those of other
    void swap(CircularQueue& other) noexcept
    {
        std::swap(m_pData, other.m_pData);
        std::swap(m_Head, other.m_Head);
        std::swap(m_Tail, other.m_Tail);
        std::swap(m_Size, other.m_Size);
        std::swap(m_Capacity, other.m_Capacity);
    }

private:
    template <typename Val>
    struct IsTrivial : std::is_trivial<typename std::remove_reference<Val>::type>
    {
    };

    template <typename Val>
    void add(Val&& val)
    {
        if (full()) pop();
        m_Tail = increment(m_Tail);
        assert(m_Tail < capacity());
        construct(std::forward<Val>(val), tail());
        ++m_Size;
    }

    // Construct elements from the range [first, last) into contiguous memory range pointed to by dest
    void construct_range(const_iterator first, const_iterator last, pointer dest)
    {
        for (; first != last; ++first, ++dest)
        {
            construct(*first, dest);
        }
    }

    // Construct a single element into memory pointed to by dest
    template <typename Val>
    typename std::enable_if<IsTrivial<Val>::value>::type construct(Val&& val, pointer dest)
    {
        *dest = val;
    }

    // Construct a single element into memory pointed to by dest
    template <typename Val>
    typename std::enable_if<!IsTrivial<Val>::value>::type construct(Val&& val, pointer dest)
    {
        ::new (dest) value_type(std::forward<Val>(val));
    }

    pointer allocate(size_t capacity) { return static_cast<pointer>(::operator new(sizeof(value_type) * capacity)); }

    // Destroy all elements and release buffer memory
    void deallocate()
    {
        if (m_pData)
        {
            destroy_range(head(), next(tail()));
            ::operator delete(m_pData);
        }
    }

    // Destroy the buffer elements in the range [first, last)
    template <typename Val = value_type>
    typename std::enable_if<std::is_trivially_destructible<Val>::value>::type destroy_range(pointer, pointer)
    {
    }

    // Destroy the buffer elements in the range [first, last)
    template <typename Val = value_type>
    typename std::enable_if<!std::is_trivially_destructible<Val>::value>::type destroy_range(pointer first, pointer last)
    {
        for (; first != last; first = next(first))
        {
            first->~value_type();
        }
    }

    void move_from(CircularQueue& other) noexcept
    {
        m_pData = other.m_pData;
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;

        other.m_pData = nullptr;
        other.m_Head = 0;
        other.m_Tail = 0;
        other.m_Size = 0;
        other.m_Capacity = 0;
    }

    // Returns pointer to head element
    pointer head() { return &m_pData[m_Head]; }
    const_pointer head() const { return &m_pData[m_Head]; }

    // Returns pointer to tail element
    pointer tail() { return &m_pData[m_Tail]; }
    const_pointer tail() const { return &m_pData[m_Tail]; }

    // Returns pointer to the next logical element
    pointer next(pointer p) { return const_cast<pointer>(next(const_cast<const_pointer>(p))); }
    const_pointer next(const_pointer p) const
    {
        size_type nextIndex = increment(p - m_pData);
        assert(nextIndex < capacity());
        return &m_pData[nextIndex];
    }

    // Increment the given index, wrapping around the container size
    size_type increment(size_type index) const noexcept { return (index == capacity() - 1) ? 0 : index + 1; }

    size_type capacity() const noexcept { return m_Capacity; }

    pointer m_pData = nullptr;
    size_type m_Head = 0;
    size_type m_Tail = 0;
    size_type m_Size = 0;
    size_type m_Capacity = 0;
};

template <typename T>
bool operator==(const CircularQueue<T>& lhs, const CircularQueue<T>& rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const CircularQueue<T>& lhs, const CircularQueue<T>& rhs)
{
    return !(lhs == rhs);
}

#else

// Fixed size circular queue
template <typename T>
class CircularQueue final
{
    using container_type = std::vector<T>;

public:
    using value_type = T;
    using difference_type = typename container_type::difference_type;
    using size_type = typename container_type::size_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    using iterator = CircularQueueIterator<CircularQueue<T>, pointer, reference>;
    using const_iterator = CircularQueueIterator<CircularQueue<T>, const_pointer, const_reference>;
    template <class, class, class>
    friend class CircularQueueIterator;

    CircularQueue() = delete;
    ~CircularQueue() = default;
    CircularQueue(const CircularQueue&) = default;
    CircularQueue(CircularQueue&&) = default;
    CircularQueue& operator=(const CircularQueue&) = default;
    CircularQueue& operator=(CircularQueue&&) = default;

    // Construct a circular queue with the given element capacity
    explicit CircularQueue(size_type capacity)
        : m_vContainer(capacity + 1)
        , m_Tail(capacity)
    {
    }

    // Returns a reference to the last element in the queue
    reference back() { return *tail(); }
    const_reference back() const { return *tail(); }

    // Returns an iterator pointing to the first element in the queue
    iterator begin() noexcept { return iterator(this, head()); }
    const_iterator begin() const noexcept { return const_iterator(const_cast<CircularQueue*>(this), head()); }

    // Returns whether the queue is empty
    bool empty() const noexcept { return size() == 0; }

    // Returns an iterator pointing to the past-the-end element in the queue
    //iterator end() noexcept { return empty() ? begin() : iterator(this, next(tail())); }
    //const_iterator end() const noexcept { return empty() ? begin() : const_iterator(const_cast<CircularQueue*>(this), next(tail())); }
    iterator end() noexcept { return iterator(this, next(tail())); }
    const_iterator end() const noexcept { return const_iterator(const_cast<CircularQueue*>(this), next(tail())); }

    // Returns a reference to the first element in the queue
    reference front() { return *head(); }
    const_reference front() const { return *head(); }

    // Returns whether the queue is at maximum capacity
    bool full() const noexcept { return size() == m_vContainer.size() - 1; }

    // Removes the first element, reduces queue size by one
    void pop() noexcept
    {
        assert(!empty());
        m_Head = increment(m_Head);
        --m_Size;
    }

    // Inserts a new element at the end of the queue
    void push(const value_type& val) { add(val); }
    void push(value_type&& val) { add(std::move(val)); }

    // Returns the number of elements in the queue
    size_type size() const noexcept { return m_Size; }

    // Exchanges the contents of the queue with those of other
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
    void add(Val&& val)
    {
        assert(!full());
        m_Tail = increment(m_Tail);
        assert(m_Tail < m_vContainer.size());
        m_vContainer[m_Tail] = std::forward<Val>(val);
        ++m_Size;
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

        for (; thisIndex != m_Tail; thisIndex = increment(thisIndex), otherIndex = increment(otherIndex))
        {
            if (m_vContainer[thisIndex] != other.m_vContainer[otherIndex])
            {
                return false;
            }
        }

        return true;
    }

    // Returns pointer to head element
    pointer head() { return &m_vContainer[m_Head]; }
    const_pointer head() const { return &m_vContainer[m_Head]; }

    // Returns pointer to tail element
    pointer tail() { return &m_vContainer[m_Tail]; }
    const_pointer tail() const { return &m_vContainer[m_Tail]; }

    // Returns pointer to the next logical element
    pointer next(pointer p)
    {
        //assert(!empty());
        size_type nextIndex = increment(p - m_vContainer.data());
        assert(nextIndex < m_vContainer.size());
        return &m_vContainer[nextIndex];
    }

    // Returns pointer to the next logical element
    const_pointer next(const_pointer p) const
    {
        //assert(!empty());
        size_type nextIndex = increment(p - m_vContainer.data());
        assert(nextIndex < m_vContainer.size());
        return &m_vContainer[nextIndex];
    }

    // Decrement the given index, wrapping around the container size
    size_type decrement(size_type index) const noexcept { return (index == 0) ? m_vContainer.size() - 1 : index - 1; }

    // Increment the given index, wrapping around the container size
    size_type increment(size_type index) const noexcept { return (index == m_vContainer.size() - 1) ? 0 : index + 1; }

    container_type m_vContainer;
    size_type m_Head = 0;
    size_type m_Tail = 0;
    size_type m_Size = 0;
};
#endif

// clang-format off
namespace CircularQueueTest
{
extern void RunCircularQueueTest();
}
// clang-format on
