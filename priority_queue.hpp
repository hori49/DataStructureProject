#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP
#include "hash_table.hpp"
#include <ostream>

template <typename ValueType>
struct Slot_queue
{
    unsigned key;
    ValueType value;
};

template <typename ValueType>
class PriorityQueue
{
public:
    /**
     * Creates a priority queue that can have at most @maxSize elements.
     *
     * Throws std::runtime_error if @maxSize is 0.
     */
    explicit PriorityQueue(unsigned maxSize) {
        size_max = maxSize;
        unsigned prime_num = 2 * maxSize;

        if (maxSize == 0) {
            throw std::runtime_error("maxSize cannot be 0.");
        }

        slot = new Slot_queue<ValueType>[maxSize];

        bool is_prime = false;
        prime_num--; // For the case where size_max was prime number
        while(!is_prime) {
            prime_num++;
            bool flag_prime = true;
            for (unsigned i = 2; i < prime_num; ++i) {
                if (prime_num % i == 0) {
                    flag_prime = false;
                    break;
                }
            }
            if(flag_prime) {
                is_prime = true;
            }
        }

        mapping = new HashTable<unsigned> (prime_num);
    }

    ~PriorityQueue() {
        delete[] slot;
        // delete mapping;
    }

    /**
     * Makes the underlying implementation details (including the max size) look
     * exactly the same as that of @rhs.
     */
    PriorityQueue(const PriorityQueue& rhs) {
        slot = new struct Slot_queue<ValueType>[rhs.size_max];
        size_max = rhs.size_max;
        num_element = rhs.num_element;

        for (std::size_t i = 0; i < size_max; ++i) {
            slot[i] = rhs.slot[i];
        }
        mapping = rhs.mapping;
    }

    PriorityQueue& operator=(const PriorityQueue& rhs) {
        slot = new struct Slot_queue<ValueType>[rhs.size_max];
        size_max = rhs.size_max;
        num_element = rhs.num_element;

        for (std::size_t i = 0; i < size_max; ++i) {
            slot[i] = rhs.slot[i];
        }
        mapping = rhs.mapping;
        return *this;
    }

    /**
     * Takes the underlying implementation details of @rhs
     * and gives them to "this" object.
     * After this, @rhs should be in a "moved from" state.
     */
    PriorityQueue(PriorityQueue&& rhs) noexcept {
        slot = rhs.slot;
        size_max = rhs.size_max;
        num_element = rhs.num_element;
        rhs.slot = nullptr;
        rhs.size_max = 0;
        rhs.num_element = 0;
    }

    PriorityQueue& operator=(PriorityQueue&& rhs) noexcept {
        slot = rhs.slot;
        size_max = rhs.size_max;
        num_element = rhs.num_element;
        rhs.slot = nullptr;
        rhs.size_max = 0;
        rhs.num_element = 0;

        return *this;
    }


    /**
     * Both of these must run in constant time.
     */
    unsigned numElements() const {
        return num_element;
    }
    unsigned maxSize() const {
        return size_max;
    }

    /**
     * Print the underlying heap level-by-level.
     */
    friend std::ostream& operator<<(
        std::ostream& os,
        const PriorityQueue<ValueType>& pq)
    {
        unsigned x = 0;
        unsigned y = 2;
        for (unsigned i = 0; i < pq.num_element; ++i) {
            os << "(" << pq.slot[i].key << "," << pq.slot[i].value << ") ";
            if (i == x && (i != pq.num_element - 1)) {
                os << "\n";
                x = x + y;
                y *= 2;
            }
        }
        os << "\n";
        return os;
    }

    void swap(unsigned index1, unsigned index2) {
        *(mapping->get(slot[index1].key)) = index2;
        *(mapping->get(slot[index2].key)) = index1;

        auto tmp = slot[index1];
        slot[index1] = slot[index2];
        slot[index2] = tmp;
    }

    void up(unsigned index) {
        if (index == 0) return;
        unsigned parent = ((index + 1) / 2) - 1;
        while (slot[parent].key > slot[index].key) {
            swap(parent, index);
            index = parent;
            if (index == 0) return;
            parent = ((index + 1) / 2) - 1;
        }
    }

    void down(unsigned index) {
        unsigned left_child = 2 * (index + 1) - 1;
        unsigned right_child = (2 * (index + 1)) + 1 - 1;
        unsigned small_child;
        if (slot[left_child].key < slot[right_child].key) {
            small_child = left_child;
        } else {
            small_child = right_child;
        }

        while (small_child < num_element && slot[index].key > slot[small_child].key) {
            swap(index, small_child);
            index = small_child;
            left_child = 2 * (index + 1) - 1;
            right_child = (2 * (index + 1)) + 1 - 1;

            if (left_child >= num_element && right_child >= num_element) {
                return;
            }
            else if (left_child >= num_element && right_child < num_element) {
                small_child = right_child;
            }
            else if (left_child < num_element && right_child >= num_element)
            {
                small_child = left_child;
            }
            else {
                if (slot[left_child].key < slot[right_child].key) {
                    small_child = left_child;
                } else {
                    small_child = right_child;
                }
            }
        }
    }

    /**
     * Inserts a key-value pair mapping @key to @value into
     * the priority queue.
     *
     * Returns true if success.
     *
     * Returns false if @key is already in the priority queue
     * or if max size would be exceeded.
     */
    bool insert(unsigned key, const ValueType& value) {
        if (num_element == size_max) {
            return false;
        }

        unsigned* value_address = mapping -> get(key);
        if (value_address != nullptr) {
            return false;
        }

        slot[num_element].key = key;
        slot[num_element].value = value;
        mapping->insert(key,num_element);
        up(num_element);
        num_element++;
        return true;
    }


    /**
     * Returns key of the smallest element in the priority queue
     * or null pointer if empty.
     *
     * The pointer may be invalidated if the priority queue is modified.
     */
    const unsigned* getMinKey() const {
        if (num_element == 0) {
            return nullptr;
        }

        return &slot[0].key;
    }


    /**
     * Returns value of the smallest element in the priority queue
     * or null pointer if empty.
     *
     * The pointer may be invalidated if the priority queue is modified.
     */
    const ValueType* getMinValue() const {
        if (num_element == 0) {
            return nullptr;
        }

        return &slot[0].value;
    }


    /**
     * Removes the root of the priority queue.
     *
     * Returns true if success.
     * Returns false if priority queue is empty, i.e. nothing to delete.
     */
    bool deleteMin() {
        if (num_element == 0) {
            return false;
        }

        // The most right bottom slot will move to the root
        slot[0].key = slot[num_element - 1].key;
        slot[0].value = slot[num_element - 1].value;
        num_element--;

        // Move the new root down.
        down(0);

        return true;
    }


    /**
     * Returns address of the value that @key is mapped to in the priority queue.
     *
     * Returns null pointer if @key is not in the table.
     */
    ValueType* get(unsigned key) {
        // when the key exists in the table.
        unsigned* mp = mapping -> get(key);

        if (mp != nullptr) {
            return &slot[*mp].value;
        }

        return nullptr;
    }

    const ValueType* get(unsigned key) const {
        return ((PriorityQueue*) this) -> get(key);
    }


    /**
     * Subtracts/adds @change from/to the key of
     * the element that has key @key.
     *
     * Returns true if success.
     * Returns false if any of the following:
     * - @change is 0.
     * - @key not found.
     * - If the change would lead to a duplicate key.
     *
     * The function does not do anything about  overflow/underflow.
     * For example, an operation like decreaseKey(2, 10)
     * has an undefined effect.
     */
    bool decreaseKey(unsigned key, unsigned change) {
        unsigned* mp1 = mapping -> get(key);

        if (change == 0 || mp1 == nullptr || key < change) {
            return false;
        }

        // Return false if the change would lead to a duplication.
        unsigned* mp2 = mapping -> get(key - change);
        if (mp2 != nullptr) {
            return false;
        }

        // The case which result will be true.
        mapping->insert(key-change,*mp1);
        slot[*mp1].key = key - change;
        up(*mp1);
        mapping->remove(key);
        return true;
    }

    bool increaseKey(unsigned key, unsigned change) {
        unsigned* mp1 = mapping -> get(key);

        if (change == 0 || mp1 == nullptr) {
            return false;
        }

        // Return false if the change would lead to a duplication.
        unsigned* mp2 = mapping -> get(key - change);
        if (mp2 != nullptr) {
            return false;
        }

        // The case which result will be true.
        mapping->insert(key + change, *mp1);
        slot[*mp1].key = key + change;
        down(*mp1);
        mapping->remove(key);
        return true;
    }


    /**
     * Removes element that has key @key.
     *
     * Returns true if success.
     * Returns false if @key not found.
     */
    bool remove(unsigned key)
    {
        // Find the address of the target key node.
        unsigned *mp = mapping -> get(key);

        if (mp == nullptr) {
            return false;
        }

        // Replace it with the most right bottom node.
        slot[*mp].key = slot[num_element - 1].key;
        slot[*mp].value = slot[num_element - 1].value;
        num_element--;

        // Need to modify the hash_table
        mapping->remove(key);

        // Move the node up or down.
        up(*mp);
        down(*mp);
        return true;
    }
private:
    struct Slot_queue<ValueType> *slot;
    HashTable<unsigned> *mapping;
    unsigned num_element = 0;
    unsigned size_max;
};

#endif  // PRIORITY_QUEUE_HPP