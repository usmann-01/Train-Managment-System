#pragma once
// ============================================================
//  modules/seating.h  —  Module 4: Dual-Layer Seat Management
//  Layer 1: HashMap  (O(1) lookup)
//  Layer 2: BST      (sorted display via in-order traversal)
//  Both layers stay in sync on every booking/cancellation.
// ============================================================
#include "../Data_Strucutres_PROJECT/dataSt.h"
#include "../Data_Strucutres_PROJECT/train.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::setw;
using std::left;
using std::endl;

class SeatManager {
public:
    // Per-train seat store: one HashMap + one BST per trainID
    struct SeatStore {
        HashMap<int, Seat>* hashMap; // seatNumber -> Seat
        BST<Seat, int>* bst;     // seatNumber key
        SeatStore() : hashMap(nullptr), bst(nullptr) {}
    };

    HashMap<int, SeatStore> stores; // trainID -> SeatStore

    SeatManager() : stores(211) {}

    ~SeatManager() {
        stores.forEach([](int, SeatStore& s) {
            delete s.hashMap;
            delete s.bst;
            });
    }

    // Initialise seat store for a train (coach totalSeats must be known)
    void initTrain(int trainID, int totalSeats) {
        SeatStore s;
        // Use a prime ≥ 2× totalSeats for low load factor
        int cap = totalSeats * 3 + 7;
        s.hashMap = new HashMap<int, Seat>(cap);
        s.bst = new BST<Seat, int>();
        // Pre-populate all seats as unbooked
        for (int i = 1; i <= totalSeats; i++) {
            Seat seat(i, -1);
            s.hashMap->insert(i, seat);
            s.bst->insert(seat, i);
        }
        stores.insert(trainID, s);
        cout << "  [+] Seat chart initialised for train " << trainID
            << " (" << totalSeats << " seats).\n";
    }

    // Add seats for a newly attached coach
    void addCoachSeats(int trainID, int coachID, int startSeat, int count) {
        SeatStore* sp = stores.getRef(trainID);
        if (!sp) {
            cout << "  [!] No seat store for train " << trainID << ".\n";
            return;
        }
        for (int i = startSeat; i < startSeat + count; i++) {
            Seat seat(i, coachID);
            sp->hashMap->insert(i, seat);
            sp->bst->insert(seat, i);
        }
    }

    // ── Book a seat ───────────────────────────────────────
    bool bookSeat(int trainID, int seatNum, const string& name, const string& pid) {
        SeatStore* sp = stores.getRef(trainID);
        if (!sp) { cout << "  [!] Train " << trainID << " not found.\n"; return false; }

        Seat* s = sp->hashMap->getRef(seatNum);
        if (!s) { cout << "  [!] Seat " << seatNum << " does not exist.\n"; return false; }
        if (s->isBooked) {
            cout << "  [!] Seat " << seatNum << " is already booked by "
                << s->passengerName << ".\n";
            return false;
        }
        s->isBooked = true;
        s->passengerName = name;
        s->passengerID = pid;
        // Sync BST
        sp->bst->remove(seatNum);
        sp->bst->insert(*s, seatNum);
        cout << "  [+] Seat " << seatNum << " booked for " << name << ".\n";
        return true;
    }

    // ── Cancel a booking ──────────────────────────────────
    bool cancelSeat(int trainID, int seatNum) {
        SeatStore* sp = stores.getRef(trainID);
        if (!sp) { cout << "  [!] Train " << trainID << " not found.\n"; return false; }

        Seat* s = sp->hashMap->getRef(seatNum);
        if (!s) { cout << "  [!] Seat " << seatNum << " does not exist.\n"; return false; }
        if (!s->isBooked) { cout << "  [i] Seat " << seatNum << " is already free.\n"; return false; }
        string who = s->passengerName;
        s->isBooked = false;
        s->passengerName = "";
        s->passengerID = "";
        // Sync BST
        sp->bst->remove(seatNum);
        sp->bst->insert(*s, seatNum);
        cout << "  [-] Seat " << seatNum << " cancellation confirmed (was: " << who << ").\n";
        return true;
    }

    // ── Check a seat ──────────────────────────────────────
    void checkSeat(int trainID, int seatNum) const {
        const SeatStore* sp = stores.getRef(trainID);
        if (!sp) { cout << "  [!] Train " << trainID << " not found.\n"; return; }
        Seat out;
        if (!sp->hashMap->get(seatNum, out)) {
            cout << "  [!] Seat " << seatNum << " does not exist.\n";
            return;
        }
        cout << "\n  ── Seat " << seatNum << " ──\n";
        cout << "  Status  : " << (out.isBooked ? "BOOKED" : "Available") << "\n";
        if (out.isBooked) {
            cout << "  Passenger: " << out.passengerName
                << "  (ID: " << out.passengerID << ")\n";
        }
    }

    // ── Display all seats (sorted via BST in-order) ───────
    void displayAllSeats(int trainID) const {
        const SeatStore* sp = stores.getRef(trainID);
        if (!sp) { cout << "  [!] Train " << trainID << " not found.\n"; return; }
        cout << "\n  ══ Seat Chart (Sorted) ══\n";
        cout << "  " << setw(6) << left << "Seat"
            << setw(12) << left << "Status"
            << "Passenger\n";
        cout << "  " << string(40, '-') << "\n";

        int booked = 0, free = 0;
        sp->bst->inOrder([&](const Seat& s) {
            cout << "  " << setw(6) << left << s.seatNumber
                << setw(12) << left << (s.isBooked ? "BOOKED" : "Available");
            if (s.isBooked) cout << s.passengerName;
            cout << "\n";
            if (s.isBooked) booked++; else free++;
            });
        cout << "  Booked: " << booked << "  |  Free: " << free << "\n";
    }

    void removeTrain(int trainID) {
        SeatStore* sp = stores.getRef(trainID);
        if (sp) {
            delete sp->hashMap;
            delete sp->bst;
            stores.remove(trainID);
        }
    }
};
