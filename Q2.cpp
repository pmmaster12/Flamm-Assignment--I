#include <bits/stdc++.h>
using namespace std;

class MyHashMap {
private:
    // Node structure for linked list to handle collisions
    struct Node {
        int key;
        int value;
        Node* next;
        Node(int k, int v) : key(k), value(v), next(nullptr) {}
    };

    // Size of the hash table
    static const int SIZE = 10007;  
    vector<Node*> table;

    // Hash function
    int hash(int key) {
        return key % SIZE;
    }

public:
    MyHashMap() {
        table.resize(SIZE, nullptr);
    }

    void put(int key, int value) {
        int index = hash(key);
        
        // If key already exists, update its value
        Node* current = table[index];
        while (current != nullptr) {
            if (current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }

        // If key doesn't exist, create new node
        Node* newNode = new Node(key, value);
        newNode->next = table[index];
        table[index] = newNode;
    }

    int get(int key) {
        int index = hash(key);
        Node* current = table[index];

        while (current != nullptr) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }

        return -1;  // Key not found
    }

    void remove(int key) {
        int index = hash(key);
        Node* current = table[index];
        Node* prev = nullptr;

        while (current != nullptr) {
            if (current->key == key) {
                if (prev == nullptr) {
                    // If node to remove is the first node
                    table[index] = current->next;
                } else {
                    // If node to remove is in the middle or end
                    prev->next = current->next;
                }
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        }
    }

    // Destructor to clean up memory
    ~MyHashMap() {
        for (int i = 0; i < SIZE; i++) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
};

int main() {
    MyHashMap map;
    
    // Test cases
    cout << "Testing HashMap implementation:" << endl;
    
    // Test put and get
    map.put(1, 1);
    map.put(2, 2);
    cout << "get(1): " << map.get(1) << endl;  // Should return 1
    cout << "get(2): " << map.get(2) << endl;  // Should return 2
    cout << "get(3): " << map.get(3) << endl;  // Should return -1
    
    // Test update
    map.put(2, 1);
    cout << "get(2) after update: " << map.get(2) << endl;  // Should return 1
    
    // Test remove
    map.remove(2);
    cout << "get(2) after remove: " << map.get(2) << endl;  // Should return -1
    
    // Test edge cases
    map.put(0, 0);
    cout << "get(0): " << map.get(0) << endl;  // Should return 0
    
    map.put(1000000, 1000000);
    cout << "get(1000000): " << map.get(1000000) << endl;  // Should return 1000000
    
    return 0;
}
