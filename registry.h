#pragma once
// ============================================================
//  modules/registry.h  —  Module 1: Train Registry (AVL Tree)
// ============================================================
#include "../Data_Strucutres_PROJECT/dataSt.h"
#include "../Data_Strucutres_PROJECT/train.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::setw;
using std::left;
using std::endl;

class TrainRegistry {
public:
    AVLTree<Train, int> avl;

    // ── Register a new train ──────────────────────────────
    bool registerTrain(const Train& t) {
        if (avl.search(t.trainID)) {
            cout << "  [!] Train ID " << t.trainID << " already registered.\n";
            return false;
        }
        avl.insert(t, t.trainID);
        cout << "  [+] Train \"" << t.trainName << "\" (ID " << t.trainID << ") registered.\n";
        return true;
    }

    // ── Decommission (remove) a train ─────────────────────
    bool decommissionTrain(int id) {
        if (!avl.search(id)) {
            cout << "  [!] Train ID " << id << " not found.\n";
            return false;
        }
        avl.remove(id);
        cout << "  [-] Train ID " << id << " decommissioned.\n";
        return true;
    }

    // ── Find and return a train ───────────────────────────
    Train* findTrain(int id) {
        auto* node = avl.search(id);
        return node ? &node->data : nullptr;
    }

    // ── Update train data ─────────────────────────────────
    bool updateTrain(const Train& t) {
        auto* node = avl.search(t.trainID);
        if (!node) return false;
        node->data = t;
        return true;
    }

    // ── Traversal displays ────────────────────────────────
    static void printTrain(const Train& t) {
        cout << "  | " << setw(5) << left << t.trainID
            << " | " << setw(20) << left << t.trainName
            << " | " << setw(12) << left << t.source
            << " -> " << setw(12) << left << t.destination
            << " | Dep: " << t.departureTime
            << " | Arr: " << t.arrivalTime
            << " | " << t.status << " |\n";
    };

    void displayInOrder() const {
        if (avl.isEmpty()) { cout << "  [i] No trains registered.\n"; return; }
        cout << "\n  ═══ Train Registry — In-Order (by ID) ═══\n";
        avl.inOrder(printTrain);
        cout << "  Tree height: " << avl.treeHeight() << "\n";
    }

    void displayPreOrder() const {
        if (avl.isEmpty()) { cout << "  [i] No trains registered.\n"; return; }
        cout << "\n  ═══ Train Registry — Pre-Order ═══\n";
        avl.preOrder(printTrain);
    }

    void displayPostOrder() const {
        if (avl.isEmpty()) { cout << "  [i] No trains registered.\n"; return; }
        cout << "\n  ═══ Train Registry — Post-Order ═══\n";
        avl.postOrder(printTrain);
    }

    bool isEmpty() const { return avl.isEmpty(); }
};
