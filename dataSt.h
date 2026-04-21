#pragma once
// ============================================================
//  dataSt.h  —  The Iron Nexus Custom Data Structures
//  ALL data structures implemented manually. NO STL containers.
// ============================================================
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <climits>

using std::string;
using std::cout;
using std::endl;

// ============================================================
//  1.  DOUBLY LINKED LIST
// ============================================================
template <typename T>
struct DLLNode {
    T data;
    DLLNode<T>* prev;
    DLLNode<T>* next;
    explicit DLLNode(const T& d) : data(d), prev(nullptr), next(nullptr) {}
};

template <typename T>
class DoublyLinkedList {
public:
    DLLNode<T>* head;
    DLLNode<T>* tail;
    int size;

    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~DoublyLinkedList() { clear(); }

    void clear() {
        DLLNode<T>* cur = head;
        while (cur) {
            DLLNode<T>* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head = tail = nullptr;
        size = 0;
    }

    // Insert at front
    void insertFront(const T& val) {
        DLLNode<T>* node = new DLLNode<T>(val);
        if (!head) { head = tail = node; }
        else {
            node->next = head;
            head->prev = node;
            head = node;
        }
        size++;
    }

    // Insert at back
    void insertBack(const T& val) {
        DLLNode<T>* node = new DLLNode<T>(val);
        if (!tail) { head = tail = node; }
        else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        size++;
    }

    // Insert at 1-based position (1 = front)
    bool insertAt(int pos, const T& val) {
        if (pos < 1 || pos > size + 1) return false;
        if (pos == 1) { insertFront(val); return true; }
        if (pos == size + 1) { insertBack(val);  return true; }
        DLLNode<T>* cur = head;
        for (int i = 1; i < pos - 1; i++) cur = cur->next;
        DLLNode<T>* node = new DLLNode<T>(val);
        node->next = cur->next;
        node->prev = cur;
        if (cur->next) cur->next->prev = node;
        cur->next = node;
        size++;
        return true;
    }

    // Delete at 1-based position
    bool deleteAt(int pos) {
        if (pos < 1 || pos > size || !head) return false;
        DLLNode<T>* cur = head;
        for (int i = 1; i < pos; i++) cur = cur->next;
        if (cur->prev) cur->prev->next = cur->next;
        else           head = cur->next;
        if (cur->next) cur->next->prev = cur->prev;
        else           tail = cur->prev;
        delete cur;
        size--;
        return true;
    }

    // Get node at 1-based position (returns nullptr if invalid)
    DLLNode<T>* getAt(int pos) const {
        if (pos < 1 || pos > size) return nullptr;
        DLLNode<T>* cur = head;
        for (int i = 1; i < pos; i++) cur = cur->next;
        return cur;
    }

    // Reverse in place
    void reverse() {
        DLLNode<T>* cur = head;
        DLLNode<T>* tmp = nullptr;
        while (cur) {
            tmp = cur->prev;
            cur->prev = cur->next;
            cur->next = tmp;
            cur = cur->prev;
        }
        tmp = head;
        head = tail;
        tail = tmp;
    }

    // Forward traversal with callback
    template <typename Fn>
    void traverseForward(Fn fn) const {
        DLLNode<T>* cur = head;
        while (cur) { fn(cur->data); cur = cur->next; }
    }

    // Backward traversal with callback
    template <typename Fn>
    void traverseBackward(Fn fn) const {
        DLLNode<T>* cur = tail;
        while (cur) { fn(cur->data); cur = cur->prev; }
    }

    bool isEmpty() const { return size == 0; }
};

// ============================================================
//  2.  STACK (backed by DoublyLinkedList, LIFO at front)
// ============================================================
template <typename T>
class Stack {
    DoublyLinkedList<T> list;
public:
    void push(const T& val) { list.insertFront(val); }

    T pop() {
        if (list.isEmpty()) throw std::underflow_error("Stack underflow");
        T val = list.head->data;
        list.deleteAt(1);
        return val;
    }

    T& peek() {
        if (list.isEmpty()) throw std::underflow_error("Stack is empty");
        return list.head->data;
    }

    bool isEmpty() const { return list.isEmpty(); }
    int  getSize() const { return list.size; }

    // Expose the underlying DLL for log traversal
    const DoublyLinkedList<T>& getList() const { return list; }
};

// ============================================================
//  3.  AVL TREE
// ============================================================
template <typename T, typename KeyType>
struct AVLNode {
    T        data;
    KeyType  key;
    int      height;
    AVLNode* left;
    AVLNode* right;
    AVLNode(const T& d, const KeyType& k)
        : data(d), key(k), height(1), left(nullptr), right(nullptr) {
    }
};

template <typename T, typename KeyType>
class AVLTree {
public:
    AVLNode<T, KeyType>* root;

    AVLTree() : root(nullptr) {}
    ~AVLTree() { destroyTree(root); }

private:
    void destroyTree(AVLNode<T, KeyType>* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    int height(AVLNode<T, KeyType>* n) const { return n ? n->height : 0; }
    int balanceFactor(AVLNode<T, KeyType>* n) const {
        return n ? height(n->left) - height(n->right) : 0;
    }
    void updateHeight(AVLNode<T, KeyType>* n) {
        if (n) n->height = 1 + std::max(height(n->left), height(n->right));
    }

    // Right rotation
    AVLNode<T, KeyType>* rotateRight(AVLNode<T, KeyType>* y) {
        AVLNode<T, KeyType>* x = y->left;
        AVLNode<T, KeyType>* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Left rotation
    AVLNode<T, KeyType>* rotateLeft(AVLNode<T, KeyType>* x) {
        AVLNode<T, KeyType>* y = x->right;
        AVLNode<T, KeyType>* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    AVLNode<T, KeyType>* balance(AVLNode<T, KeyType>* n) {
        updateHeight(n);
        int bf = balanceFactor(n);
        // LL
        if (bf > 1 && balanceFactor(n->left) >= 0)
            return rotateRight(n);
        // LR
        if (bf > 1 && balanceFactor(n->left) < 0) {
            n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        // RR
        if (bf < -1 && balanceFactor(n->right) <= 0)
            return rotateLeft(n);
        // RL
        if (bf < -1 && balanceFactor(n->right) > 0) {
            n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    AVLNode<T, KeyType>* insert(AVLNode<T, KeyType>* node, const T& data, const KeyType& key) {
        if (!node) return new AVLNode<T, KeyType>(data, key);
        if (key < node->key)      node->left = insert(node->left, data, key);
        else if (key > node->key) node->right = insert(node->right, data, key);
        else { node->data = data; return node; }   // update duplicate key
        return balance(node);
    }

    AVLNode<T, KeyType>* minNode(AVLNode<T, KeyType>* n) {
        while (n->left) n = n->left;
        return n;
    }

    AVLNode<T, KeyType>* remove(AVLNode<T, KeyType>* node, const KeyType& key) {
        if (!node) return nullptr;
        if (key < node->key)      node->left = remove(node->left, key);
        else if (key > node->key) node->right = remove(node->right, key);
        else {
            if (!node->left || !node->right) {
                AVLNode<T, KeyType>* child = node->left ? node->left : node->right;
                delete node;
                return child;
            }
            AVLNode<T, KeyType>* succ = minNode(node->right);
            node->data = succ->data;
            node->key = succ->key;
            node->right = remove(node->right, succ->key);
        }
        return balance(node);
    }

    AVLNode<T, KeyType>* search(AVLNode<T, KeyType>* node, const KeyType& key) const {
        if (!node) return nullptr;
        if (key == node->key) return node;
        if (key < node->key)  return search(node->left, key);
        return search(node->right, key);
    }

    template <typename Fn>
    void inOrder(AVLNode<T, KeyType>* node, Fn fn) const {
        if (!node) return;
        inOrder(node->left, fn);
        fn(node->data);
        inOrder(node->right, fn);
    }

    template <typename Fn>
    void preOrder(AVLNode<T, KeyType>* node, Fn fn) const {
        if (!node) return;
        fn(node->data);
        preOrder(node->left, fn);
        preOrder(node->right, fn);
    }

    template <typename Fn>
    void postOrder(AVLNode<T, KeyType>* node, Fn fn) const {
        if (!node) return;
        postOrder(node->left, fn);
        postOrder(node->right, fn);
        fn(node->data);
    }

public:
    void insert(const T& data, const KeyType& key) { root = insert(root, data, key); }
    void remove(const KeyType& key) { root = remove(root, key); }

    AVLNode<T, KeyType>* search(const KeyType& key) const { return search(root, key); }

    template <typename Fn> void inOrder(Fn fn)   const { inOrder(root, fn); }
    template <typename Fn> void preOrder(Fn fn)  const { preOrder(root, fn); }
    template <typename Fn> void postOrder(Fn fn) const { postOrder(root, fn); }

    bool isEmpty() const { return root == nullptr; }

    int treeHeight() const { return height(root); }
};

// ============================================================
//  4.  BINARY SEARCH TREE  (for seat ordering)
// ============================================================
template <typename T, typename KeyType>
struct BSTNode {
    T       data;
    KeyType key;
    BSTNode* left;
    BSTNode* right;
    BSTNode(const T& d, const KeyType& k) : data(d), key(k), left(nullptr), right(nullptr) {}
};

template <typename T, typename KeyType>
class BST {
public:
    BSTNode<T, KeyType>* root;
    BST() : root(nullptr) {}
    ~BST() { destroy(root); }

private:
    void destroy(BSTNode<T, KeyType>* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    BSTNode<T, KeyType>* insert(BSTNode<T, KeyType>* n, const T& data, const KeyType& key) {
        if (!n) return new BSTNode<T, KeyType>(data, key);
        if (key < n->key)      n->left = insert(n->left, data, key);
        else if (key > n->key) n->right = insert(n->right, data, key);
        else n->data = data;
        return n;
    }

    BSTNode<T, KeyType>* minNode(BSTNode<T, KeyType>* n) {
        while (n->left) n = n->left;
        return n;
    }

    BSTNode<T, KeyType>* remove(BSTNode<T, KeyType>* n, const KeyType& key) {
        if (!n) return nullptr;
        if (key < n->key)      n->left = remove(n->left, key);
        else if (key > n->key) n->right = remove(n->right, key);
        else {
            if (!n->left || !n->right) {
                BSTNode<T, KeyType>* child = n->left ? n->left : n->right;
                delete n;
                return child;
            }
            BSTNode<T, KeyType>* succ = minNode(n->right);
            n->data = succ->data;
            n->key = succ->key;
            n->right = remove(n->right, succ->key);
        }
        return n;
    }

    BSTNode<T, KeyType>* search(BSTNode<T, KeyType>* n, const KeyType& key) const {
        if (!n) return nullptr;
        if (key == n->key) return n;
        if (key < n->key)  return search(n->left, key);
        return search(n->right, key);
    }

    template <typename Fn>
    void inOrder(BSTNode<T, KeyType>* n, Fn fn) const {
        if (!n) return;
        inOrder(n->left, fn);
        fn(n->data);
        inOrder(n->right, fn);
    }

    template <typename Fn>
    void preOrder(BSTNode<T, KeyType>* n, Fn fn) const {
        if (!n) return;
        fn(n->data);
        preOrder(n->left, fn);
        preOrder(n->right, fn);
    }

public:
    void insert(const T& data, const KeyType& key) { root = insert(root, data, key); }
    void remove(const KeyType& key) { root = remove(root, key); }
    BSTNode<T, KeyType>* search(const KeyType& key) const { return search(root, key); }
    template <typename Fn> void inOrder(Fn fn)  const { inOrder(root, fn); }
    template <typename Fn> void preOrder(Fn fn) const { preOrder(root, fn); }
    bool isEmpty() const { return root == nullptr; }
};

// ============================================================
//  5.  HASH MAP  (open addressing — linear probing)
// ============================================================
template <typename KeyType, typename ValueType>
class HashMap {
    static const int DEFAULT_CAPACITY = 101; // prime

    enum SlotState { EMPTY, OCCUPIED, DELETED };

    struct Slot {
        KeyType   key;
        ValueType value;
        SlotState state;
        Slot() : state(EMPTY) {}
    };

    Slot* table;
    int   capacity;
    int   count;

    int hashFn(int key) const { return ((key % capacity) + capacity) % capacity; }
    int hashFn(const string& key) const {
        unsigned long long h = 0;
        for (char c : key) h = h * 31 + c;
        return (int)(h % (unsigned long long)capacity);
    }

public:
    explicit HashMap(int cap = DEFAULT_CAPACITY)
        : capacity(cap), count(0) {
        table = new Slot[capacity];
    }
    ~HashMap() { delete[] table; }

    bool insert(const KeyType& key, const ValueType& val) {
        if (count >= capacity * 0.75) return false; // load factor
        int idx = hashFn(key);
        int start = idx;
        do {
            if (table[idx].state != OCCUPIED) {
                table[idx].key = key;
                table[idx].value = val;
                table[idx].state = OCCUPIED;
                count++;
                return true;
            }
            if (table[idx].state == OCCUPIED && table[idx].key == key) {
                table[idx].value = val; // update
                return true;
            }
            idx = (idx + 1) % capacity;
        } while (idx != start);
        return false;
    }

    bool get(const KeyType& key, ValueType& out) const {
        int idx = hashFn(key);
        int start = idx;
        do {
            if (table[idx].state == EMPTY) return false;
            if (table[idx].state == OCCUPIED && table[idx].key == key) {
                out = table[idx].value;
                return true;
            }
            idx = (idx + 1) % capacity;
        } while (idx != start);
        return false;
    }

    ValueType* getRef(const KeyType& key) {
        int idx = hashFn(key);
        int start = idx;
        do {
            if (table[idx].state == EMPTY) return nullptr;
            if (table[idx].state == OCCUPIED && table[idx].key == key)
                return &table[idx].value;
            idx = (idx + 1) % capacity;
        } while (idx != start);
        return nullptr;
    }

    const ValueType* getRef(const KeyType& key) const {
        int idx = hashFn(key);
        int start = idx;
        do {
            if (table[idx].state == EMPTY) return nullptr;
            if (table[idx].state == OCCUPIED && table[idx].key == key)
                return &table[idx].value;
            idx = (idx + 1) % capacity;
        } while (idx != start);
        return nullptr;
    }

    bool remove(const KeyType& key) {
        int idx = hashFn(key);
        int start = idx;
        do {
            if (table[idx].state == EMPTY) return false;
            if (table[idx].state == OCCUPIED && table[idx].key == key) {
                table[idx].state = DELETED;
                count--;
                return true;
            }
            idx = (idx + 1) % capacity;
        } while (idx != start);
        return false;
    }

    bool contains(const KeyType& key) const {
        ValueType dummy;
        return get(key, dummy);
    }

    int getCount()    const { return count; }
    int getCapacity() const { return capacity; }

    // Iterate over all occupied slots
    template <typename Fn>
    void forEach(Fn fn) const {
        for (int i = 0; i < capacity; i++)
            if (table[i].state == OCCUPIED)
                fn(table[i].key, table[i].value);
    }
};

// ============================================================
//  6.  GRAPH  (adjacency matrix, up to 20 nodes)
//      Dijkstra with a manual min-array priority queue
// ============================================================
static const int MAX_CITIES = 20;
static const int INF_DIST = INT_MAX / 2;

struct Edge {
    int distanceKm;
    int travelTimeMin;
    bool exists;
    Edge() : distanceKm(0), travelTimeMin(0), exists(false) {}
};

class Graph {
public:
    string cityNames[MAX_CITIES];
    Edge   adj[MAX_CITIES][MAX_CITIES];
    int    numCities;

    Graph() : numCities(0) {}

    // Returns city index, -1 if not found
    int findCity(const string& name) const {
        for (int i = 0; i < numCities; i++)
            if (cityNames[i] == name) return i;
        return -1;
    }

    bool addCity(const string& name) {
        if (numCities >= MAX_CITIES) return false;
        if (findCity(name) != -1)   return false;
        cityNames[numCities++] = name;
        return true;
    }

    bool removeCity(const string& name) {
        int idx = findCity(name);
        if (idx == -1) return false;
        // Remove all edges to/from this city
        for (int i = 0; i < numCities; i++) {
            adj[idx][i].exists = false;
            adj[i][idx].exists = false;
        }
        // Shift cities left
        for (int i = idx; i < numCities - 1; i++) {
            cityNames[i] = cityNames[i + 1];
            for (int j = 0; j < numCities; j++) {
                adj[i][j] = adj[i + 1][j];
                adj[j][i] = adj[j][i + 1];
            }
        }
        numCities--;
        return true;
    }

    bool addTrack(const string& from, const string& to, int distKm, int timeMin, bool bidirectional = true) {
        int a = findCity(from), b = findCity(to);
        if (a == -1 || b == -1) return false;
        adj[a][b].distanceKm = distKm;
        adj[a][b].travelTimeMin = timeMin;
        adj[a][b].exists = true;
        if (bidirectional) {
            adj[b][a].distanceKm = distKm;
            adj[b][a].travelTimeMin = timeMin;
            adj[b][a].exists = true;
        }
        return true;
    }

    bool removeTrack(const string& from, const string& to, bool bidirectional = true) {
        int a = findCity(from), b = findCity(to);
        if (a == -1 || b == -1) return false;
        adj[a][b].exists = false;
        if (bidirectional) adj[b][a].exists = false;
        return true;
    }

    // Dijkstra — mode: 0=distance, 1=time
    // Returns path as array of city indices, pathLen = number of nodes
    bool shortestPath(const string& from, const string& to, int mode,
        int outPath[], int& pathLen, int& totalCost) const {
        int src = findCity(from), dst = findCity(to);
        if (src == -1 || dst == -1) return false;

        int  dist[MAX_CITIES];
        bool visited[MAX_CITIES];
        int  prev[MAX_CITIES];
        for (int i = 0; i < numCities; i++) {
            dist[i] = INF_DIST;
            visited[i] = false;
            prev[i] = -1;
        }
        dist[src] = 0;

        for (int iter = 0; iter < numCities; iter++) {
            // Find unvisited node with minimum distance
            int u = -1;
            for (int i = 0; i < numCities; i++)
                if (!visited[i] && (u == -1 || dist[i] < dist[u]))
                    u = i;
            if (u == -1 || dist[u] == INF_DIST) break;
            visited[u] = true;

            for (int v = 0; v < numCities; v++) {
                if (!adj[u][v].exists) continue;
                int w = (mode == 0) ? adj[u][v].distanceKm : adj[u][v].travelTimeMin;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    prev[v] = u;
                }
            }
        }

        if (dist[dst] == INF_DIST) return false;

        // Reconstruct path
        int tmp[MAX_CITIES];
        int len = 0;
        for (int at = dst; at != -1; at = prev[at])
            tmp[len++] = at;
        // Reverse
        for (int i = 0; i < len; i++)
            outPath[i] = tmp[len - 1 - i];
        pathLen = len;
        totalCost = dist[dst];
        return true;
    }

    void displayMatrix() const {
        // Print header
        cout << "\n  Network Adjacency (km / min)\n";
        cout << "  [";
        for (int i = 0; i < numCities; i++) {
            cout << cityNames[i].substr(0, 6);
            if (i < numCities - 1) cout << "|";
        }
        cout << "]\n";
        for (int i = 0; i < numCities; i++) {
            cout << "  " << cityNames[i].substr(0, 6) << " | ";
            for (int j = 0; j < numCities; j++) {
                if (adj[i][j].exists)
                    cout << adj[i][j].distanceKm << "/" << adj[i][j].travelTimeMin << "  ";
                else
                    cout << "--/--  ";
            }
            cout << "\n";
        }
    }
};
