#include <iostream>
#include <memory>
#include <initializer_list>
#include <map>
using namespace std;

template <typename T>
struct Node
{
    shared_ptr<Node<T>> prev;
    shared_ptr<Node<T>> next;
    T value;
    size_t index;

    Node(shared_ptr<Node<T>> p, shared_ptr<Node<T>> n, T v, size_t i)
        : prev(p), next(n), value(v), index(i) {
    }
};

template <typename T>
class LinkedList
{
private:
    shared_ptr<Node<T>> firstNode = nullptr;
    shared_ptr<Node<T>> lastNode = nullptr;
    size_t size = 0;
public:
    LinkedList() = default;
    LinkedList(T value)
    {
        auto first = make_shared<Node<T>>(nullptr, nullptr, value, 0);
        this->firstNode = this->lastNode = first;
        size++;
    }
    LinkedList(initializer_list<T> values)
    {
        for (auto value : values) push(value);
    }

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

    size_t length() const
    {
        return this->size;
    }

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

    void push(T element)
    {
        auto temp = make_shared<Node<T>>(lastNode, nullptr, element, size);
        if (lastNode) lastNode->next = temp;
        lastNode = temp;
        if (!firstNode) firstNode = lastNode;
        size++;
    }

    void unshift(T element)
    {
        auto temp = make_shared<Node<T>>(nullptr, firstNode, element, 0);
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
            auto newNode = make_shared<Node<T>>(temp, temp->next, value, index); 
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
        
    void remove(T value)
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

    shared_ptr<Node<T>> getFirstNode() const { return firstNode; }
};

template <typename K, typename V>
class HashTable
{
private:
    LinkedList<pair<K, V>> entries;
    size_t size = 0;
public:
    HashTable() = default;
    HashTable(initializer_list<pair<K, V>> initList)
    {
        for (auto& entry : initList)
        {
            entries.push(entry); size++;
        }
    }

    size_t length() const
    {
        return this->size;
    }

    void add(K key, V value)
    {
        auto temp = entries.getFirstNode();
        while (temp)
        {
            if (temp->value.first == key) return;
            temp = temp->next;
        }

        entries.push(make_pair(key, value));
        size++;
    }

    void remove(K key)
    {
        auto temp = entries.getFirstNode();
        size_t index = 0;
        while (temp)
        {
            if (temp->value.first == key)
            {
                entries.removeAt(index);
                size--;
                return;
            }
            index++;
            temp = temp->next;
        }
    }

    V get(K key)
    {
        auto temp = entries.getFirstNode();
        while (temp)
        {
            if (temp->value.first == key)
            {
                return temp->value.second;
            }
            temp = temp->next;
        }
    }

    void clear()
    {
        entries.clear();
        size = 0;
    }

    K* keys()
    {
        K* arr = new K[this->size];

        auto temp = entries.getFirstNode();
        size_t index = 0;
        while (temp)
        {
            arr[index] = temp->value.first;
            temp = temp->next;
            index++;
        }

        return arr;
    }

    V* values()
    {
        V* arr = new V[this->size];

        auto temp = entries.getFirstNode();
        size_t index = 0;
        while (temp)
        {
            arr[index] = temp->value.second;
            temp = temp->next;
            index++;
        }

        return arr;
    }

    HashTable<K, V> clone() const
    {
        HashTable<K, V> copy;
        auto temp = entries.getFirstNode();
        while (temp)
        {
            copy.add(temp->value.first, temp->value.second);
            temp = temp->next;
        }
        return copy;
    }

    friend ostream& operator<<(ostream& os, const HashTable<K, V>& table)
    {
        auto temp = table.entries.getFirstNode();
        os << "{ ";
        while (temp)
        {
            os << "{" << temp->value.first << ": " << temp->value.second << "} ";
            temp = temp->next;
        }
        os << "}";
        return os;
    }
};


int main()
{
    LinkedList<int> list({ 1, 2, 3 });
    cout << list << endl;

    list.push(4);
    list.unshift(0);
    cout << list << endl;

    list.insert(2, { 5, 6, 7 });
    cout << list << endl;

    list.remove(5);
    cout << list << endl;

    list.removeAt(1);
    cout << list << endl;

    list.removeFirst();
    list.removeLast();
    cout << list << endl;

    LinkedList<int> clonedList = list.clone();
    cout << clonedList << endl;

    clonedList.clear();
    cout << clonedList << " " << clonedList.length() << endl;
    cout << list << " " << list.length() << endl;

    auto array = list.toArray();
    for (size_t i = 0; i < list.length(); i++)
    {
        cout << array[i] << " ";
    }

    cout << "\n\n==============================\n\n";

    HashTable<int, string> hashTable({ {1, "one"}, {2, "two"}, {3, "three"} });
    cout << hashTable << endl;

    hashTable.add(4, "four");
    cout << hashTable << endl;

    hashTable.add(2, "two_already_created");
    cout << hashTable << endl;

    hashTable.remove(3); 
    cout << hashTable << endl;

    cout << hashTable.get(2) << endl;

    HashTable<int, string> clonedHashTable = hashTable.clone();
    cout << clonedHashTable << endl;

    clonedHashTable.clear();
    cout << clonedHashTable << " Length: " << clonedHashTable.length() << endl;
    cout << hashTable << " Length: " << hashTable.length() << endl;

    auto keys = hashTable.keys();
    for (size_t i = 0; i < hashTable.length(); i++)
    {
        cout << keys[i] << " ";
    }
    cout << endl;

    auto values = hashTable.values();
    for (size_t i = 0; i < hashTable.length(); i++)
    {
        cout << values[i] << " ";
    }
    cout << endl;
}