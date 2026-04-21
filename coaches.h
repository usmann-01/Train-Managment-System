#pragma once
// ============================================================
//  modules/coaches.h  —  Module 2: Coach Management (DLL)
// ============================================================
#include "../Data_Strucutres_PROJECT/dataSt.h"
#include "../Data_Strucutres_PROJECT/coaches.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::setw;
using std::left;
using std::endl;
using std::string;

class CoachManager {
public:
    HashMap<int, DoublyLinkedList<Coach>*> trainCoaches;
    int nextCoachID;

    CoachManager() : trainCoaches(211), nextCoachID(1001) {}

    ~CoachManager() {
        trainCoaches.forEach([](int, DoublyLinkedList<Coach>* dll) {
            delete dll;
            });
    }

    // ── Train Initialization ──────────────────────────────

    // Always call this when a train is registered or loaded
    void initTrain(int trainID) {
        if (!trainCoaches.contains(trainID)) {
            trainCoaches.insert(trainID, new DoublyLinkedList<Coach>());
            cout << "  [i] Coach list initialized for train " << trainID << ".\n";
        }
    }

    void removeTrain(int trainID) {
        DoublyLinkedList<Coach>* dll = nullptr;
        if (trainCoaches.get(trainID, dll) && dll) {
            delete dll;
            trainCoaches.remove(trainID);
            cout << "  [-] Coach list removed for train " << trainID << ".\n";
        }
    }

    // ── Safe Getters ──────────────────────────────────────

    DoublyLinkedList<Coach>* getList(int trainID) {
        DoublyLinkedList<Coach>* dll = nullptr;
        if (!trainCoaches.get(trainID, dll) || !dll) {
            cout << "  [!] Train " << trainID << " not found in CoachManager.\n";
            cout << "  [?] Did you forget to call initTrain(" << trainID << ")?\n";
            return nullptr;
        }
        return dll;
    }

    bool trainExists(int trainID) const {
        DoublyLinkedList<Coach>* dll = nullptr;
        return trainCoaches.get(trainID, dll) && dll != nullptr;
    }

    // ── Attach Coaches ────────────────────────────────────

    int attachFront(int trainID, const string& type, int seats) {
        // Auto-init if train is missing (safety net)
        if (!trainExists(trainID)) {
            cout << "  [~] Auto-initializing coach list for train " << trainID << ".\n";
            initTrain(trainID);
        }
        auto* dll = getList(trainID);
        if (!dll) return -1;

        if (!isValidType(type)) {
            cout << "  [!] Invalid coach type '" << type << "'. Use: Economy, Business, Sleeper, First.\n";
            return -1;
        }
        if (seats <= 0) {
            cout << "  [!] Seat count must be greater than 0.\n";
            return -1;
        }

        Coach c(nextCoachID++, type, seats, 1);
        dll->insertFront(c);
        resequence(dll);
        cout << "  [+] Coach " << c.coachID << " (" << type << ", "
            << seats << " seats) attached at front of train " << trainID << ".\n";
        return c.coachID;
    }

    int attachRear(int trainID, const string& type, int seats) {
        if (!trainExists(trainID)) {
            cout << "  [~] Auto-initializing coach list for train " << trainID << ".\n";
            initTrain(trainID);
        }
        auto* dll = getList(trainID);
        if (!dll) return -1;

        if (!isValidType(type)) {
            cout << "  [!] Invalid coach type '" << type << "'. Use: Economy, Business, Sleeper, First.\n";
            return -1;
        }
        if (seats <= 0) {
            cout << "  [!] Seat count must be greater than 0.\n";
            return -1;
        }

        Coach c(nextCoachID++, type, seats, dll->size + 1);
        dll->insertBack(c);
        cout << "  [+] Coach " << c.coachID << " (" << type << ", "
            << seats << " seats) attached at rear of train " << trainID << ".\n";
        return c.coachID;
    }

    int attachAt(int trainID, int pos, const string& type, int seats) {
        if (!trainExists(trainID)) {
            cout << "  [~] Auto-initializing coach list for train " << trainID << ".\n";
            initTrain(trainID);
        }
        auto* dll = getList(trainID);
        if (!dll) return -1;

        if (!isValidType(type)) {
            cout << "  [!] Invalid coach type '" << type << "'. Use: Economy, Business, Sleeper, First.\n";
            return -1;
        }
        if (seats <= 0) {
            cout << "  [!] Seat count must be greater than 0.\n";
            return -1;
        }
        if (pos < 1 || pos > dll->size + 1) {
            cout << "  [!] Invalid position " << pos
                << ". Valid range: 1 to " << dll->size + 1 << ".\n";
            return -1;
        }

        Coach c(nextCoachID++, type, seats, pos);
        dll->insertAt(pos, c);
        resequence(dll);
        cout << "  [+] Coach " << c.coachID << " (" << type << ", "
            << seats << " seats) inserted at position " << pos
            << " of train " << trainID << ".\n";
        return c.coachID;
    }

    // ── Detach Coaches ────────────────────────────────────

    bool detachAt(int trainID, int pos) {
        auto* dll = getList(trainID);
        if (!dll) return false;
        if (dll->isEmpty()) { cout << "  [!] No coaches to detach.\n"; return false; }
        if (pos < 1 || pos > dll->size) {
            cout << "  [!] Invalid position " << pos
                << ". Valid range: 1 to " << dll->size << ".\n";
            return false;
        }
        if (!dll->deleteAt(pos)) { cout << "  [!] Detach failed.\n"; return false; }
        resequence(dll);
        cout << "  [-] Coach at position " << pos
            << " detached from train " << trainID << ".\n";
        return true;
    }

    bool detachByID(int trainID, int coachID) {
        auto* dll = getList(trainID);
        if (!dll) return false;
        if (dll->isEmpty()) { cout << "  [!] No coaches to detach.\n"; return false; }

        int pos = 1;
        auto* cur = dll->head;
        while (cur) {
            if (cur->data.coachID == coachID) {
                dll->deleteAt(pos);
                resequence(dll);
                cout << "  [-] Coach " << coachID
                    << " detached from train " << trainID << ".\n";
                return true;
            }
            cur = cur->next;
            pos++;
        }
        cout << "  [!] Coach ID " << coachID
            << " not found on train " << trainID << ".\n";
        return false;
    }

    // ── Search ────────────────────────────────────────────

    // Find a coach by ID, returns nullptr if not found
    Coach* findCoachByID(int trainID, int coachID) {
        auto* dll = getList(trainID);
        if (!dll) return nullptr;
        auto* cur = dll->head;
        while (cur) {
            if (cur->data.coachID == coachID) return &cur->data;
            cur = cur->next;
        }
        cout << "  [!] Coach ID " << coachID << " not found.\n";
        return nullptr;
    }

    // Find all coaches of a given type on a train
    void findByType(int trainID, const string& type) const {
        DoublyLinkedList<Coach>* dll = nullptr;
        trainCoaches.get(trainID, dll);
        if (!dll || dll->isEmpty()) { cout << "  [i] No coaches found.\n"; return; }

        cout << "\n  ══ Coaches of type '" << type << "' on train " << trainID << " ══\n";
        bool found = false;
        dll->traverseForward([&](const Coach& c) {
            if (c.coachType == type) {
                printCoachRow(c);
                found = true;
            }
            });
        if (!found) cout << "  [i] No coaches of type '" << type << "' found.\n";
    }

    // ── Traversal & Display ───────────────────────────────

    void traverseForward(int trainID) const {
        DoublyLinkedList<Coach>* dll = nullptr;
        trainCoaches.get(trainID, dll);
        if (!dll || dll->isEmpty()) { cout << "  [i] No coaches attached to train " << trainID << ".\n"; return; }

        cout << "\n  ══ Train " << trainID << " — Coaches (Front → Rear) ══\n";
        printHeader();
        dll->traverseForward([&](const Coach& c) { printCoachRow(c); });
        printFooter(dll->size);
    }

    void traverseBackward(int trainID) const {
        DoublyLinkedList<Coach>* dll = nullptr;
        trainCoaches.get(trainID, dll);
        if (!dll || dll->isEmpty()) { cout << "  [i] No coaches attached to train " << trainID << ".\n"; return; }

        cout << "\n  ══ Train " << trainID << " — Coaches (Rear → Front) ══\n";
        printHeader();
        dll->traverseBackward([&](const Coach& c) { printCoachRow(c); });
        printFooter(dll->size);
    }

    void reverseCoaches(int trainID) {
        auto* dll = getList(trainID);
        if (!dll) return;
        if (dll->isEmpty()) { cout << "  [i] No coaches to reverse.\n"; return; }
        dll->reverse();
        resequence(dll);
        cout << "  [~] Coach order reversed for train "
            << trainID << " (return journey mode).\n";
    }

    // ── Stats ─────────────────────────────────────────────

    int coachCount(int trainID) {
        auto* dll = getList(trainID);
        return dll ? dll->size : 0;
    }

    int totalSeats(int trainID) {
        auto* dll = getList(trainID);
        if (!dll) return 0;
        int total = 0;
        dll->traverseForward([&](const Coach& c) { total += c.totalSeats; });
        return total;
    }

    void printSummary(int trainID) {
        auto* dll = getList(trainID);
        if (!dll) return;
        cout << "\n  ══ Coach Summary — Train " << trainID << " ══\n";
        cout << "  Total Coaches : " << dll->size << "\n";
        cout << "  Total Seats   : " << totalSeats(trainID) << "\n";

        // Count by type
        int economy = 0, business = 0, sleeper = 0, first = 0;
        dll->traverseForward([&](const Coach& c) {
            if (c.coachType == "Economy")  economy++;
            else if (c.coachType == "Business") business++;
            else if (c.coachType == "Sleeper")  sleeper++;
            else if (c.coachType == "First")    first++;
            });
        if (economy)  cout << "  Economy       : " << economy << " coach(es)\n";
        if (business) cout << "  Business      : " << business << " coach(es)\n";
        if (sleeper)  cout << "  Sleeper       : " << sleeper << " coach(es)\n";
        if (first)    cout << "  First Class   : " << first << " coach(es)\n";
    }

private:
    // ── Helpers ───────────────────────────────────────────

    void resequence(DoublyLinkedList<Coach>* dll) {
        int pos = 1;
        auto* cur = dll->head;
        while (cur) {
            cur->data.coachPosition = pos++;
            cur = cur->next;
        }
    }

    bool isValidType(const string& type) const {
        return type == "Economy" || type == "Business" ||
            type == "Sleeper" || type == "First";
    }

    void printHeader() const {
        cout << "  " << std::string(52, '-') << "\n";
        cout << "  " << left << setw(6) << "Pos"
            << setw(10) << "CoachID"
            << setw(12) << "Type"
            << setw(8) << "Seats" << "\n";
        cout << "  " << std::string(52, '-') << "\n";
    }

    void printCoachRow(const Coach& c) const {
        cout << "  " << left << setw(6) << c.coachPosition
            << setw(10) << c.coachID
            << setw(12) << c.coachType
            << setw(8) << c.totalSeats << "\n";
    }

    void printFooter(int count) const {
        cout << "  " << std::string(52, '-') << "\n";
        cout << "  Total: " << count << " coach(es)\n";
    }
};