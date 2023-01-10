#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <iostream>
/**
 * Implementation of a hash table that stores key-value
 * pairs mapping unsigned integers to instances of
 * ValueType.
 *
 * Hash function: key % tableSize
 * Collision resolution: quadratic probing.
 * Non-unique keys are not supported.
 */

template <typename ValueType>
struct Slot
{
    unsigned key;
    ValueType value;
    bool is_empty = true;
    bool is_deleted = false;
};

template <typename ValueType>
class HashTable
{
public:
    /**
     * Creates a hash table with the given number of
     * buckets/slots.
     *
     * Throws std::runtime_error if @tableSize is 0 or not
     * prime.
     */
    explicit HashTable(unsigned tableSize) {
        size_table = tableSize;

        if (tableSize == 0) {
            throw std::runtime_error("Table size is 0.");
        }

        for (unsigned i = 2; i < tableSize / 2; i++) {
            if ((tableSize % i) == 0) {
                throw std::runtime_error("Table size is NOT prime.");
            }
        }
        slot = new Slot<ValueType>[tableSize];
    }

    ~HashTable() {
        delete[] slot;
    }

    /**
     * Makes the underlying hash table of this object look
     * exactly the same as that of @rhs.
     */
    HashTable(const HashTable& rhs) {
        slot = new struct Slot<ValueType>[rhs.size_table];
        size_table = rhs.size_table;
        num_element = rhs.num_element;

        for (std::size_t i = 0; i < size_table; ++i) {
            slot[i] = rhs.slot[i];
        }
    }

    HashTable& operator=(const HashTable& rhs) {
        slot = new struct Slot<ValueType>[rhs.size_table];
        size_table = rhs.size_table;
        num_element = rhs.num_element;

        for (std::size_t i = 0; i < size_table; ++i) {
            slot[i] = rhs.slot[i];
        }

        return *this;
    }

    /**
     * Takes the underlying implementation details of @rhs
     * and gives them to "this" object.
     * After this, @rhs should be in a "moved from" state.
     */
    HashTable(HashTable&& rhs) noexcept {
        slot = rhs.slot;
        size_table = rhs.size_table;
        num_element = rhs.num_element;
        rhs.slot = nullptr;
        rhs.size_table = 0;
        rhs.num_element = 0;
    }

    HashTable& operator=(HashTable&& rhs) noexcept {
        slot = rhs.slot;
        size_table = rhs.size_table;
        num_element = rhs.num_element;
        rhs.slot = nullptr;
        rhs.size_table = 0;
        rhs.num_element = 0;

        return *this;
    }

    /**
     * Both of these must run in constant time.
     */
    unsigned tableSize() const {
        return size_table;
    }
    unsigned numElements() const {
        return num_element;
    }

    /**
     * Prints each bucket in the hash table.
     */
    friend std::ostream& operator<<(std::ostream& os,
                                    const HashTable<ValueType>& ht)
    {
        for (std::size_t i = 0; i < ht.size_table; ++i) {
            if (ht.slot[i].is_empty || ht.slot[i].is_deleted) {
                os << "Bucket " << i << ": " << "(empty)" << std::endl;
            } else {
                os << "Bucket " << i << ": " << ht.slot[i].key << " -> " << ht.slot[i].value << std::endl;
            }
        }

        return os;
    }
    void rehash(unsigned key, const ValueType& value) {
        bool is_prime = false;
        unsigned size_prev_table = size_table;

        if(((double)(num_element + 1) / (double)size_table) >= 0.5) {

            size_table *= 2;
            while(!is_prime) {
                size_table++;
                bool flag_prime = true;
                for (unsigned i = 2; i < size_table; ++i) {
                    if (size_table % i == 0) {
                        flag_prime = false;
                        break;
                    }
                }
                if(flag_prime) {
                    is_prime = true;
                }
            }

            unsigned copy_key[size_prev_table];
            ValueType copy_value[size_prev_table];
            unsigned copy_array_index = 0;

            for (unsigned i = 0; i < size_prev_table; ++i) {
                if (!slot[i].is_empty && !slot[i].is_deleted) {
                    copy_key[copy_array_index] = slot[i].key;
                    copy_value[copy_array_index] = slot[i].value;
                    copy_array_index++;
                }
            }

            delete[] slot;
            slot = new Slot<ValueType>[size_table];

            num_element = 0;
            for (unsigned i = 0; i < copy_array_index; ++i) {
                insert(copy_key[i], copy_value[i]);
            }

            insert(key, value);
        }
    }
    /**
     * Inserts a key-value pair mapping @key to @value into
     * the table.
     *
     * Returns true if success.
     * Returns false if @key is already in the table
     * (in which case, the insertion is not performed).
     */
    bool insert(unsigned key, const ValueType& value) {
        unsigned quadratic = 0;
        unsigned slot_insert_index =(key + (quadratic * quadratic)) % size_table;
        bool flag_unique = false;

        while (true) {
            Slot<ValueType>* tmp_slot = &slot[(key + (quadratic * quadratic)) % size_table];

            if (!tmp_slot->is_empty && tmp_slot->key == key) {
                return false;
            }

            if (tmp_slot->is_empty && !flag_unique) {
                flag_unique = true;
                slot_insert_index = (key + (quadratic * quadratic)) % size_table;
            }

            if(tmp_slot->is_empty && !tmp_slot->is_deleted) {
                break;
            }

            quadratic++;
        }

        if (((double)(num_element + 1) / (double)size_table) >= 0.5){
            rehash(key, value);
        } else {
            slot[slot_insert_index].key = key;
            slot[slot_insert_index].value = value;
            slot[slot_insert_index].is_empty = false;
            slot[slot_insert_index].is_deleted = false;
            num_element++;
        }
        return true;
    }

    /**
     * Finds the value corresponding to the given key
     * and returns its address.
     *
     * Returns null pointer if @key is not in the table.
     */
    ValueType* get(unsigned key) {
        // bool flag_found = false;
        // unsigned index_original = key % size_table;
        // slot_insert_index = (key + (quadratic * quadratic)) % size_table;
        unsigned slot_insert_index;
        unsigned quadratic = 0;

        while (true) {
            slot_insert_index = (key + (quadratic * quadratic)) % size_table;

            if(!slot[slot_insert_index].is_empty && slot[slot_insert_index].key == key) {
                return &slot[slot_insert_index].value;

            } else if (slot[slot_insert_index].is_empty && !slot[slot_insert_index].is_deleted) {
                break;
            } else {
                quadratic++;
            }
        }
        return nullptr;
    }

    const ValueType* get(unsigned key) const {
        return ((HashTable*) this) -> get(key);
    }

    /**
     * Updates the key-value pair with key @key to be
     * mapped to @newValue.
     *
     * Returns true if success.
     * Returns false if @key is not in the table.
     */
    bool update(unsigned key, const ValueType& newValue) {
        unsigned slot_insert_index;
        unsigned quadratic = 0;

        while (true) {
            slot_insert_index = (key + (quadratic * quadratic)) % size_table;

            if(!slot[slot_insert_index].is_empty && slot[slot_insert_index].key == key) {
                slot[slot_insert_index].value = newValue;
                return true;

            } else if (slot[slot_insert_index].is_empty && !slot[slot_insert_index].is_deleted) {
                break;
            } else {
                quadratic++;
            }
        }
        return false;
    }

    /**
     * Deletes the element that has the given key.
     *
     * Returns true if success.
     * Returns false if @key not found.
     */
    bool remove(unsigned key) {
        bool flag_found = false;
        unsigned slot_insert_index;
        unsigned quadratic = 0;

        while (!flag_found) {
            slot_insert_index = (key + (quadratic * quadratic)) % size_table;

            if(slot[slot_insert_index].key == key) {
                slot[slot_insert_index].is_deleted = true;
                slot[slot_insert_index].is_empty = true;
                num_element--;
                flag_found = true;
            } else if (slot[slot_insert_index].is_empty) {
                break;
            } else {
                quadratic++;
            }
        }
        return flag_found;
    }

    /**
     * Deletes all elements that have the given value.
     *
     * Returns the number of elements deleted.
     */
    unsigned removeAllByValue(const ValueType& value) {
        unsigned num_deleted = 0;
        for (unsigned i = 0; i < size_table; ++i) {
            if (!slot[i].is_empty && slot[i].value == value) {
                slot[i].is_empty = true;
                slot[i].is_deleted = true;
                num_deleted++;
            }
        }
        num_element -= num_deleted;
        return num_deleted;
    }

    /**
     * Two instances of HashTable<ValueType> are considered
     * equal if they contain the same elements, even if those
     * elements are in different buckets (i.e. even if the
     * hash tables have different sizes).
     */
    bool operator==(const HashTable& rhs) const {
        if (num_element != rhs.num_element) {
            return false;
        }

        for (unsigned i = 0; i < size_table; ++i) {
            if(!slot[i].is_empty) {
                const ValueType* value = rhs.get(slot[i].key);
                if(value == nullptr || *value != slot[i].value) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const HashTable& rhs) const {
        if (num_element != rhs.num_element) {
            return true;
        }

        for (unsigned i = 0; i < size_table; ++i) {

            if(!slot[i].is_empty) {
                const ValueType* value = rhs.get(slot[i].key);
                if(value == nullptr || *value != slot[i].value) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Returns a newly constructed hash table that is the hash table
     * that would result from inserting each element from @rhs
     * (in the order that they appear in the buckets)
     * into this (i.e. *this) hash table.
     */
    HashTable operator+(const HashTable& rhs) const {
        HashTable<ValueType> hash_table = *this;

        for(unsigned i = 0; i < rhs.size_table; ++i) {
            if (!rhs.slot[i].is_empty) {
                hash_table.insert(rhs.slot[i].key, rhs.slot[i].value);
            }
        }
        return hash_table;
    }

private:
    struct Slot<ValueType> *slot;
    unsigned size_table;
    unsigned num_element = 0;
};

#endif  // HASH_TABLE_HPP
