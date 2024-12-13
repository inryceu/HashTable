#include <iostream>
#include <memory>
#include <initializer_list>
#include <vector>
using namespace std;

template <typename T>
class LinkedList
{
private:
    struct Node
    {
        shared_ptr<Node> prev;
        shared_ptr<Node> next;
        T value;
        size_t index;

        Node(shared_ptr<Node> p, shared_ptr<Node> n, T v, size_t i)
            : prev(p), next(n), value(v), index(i) {
        }
    };

    shared_ptr<Node> firstNode = nullptr;
    shared_ptr<Node> lastNode = nullptr;
    size_t size = 0;
public:
    LinkedList() = default;
    LinkedList(const T& value)
    {
        auto first = make_shared<Node>(nullptr, nullptr, value, 0);
        this->firstNode = this->lastNode = first;
        size++;
    }
    LinkedList(initializer_list<T> values)
    {
        for (const auto& value : values) push(value);
    }

    class Iterator
    {
    private:
        shared_ptr<Node> current;

    public:
        Iterator(shared_ptr<Node> node) : current(node) {}

        T& operator*() { return current->value; }

        Iterator& operator++()
        {
            if (current) {
                current = current->next;
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    Iterator begin() const { return Iterator(firstNode); }
    Iterator end() const { return Iterator(nullptr); }

    T first() const
    {
        if (firstNode) return firstNode->value;
        else throw out_of_range("List is empty");
    }

    T last() const
    {
        if (lastNode) return lastNode->value;
        else throw out_of_range("List is empty");
    }

    size_t length() const { return this->size; }

    T& at(size_t index)
    {
        if (index >= this->size) throw out_of_range("Index out of range");
        auto temp = firstNode;
        while (temp->index != index)
        {
            temp = temp->next;
        }

        return temp->value;
    }

    T& operator[](size_t index)
    {
        return this->at(index);
    }

    void push(const T& element)
    {
        auto temp = make_shared<Node>(lastNode, nullptr, element, size);
        if (lastNode) lastNode->next = temp;
        lastNode = temp;
        if (!firstNode) firstNode = lastNode;
        size++;
    }

    void unshift(const T& element)
    {
        auto temp = make_shared<Node>(nullptr, firstNode, element, 0);
        if (firstNode) firstNode->prev = temp;
        firstNode = temp;
        if (!lastNode) lastNode = firstNode;

        temp = firstNode->next;
        while (temp)
        {
            temp->index++;
            temp = temp->next;
        }
        size++;
    }

    void insert(size_t index, initializer_list<T> values)
    {
        if (index > this->size) throw out_of_range("Index out of range");

        auto temp = firstNode;
        for (size_t i = 0; i < index; i++)
        {
            temp = temp->next;
        }

        for (auto value : values)
        {
            auto newNode = make_shared<Node>(temp, temp->next, value, index);
            temp->next->prev = newNode;
            temp->next = newNode;

            if (index == 0) { firstNode = newNode; }
            else if (index == size) { lastNode = newNode; }
            temp = newNode;
            index++;
            size++;
        }
        while (temp)
        {
            temp->index = index;
            temp = temp->next;
            index++;
        }
    }

    void remove(const T& value)
    {
        auto temp = firstNode;
        while (temp)
        {
            if (temp->value == value)
            {
                if (temp->prev) temp->prev->next = temp->next;
                if (temp->next) temp->next->prev = temp->prev;
                if (temp == firstNode) firstNode = temp->next;
                if (temp == lastNode) lastNode = temp->prev;
                size--;
                break;
            }
            temp = temp->next;
        }

        temp = firstNode;
        size_t index = 0;
        while (temp)
        {
            temp->index = index++;
            temp = temp->next;
        }
    }

    void removeAt(size_t index)
    {
        if (index >= this->size) throw out_of_range("Index out of range");

        auto temp = firstNode;
        for (size_t i = 0; i < index; i++)
        {
            temp = temp->next;
        }
        if (temp->prev) temp->prev->next = temp->next;
        if (temp->next) temp->next->prev = temp->prev;
        if (temp == firstNode) firstNode = temp->next;
        if (temp == lastNode) lastNode = temp->prev;
        size--;

        temp = firstNode;
        size_t i = 0;
        while (temp)
        {
            temp->index = i++;
            temp = temp->next;
        }
    }

    void removeFirst()
    {
        if (!firstNode) throw out_of_range("List is empty");

        auto temp = firstNode;
        firstNode = firstNode->next;
        if (firstNode) firstNode->prev = nullptr;
        size--;

        temp = firstNode;
        size_t index = 0;
        while (temp)
        {
            temp->index = index++;
            temp = temp->next;
        }

        if (size == 0) lastNode = nullptr;
    }

    void removeLast()
    {
        if (!lastNode) throw out_of_range("List is empty");

        auto temp = lastNode;
        lastNode = lastNode->prev;
        if (lastNode) lastNode->next = nullptr;
        size--;

        if (size == 0) firstNode = nullptr;
    }

    void clear()
    {
        firstNode = lastNode = nullptr;
        size = 0;
    }

    LinkedList<T> clone() const
    {
        LinkedList<T> copy;
        auto temp = firstNode;
        while (temp)
        {
            copy.push(temp->value);
            temp = temp->next;
        }
        return copy;
    }

    T* toArray()
    {
        T* arr = new T[this->size];

        auto temp = firstNode;
        size_t index = 0;
        while (temp)
        {
            arr[index] = temp->value;
            temp = temp->next;
            index++;
        }

        return arr;
    }

    friend ostream& operator<<(ostream& os, const LinkedList<T>& list)
    {
        auto temp = list.firstNode;
        os << "[";
        while (temp)
        {
            os << temp->value;
            temp = temp->next;
            if (temp) os << ", ";
        }
        os << "]";

        return os;
    }
};

template <typename K, typename V>
class HashTable
{
private:
    vector<LinkedList<pair<K, V>>> buckets;
    size_t bucketCount;
    size_t size = 0;

    size_t getBucketIndex(const K& key) const
    {
        return hash<K>{}(key) % bucketCount;
    }

public:
    HashTable(size_t bucketCount = 16) : bucketCount(bucketCount)
    {
        buckets.resize(bucketCount);
    }

    HashTable(initializer_list<pair<K, V>> initList, size_t bucketCount = 16)
        : bucketCount(bucketCount)
    {
        buckets.resize(bucketCount);
        for (const auto& entry : initList)
        {
            add(entry.first, entry.second);
        }
    }

    class HashTableIterator
    {
    private:
        typename vector<LinkedList<pair<K, V>>>::iterator bucketIterator;
        typename LinkedList<pair<K, V>>::Iterator listIterator;

    public:
        HashTableIterator(typename vector<LinkedList<pair<K, V>>>::iterator bucketIt)
            : bucketIterator(bucketIt), listIterator(bucketIt->begin()) {
        }

        bool operator!=(const HashTableIterator& other) const
        {
            return bucketIterator != other.bucketIterator || listIterator != other.listIterator;
        }

        void operator++()
        {
            ++listIterator;
            if (listIterator == bucketIterator->end()) {
                ++bucketIterator;
                if (bucketIterator != bucketIterator->end()) {
                    listIterator = bucketIterator->begin();
                }
            }
        }

        const pair<K, V>& operator*() const { return *listIterator; }

        pair<K, V>& operator*() { return *listIterator; }
    };

    HashTableIterator begin() const
    {
        for (auto it = buckets.begin(); it != buckets.end(); ++it)
        {
            if (it->begin() != it->end())
                return HashTableIterator(it);
        }
        return end();
    }

    HashTableIterator end() const
    {
        return HashTableIterator(buckets.end());
    }

    void add(const K& key, const V& value)
    {
        auto& bucket = buckets[getBucketIndex(key)];

        for (auto& entry : bucket)
        {
            if (entry.first == key)
            {
                entry.second = value;
                return;
            }
        }

        bucket.push(make_pair(key, value));
        size++;
    }

    void remove(const K& key)
    {
        auto& bucket = buckets[getBucketIndex(key)];
        for (auto it = bucket.begin(); it != bucket.end(); ++it)
        {
            if ((*it).first == key)
            {
                bucket.remove(*it);
                size--;
                return;
            }
        }
    }

    V get(const K& key) const
    {
        const auto& bucket = buckets[getBucketIndex(key)];

        for (const auto& entry : bucket)
        {
            if (entry.first == key)
                return entry.second;
        }

        throw out_of_range("Key not found in hash table");
    }

    bool contains(const K& key) const
    {
        const auto& bucket = buckets[getBucketIndex(key)];
        for (const auto& entry : bucket)
        {
            if (entry.first == key)
                return true;
        }
        return false;
    }

    size_t getSize() const { return size; }
    size_t getBucketCount() const { return bucketCount; }

    void clear()
    {
        for (auto& bucket : buckets)
        {
            bucket.clear();
        }
        size = 0;
    }

    HashTable<K, V> clone() const
    {
        HashTable<K, V> newTable(bucketCount);
        for (const auto& bucket : buckets)
        {
            for (const auto& entry : bucket)
            {
                newTable.add(entry.first, entry.second);
            }
        }
        return newTable;
    }

    friend ostream& operator<<(ostream& os, const HashTable<K, V>& hashTable)
    {
        os << "{ ";
        bool first = true;
        for (const auto& bucket : hashTable.buckets)
        {
            for (const auto& entry : bucket)
            {
                if (!first) {
                    os << ", ";
                }
                os << entry.first << ": " << entry.second;
                first = false;
            }
        }
        os << " }";
        return os;
    }
};

int main() {
    cout << "Testing LinkedList:" << endl;

    LinkedList<int> list = { 1, 2, 3, 4 };
    cout << "Initial list: " << list << endl;

    list.push(5);
    cout << "After push(5): " << list << endl;

    list.unshift(0);
    cout << "After unshift(0): " << list << endl;

    cout << "First element: " << list.first() << endl;
    cout << "Last element: " << list.last() << endl;

    list.remove(3);
    cout << "After removing 3: " << list << endl;

    list.removeAt(1);
    cout << "After removing element at index 1: " << list << endl;

    list.removeFirst();
    cout << "After removing the first element: " << list << endl;

    list.removeLast();
    cout << "After removing the last element: " << list << endl;

    list.clear();
    cout << "After clear(): " << list << endl;

    cout << "\nTesting HashTable:" << endl;

    HashTable<string, int> table = { {"apple", 1}, {"banana", 2} };
    cout << "Initial HashTable: " << table << endl;

    table.add("cherry", 3);
    cout << "After add('cherry', 3): " << table << endl;

    table.add("banana", 4);
    cout << "After add('banana', 4): " << table << endl;

    cout << "Value for 'apple': " << table.get("apple") << endl;
    cout << "Does it contain 'banana'? " << (table.contains("banana") ? "Yes" : "No") << endl;

    table.remove("apple");
    cout << "After removing 'apple': " << table << endl;

    table.clear();
    cout << "After clear(): " << table << endl;

    HashTable<string, int> clonedTable = table.clone();
    cout << "Cloned table: " << clonedTable << endl;

    table.add("grape", 5);
    table.add("orange", 6);
    cout << "After re-adding items to the original table: " << table << endl;
    cout << "Cloned table (should be empty as it was cloned before adding): " << clonedTable << endl;
}