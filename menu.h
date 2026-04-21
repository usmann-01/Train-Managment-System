#pragma once
// ============================================================
//  menu.h  —  CLI Menu System for The Iron Nexus
// ============================================================
#include "../Data_Strucutres_PROJECT/registry.h"
#include "../Data_Strucutres_PROJECT/coaches.h"
#include "../Data_Strucutres_PROJECT/network.h"
#include "../Data_Strucutres_PROJECT/seating.h"
#include "../Data_Strucutres_PROJECT/logging.h"
#include "../Data_Strucutres_PROJECT/storage.h"
#include "undo.h"

#include <iostream>
#include <string>
#include <limits>
#include <ctime>

using std::cin;
using std::cout;
using std::endl;
using std::string;

// ── Utility helpers ───────────────────────────────────────
static int readInt(const string& prompt, int lo = INT_MIN, int hi = INT_MAX) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= lo && val <= hi) {
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Try again.\n";
    }
}

static string readStr(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

static string currentDate() {
    time_t now = time(nullptr);
    char buf[32];
    struct tm t {};
    localtime_s(&t, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &t);
    return string(buf);
}

static void banner() {
    cout << "\n";
    cout << "  ================================================\n";
    cout << "  |         T H E   I R O N   N E X U S          |\n";
    cout << "  |       Railway Management System v1.0         |\n";
    cout << "  ================================================\n\n";
}

static void pause() {
    cout << "\n  Press ENTER to continue...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ── Forward declarations ──────────────────────────────────
void menuRegistry(TrainRegistry&, CoachManager&, SeatManager&, OperationLogger&, UndoManager&);
void menuCoaches(TrainRegistry&, CoachManager&, SeatManager&, OperationLogger&, UndoManager&);
void menuNetwork(RailwayNetwork&, OperationLogger&, UndoManager&);
void menuSeating(TrainRegistry&, SeatManager&, OperationLogger&, UndoManager&);
void menuLogging(OperationLogger&);
void menuStorage(TrainRegistry&, CoachManager&, RailwayNetwork&, SeatManager&, OperationLogger&);
void menuUndoRedo(TrainRegistry&, CoachManager&, SeatManager&, RailwayNetwork&,
    OperationLogger&, UndoManager&);

// ═══════════════════════════════════════════════════════════
//  MAIN MENU
// ═══════════════════════════════════════════════════════════
void mainMenu(TrainRegistry& reg, CoachManager& cm, RailwayNetwork& net,
    SeatManager& sm, OperationLogger& logger, UndoManager& undo,
    StorageManager& storage) {
    (void)storage; // available for auto-save extension
    while (true) {
        banner();
        cout << "  [1] Train Registry\n";
        cout << "  [2] Coach Management\n";
        cout << "  [3] Railway Network\n";
        cout << "  [4] Seat Booking\n";
        cout << "  [5] Operation Logs\n";
        cout << "  [6] Save / Load\n";
        cout << "  [7] Undo / Redo  (Undo:" << undo.undoDepth()
            << "  Redo:" << undo.redoDepth() << ")\n";
        cout << "  [0] Exit\n";
        int choice = readInt("  > ", 0, 7);
        switch (choice) {
        case 1: menuRegistry(reg, cm, sm, logger, undo); break;
        case 2: menuCoaches(reg, cm, sm, logger, undo);  break;
        case 3: menuNetwork(net, logger, undo);           break;
        case 4: menuSeating(reg, sm, logger, undo);       break;
        case 5: menuLogging(logger);                      break;
        case 6: menuStorage(reg, cm, net, sm, logger);   break;
        case 7: menuUndoRedo(reg, cm, sm, net, logger, undo); break;
        case 0:
            cout << "\n  Goodbye. The Iron Nexus signing off.\n\n";
            return;
        }
    }
}

// ═══════════════════════════════════════════════════════════
//  MODULE 1 — Train Registry
// ═══════════════════════════════════════════════════════════
void menuRegistry(TrainRegistry& reg, CoachManager& cm, SeatManager& sm,
    OperationLogger& logger, UndoManager& undo) {
    while (true) {
        cout << "\n  ── Train Registry ──\n";
        cout << "  [1] Register New Train\n";
        cout << "  [2] Decommission Train\n";
        cout << "  [3] Find Train\n";
        cout << "  [4] Display All (In-Order)\n";
        cout << "  [5] Display All (Pre-Order)\n";
        cout << "  [6] Display All (Post-Order)\n";
        cout << "  [0] Back\n";
        int c = readInt("  > ", 0, 6);
        if (c == 0) return;

        if (c == 1) {
            Train t;
            t.trainID = readInt("  Train ID         : ");
            if (reg.findTrain(t.trainID)) {
                cout << "  [!] ID already exists.\n"; pause(); continue;
            }
            t.trainName = readStr("  Train Name        : ");
            t.source = readStr("  Source Station    : ");
            t.destination = readStr("  Destination       : ");
            t.departureTime = readStr("  Departure (HH:MM) : ");
            t.arrivalTime = readStr("  Arrival   (HH:MM) : ");
            t.status = "Active";
            t.totalCoaches = 0;

            undo.recordTrain(UndoOp::REGISTER_TRAIN, t);
            reg.registerTrain(t);
            cm.initTrain(t.trainID);
            logger.logAction("REGISTER_TRAIN",
                "Registered '" + t.trainName + "'", currentDate(), t.trainID);
        }
        else if (c == 2) {
            int id = readInt("  Train ID to decommission: ");
            Train* tp = reg.findTrain(id);
            if (!tp) { cout << "  [!] Not found.\n"; pause(); continue; }
            undo.recordTrain(UndoOp::DECOMMISSION_TRAIN, *tp);
            reg.decommissionTrain(id);
            cm.removeTrain(id);
            sm.removeTrain(id);
            logger.logAction("DECOMMISSION_TRAIN",
                "Decommissioned train ID " + std::to_string(id),
                currentDate(), id);
        }
        else if (c == 3) {
            int id = readInt("  Train ID to find: ");
            Train* tp = reg.findTrain(id);
            if (!tp) cout << "  [!] Not found.\n";
            else {
                cout << "\n  ── Train Record ──\n";
                cout << "  ID    : " << tp->trainID << "\n";
                cout << "  Name  : " << tp->trainName << "\n";
                cout << "  Route : " << tp->source << " -> " << tp->destination << "\n";
                cout << "  Dep   : " << tp->departureTime << "\n";
                cout << "  Arr   : " << tp->arrivalTime << "\n";
                cout << "  Status: " << tp->status << "\n";
                cout << "  Coaches: " << tp->totalCoaches << "\n";
            }
        }
        else if (c == 4) reg.displayInOrder();
        else if (c == 5) reg.displayPreOrder();
        else if (c == 6) reg.displayPostOrder();
        pause();
    }
}

// ═══════════════════════════════════════════════════════════
//  MODULE 2 — Coach Management
// ═══════════════════════════════════════════════════════════
void menuCoaches(TrainRegistry& reg, CoachManager& cm, SeatManager& sm,
    OperationLogger& logger, UndoManager& undo) {
    while (true) {
        cout << "\n  ── Coach Management ──\n";
        cout << "  [1] Attach Coach (Front)\n";
        cout << "  [2] Attach Coach (Rear)\n";
        cout << "  [3] Attach Coach (Position)\n";
        cout << "  [4] Detach Coach by Position\n";
        cout << "  [5] Detach Coach by ID\n";
        cout << "  [6] View Coaches (Front -> Rear)\n";
        cout << "  [7] View Coaches (Rear -> Front)\n";
        cout << "  [8] Reverse Coach Order\n";
        cout << "  [0] Back\n";
        int c = readInt("  > ", 0, 8);
        if (c == 0) return;

        int trainID = readInt("  Train ID: ");
        if (!reg.findTrain(trainID)) {
            cout << "  [!] Train not found.\n"; pause(); continue;
        }

        if (c == 1 || c == 2 || c == 3) {
            string type = readStr("  Coach Type (Economy/Business/VIP/Sleeper): ");
            int seats = readInt("  Number of Seats: ", 1, 200);
            Coach snap;
            snap.coachType = type;
            snap.totalSeats = seats;
            int id = -1;
            if (c == 1) id = cm.attachFront(trainID, type, seats);
            else if (c == 2) id = cm.attachRear(trainID, type, seats);
            else {
                int pos = readInt("  Position: ", 1);
                id = cm.attachAt(trainID, pos, type, seats);
            }
            if (id != -1) {
                snap.coachID = id;
                undo.recordCoach(UndoOp::ATTACH_COACH, trainID, snap);
                Train* tp = reg.findTrain(trainID);
                if (tp) tp->totalCoaches++;
                // Allocate seats starting from existing total+1
                SeatManager::SeatStore* sp = sm.stores.getRef(trainID);
                int startSeat = sp ? sp->hashMap->getCount() + 1 : 1;
                sm.addCoachSeats(trainID, id, startSeat, seats);
                if (tp) tp->totalCoaches = cm.coachCount(trainID);
                logger.logAction("ATTACH_COACH",
                    "Coach " + std::to_string(id) + " (" + type + ") attached to train " + std::to_string(trainID),
                    currentDate(), trainID);
            }
        }
        else if (c == 4) {
            auto* dll = cm.getList(trainID);
            if (!dll || dll->isEmpty()) { cout << "  [!] No coaches.\n"; pause(); continue; }
            int pos = readInt("  Position to detach: ", 1, dll->size);
            auto* node = dll->getAt(pos);
            if (node) {
                undo.recordCoach(UndoOp::DETACH_COACH, trainID, node->data);
            }
            cm.detachAt(trainID, pos);
            Train* tp = reg.findTrain(trainID);
            if (tp) tp->totalCoaches = cm.coachCount(trainID);
            logger.logAction("DETACH_COACH",
                "Coach at position " + std::to_string(pos) + " detached from train " + std::to_string(trainID),
                currentDate(), trainID);
        }
        else if (c == 5) {
            int cid = readInt("  Coach ID to detach: ");
            auto* dll = cm.getList(trainID);
            Coach snap;
            if (dll) {
                auto* cur = dll->head;
                while (cur) {
                    if (cur->data.coachID == cid) { snap = cur->data; break; }
                    cur = cur->next;
                }
            }
            undo.recordCoach(UndoOp::DETACH_COACH, trainID, snap);
            cm.detachByID(trainID, cid);
            Train* tp = reg.findTrain(trainID);
            if (tp) tp->totalCoaches = cm.coachCount(trainID);
            logger.logAction("DETACH_COACH",
                "Coach " + std::to_string(cid) + " detached from train " + std::to_string(trainID),
                currentDate(), trainID);
        }
        else if (c == 6) cm.traverseForward(trainID);
        else if (c == 7) cm.traverseBackward(trainID);
        else if (c == 8) {
            cm.reverseCoaches(trainID);
            logger.logAction("REVERSE_COACHES",
                "Coaches reversed for train " + std::to_string(trainID),
                currentDate(), trainID);
        }
        pause();
    }
}

// ═══════════════════════════════════════════════════════════
//  MODULE 3 — Railway Network
// ═══════════════════════════════════════════════════════════
void menuNetwork(RailwayNetwork& net, OperationLogger& logger, UndoManager& undo) {
    while (true) {
        cout << "\n  ── Railway Network ──\n";
        cout << "  [1] List Stations\n";
        cout << "  [2] Add Station\n";
        cout << "  [3] Remove Station\n";
        cout << "  [4] Add Track\n";
        cout << "  [5] Remove Track\n";
        cout << "  [6] Find Shortest Path (by Distance)\n";
        cout << "  [7] Find Shortest Path (by Time)\n";
        cout << "  [8] Display Network Matrix\n";
        cout << "  [0] Back\n";
        int c = readInt("  > ", 0, 8);
        if (c == 0) return;

        if (c == 1) net.listStations();
        else if (c == 2) {
            string name = readStr("  Station name: ");
            net.addStation(name);
            logger.logAction("ADD_STATION", "Added station " + name, currentDate());
        }
        else if (c == 3) {
            string name = readStr("  Station name: ");
            net.removeStation(name);
            logger.logAction("REMOVE_STATION", "Removed station " + name, currentDate());
        }
        else if (c == 4) {
            string f = readStr("  From: ");
            string t = readStr("  To  : ");
            int km = readInt("  Distance (km): ", 1);
            int mn = readInt("  Travel time (min): ", 1);
            undo.recordTrack(UndoOp::ADD_TRACK, f, t, km, mn);
            net.addTrack(f, t, km, mn);
            logger.logAction("ADD_TRACK",
                f + " <-> " + t + " [" + std::to_string(km) + "km]", currentDate());
        }
        else if (c == 5) {
            string f = readStr("  From: ");
            string t = readStr("  To  : ");
            // Get existing edge data for undo
            int a = net.graph.findCity(f), b = net.graph.findCity(t);
            if (a != -1 && b != -1 && net.graph.adj[a][b].exists) {
                undo.recordTrack(UndoOp::REMOVE_TRACK, f, t,
                    net.graph.adj[a][b].distanceKm,
                    net.graph.adj[a][b].travelTimeMin);
            }
            net.removeTrack(f, t);
            logger.logAction("REMOVE_TRACK", f + " -- " + t, currentDate());
        }
        else if (c == 6) {
            string f = readStr("  From: ");
            string t = readStr("  To  : ");
            net.findShortestPath(f, t, 0);
        }
        else if (c == 7) {
            string f = readStr("  From: ");
            string t = readStr("  To  : ");
            net.findShortestPath(f, t, 1);
        }
        else if (c == 8) net.displayNetwork();
        pause();
    }
}

// ═══════════════════════════════════════════════════════════
//  MODULE 4 — Seat Booking
// ═══════════════════════════════════════════════════════════
void menuSeating(TrainRegistry& reg, SeatManager& sm,
    OperationLogger& logger, UndoManager& undo) {
    while (true) {
        cout << "\n  ── Seat Booking ──\n";
        cout << "  [1] Initialise Seat Chart for Train\n";
        cout << "  [2] Book a Seat\n";
        cout << "  [3] Cancel a Booking\n";
        cout << "  [4] Check a Seat\n";
        cout << "  [5] Display All Seats (Sorted)\n";
        cout << "  [0] Back\n";
        int c = readInt("  > ", 0, 5);
        if (c == 0) return;

        int trainID = readInt("  Train ID: ");

        if (c == 1) {
            if (!reg.findTrain(trainID)) { cout << "  [!] Train not found.\n"; pause(); continue; }
            int total = readInt("  Total seats to initialise: ", 1, 5000);
            sm.initTrain(trainID, total);
        }
        else if (c == 2) {
            int seat = readInt("  Seat number: ", 1);
            // Save state for undo
            SeatManager::SeatStore* sp = sm.stores.getRef(trainID);
            if (sp) {
                Seat* s = sp->hashMap->getRef(seat);
                if (s) undo.recordSeat(UndoOp::BOOK_SEAT, trainID, *s);
            }
            string pname = readStr("  Passenger name: ");
            string pid = readStr("  Passenger ID (CNIC): ");
            if (sm.bookSeat(trainID, seat, pname, pid))
                logger.logAction("BOOK_SEAT",
                    "Seat " + std::to_string(seat) + " booked by " + pname,
                    currentDate(), trainID);
        }
        else if (c == 3) {
            int seat = readInt("  Seat number: ", 1);
            SeatManager::SeatStore* sp = sm.stores.getRef(trainID);
            if (sp) {
                Seat* s = sp->hashMap->getRef(seat);
                if (s) undo.recordSeat(UndoOp::CANCEL_SEAT, trainID, *s);
            }
            if (sm.cancelSeat(trainID, seat))
                logger.logAction("CANCEL_SEAT",
                    "Seat " + std::to_string(seat) + " cancelled",
                    currentDate(), trainID);
        }
        else if (c == 4) sm.checkSeat(trainID, readInt("  Seat number: ", 1));
        else if (c == 5) sm.displayAllSeats(trainID);
        pause();
    }
}

// ═══════════════════════════════════════════════════════════
//  MODULE 5 — Operation Logs
// ═══════════════════════════════════════════════════════════
void menuLogging(OperationLogger& logger) {
    while (true) {
        cout << "\n  ── Operation Logs ── (" << logger.logCount() << " entries)\n";
        cout << "  [1] View Recent Logs\n";
        cout << "  [2] View Oldest Logs\n";
        cout << "  [3] Clear Logs\n";
        cout << "  [0] Back\n";
        int c = readInt("  > ", 0, 3);
        if (c == 0) return;
        if (c == 1) {
            int n = readInt("  How many entries? ", 1, 1000);
            logger.viewRecentLogs(n);
        }
        else if (c == 2) {
            int n = readInt("  How many entries? ", 1, 1000);
            logger.viewOldestLogs(n);
        }
        else if (c == 3) {
            logger.clearLogs();
            cout << "  [-] Logs cleared.\n";
        }
        pause();
    }
}

// ═══════════════════════════════════════════════════════════
//  MODULE 6 — Save / Load
// ═══════════════════════════════════════════════════════════
void menuStorage(TrainRegistry& reg, CoachManager& cm, RailwayNetwork& net,
    SeatManager& sm, OperationLogger& logger) {
    StorageManager storage;
    while (true) {
        cout << "\n  ── Save / Load ──\n";
        cout << "  [1] Save All\n";
        cout << "  [2] Load All\n";
        cout << "  [3] Save Registry Only\n";
        cout << "  [4] Load Registry Only\n";
        cout << "  [5] Save Network Only\n";
        cout << "  [6] Load Network Only\n";
        cout << "  [7] Save Coaches Only\n";
        cout << "  [8] Load Coaches Only\n";
        cout << "  [9] Save Seats Only\n";
        cout << "  [10] Load Seats Only\n";
        cout << "  [11] Save Logs Only\n";
        cout << "  [12] Load Logs Only\n";
        cout << "  [13] Save Registry Tree Structure (Bonus)\n";
        cout << "  [14] Save Seat BST Structure (Bonus)\n";
        cout << "  [0] Back\n";
        int c = readInt("  > ", 0, 14);
        if (c == 0) return;

        switch (c) {
        case 1:  storage.saveAll(reg, cm, net, sm, logger); break;
        case 2:  storage.loadAll(reg, cm, net, sm, logger); break;
        case 3:  storage.saveRegistry(reg); break;
        case 4:  storage.loadRegistry(reg); break;
        case 5:  storage.saveNetwork(net); break;
        case 6:  storage.loadNetwork(net); break;
        case 7:  storage.saveCoaches(cm); break;
        case 8:  storage.loadCoaches(cm); break;
        case 9:  storage.saveSeats(sm); break;
        case 10: storage.loadSeats(sm);  break;
        case 11: storage.saveLogs(logger); break;
        case 12: storage.loadLogs(logger); break;
        case 13: storage.saveRegistryTree(reg); break;
        case 14: {
            int tid = readInt("  Train ID for BST save: ");
            storage.saveSeatBST(sm, tid);
            break;
        }
        }
        pause();
    }
}

// ═══════════════════════════════════════════════════════════
//  UNDO / REDO
// ═══════════════════════════════════════════════════════════
void menuUndoRedo(TrainRegistry& reg, CoachManager& cm, SeatManager& sm,
    RailwayNetwork& net, OperationLogger& logger, UndoManager& undo) {
    cout << "\n  ── Undo / Redo ──\n";
    cout << "  Undo depth: " << undo.undoDepth() << "\n";
    cout << "  Redo depth: " << undo.redoDepth() << "\n";
    cout << "  [1] Undo\n";
    cout << "  [2] Redo\n";
    cout << "  [0] Back\n";
    int c = readInt("  > ", 0, 2);
    if (c == 0) return;

    auto applyRecord = [&](const UndoRecord& r, bool isUndo) {
        switch (r.op) {
        case UndoOp::REGISTER_TRAIN:
            if (isUndo) {
                reg.decommissionTrain(r.trainSnapshot.trainID);
                cm.removeTrain(r.trainSnapshot.trainID);
                sm.removeTrain(r.trainSnapshot.trainID);
                logger.logAction("UNDO_REGISTER",
                    "Undone: registered train " + std::to_string(r.trainSnapshot.trainID),
                    currentDate(), r.trainSnapshot.trainID);
            }
            else {
                reg.registerTrain(r.trainSnapshot);
                cm.initTrain(r.trainSnapshot.trainID);
                logger.logAction("REDO_REGISTER",
                    "Redone: registered train " + std::to_string(r.trainSnapshot.trainID),
                    currentDate(), r.trainSnapshot.trainID);
            }
            break;
        case UndoOp::DECOMMISSION_TRAIN:
            if (isUndo) {
                reg.registerTrain(r.trainSnapshot);
                cm.initTrain(r.trainSnapshot.trainID);
                logger.logAction("UNDO_DECOMMISSION",
                    "Undone: decommissioned train restored",
                    currentDate(), r.trainSnapshot.trainID);
            }
            else {
                reg.decommissionTrain(r.trainSnapshot.trainID);
                cm.removeTrain(r.trainSnapshot.trainID);
                sm.removeTrain(r.trainSnapshot.trainID);
            }
            break;
        case UndoOp::BOOK_SEAT:
            if (isUndo) {
                sm.cancelSeat(r.trainSnapshot.trainID, r.seatSnapshot.seatNumber);
                logger.logAction("UNDO_BOOK", "Undone: seat " +
                    std::to_string(r.seatSnapshot.seatNumber), currentDate(),
                    r.trainSnapshot.trainID);
            }
            else {
                sm.bookSeat(r.trainSnapshot.trainID, r.seatSnapshot.seatNumber,
                    r.seatSnapshot.passengerName, r.seatSnapshot.passengerID);
            }
            break;
        case UndoOp::CANCEL_SEAT:
            if (isUndo) {
                sm.bookSeat(r.trainSnapshot.trainID, r.seatSnapshot.seatNumber,
                    r.seatSnapshot.passengerName, r.seatSnapshot.passengerID);
                logger.logAction("UNDO_CANCEL", "Undone: seat " +
                    std::to_string(r.seatSnapshot.seatNumber) + " re-booked",
                    currentDate(), r.trainSnapshot.trainID);
            }
            else {
                sm.cancelSeat(r.trainSnapshot.trainID, r.seatSnapshot.seatNumber);
            }
            break;
        case UndoOp::ATTACH_COACH:
            if (isUndo) {
                cm.detachByID(r.trainSnapshot.trainID, r.coachSnapshot.coachID);
                logger.logAction("UNDO_ATTACH",
                    "Undone: coach " + std::to_string(r.coachSnapshot.coachID) + " detached",
                    currentDate(), r.trainSnapshot.trainID);
            }
            else {
                cm.attachRear(r.trainSnapshot.trainID,
                    r.coachSnapshot.coachType, r.coachSnapshot.totalSeats);
            }
            break;
        case UndoOp::DETACH_COACH:
            if (isUndo) {
                cm.attachRear(r.trainSnapshot.trainID,
                    r.coachSnapshot.coachType, r.coachSnapshot.totalSeats);
                logger.logAction("UNDO_DETACH", "Undone: coach re-attached",
                    currentDate(), r.trainSnapshot.trainID);
            }
            else {
                cm.detachByID(r.trainSnapshot.trainID, r.coachSnapshot.coachID);
            }
            break;
        case UndoOp::ADD_TRACK:
            if (isUndo) {
                net.removeTrack(r.edgeFrom, r.edgeTo);
                logger.logAction("UNDO_ADD_TRACK",
                    "Undone: " + r.edgeFrom + " -- " + r.edgeTo + " removed",
                    currentDate());
            }
            else {
                net.addTrack(r.edgeFrom, r.edgeTo, r.edgeDist, r.edgeTime);
            }
            break;
        case UndoOp::REMOVE_TRACK:
            if (isUndo) {
                net.addTrack(r.edgeFrom, r.edgeTo, r.edgeDist, r.edgeTime);
                logger.logAction("UNDO_REMOVE_TRACK",
                    "Undone: " + r.edgeFrom + " -- " + r.edgeTo + " restored",
                    currentDate());
            }
            else {
                net.removeTrack(r.edgeFrom, r.edgeTo);
            }
            break;
        }
        };

    if (c == 1) {
        if (!undo.canUndo()) { cout << "  [i] Nothing to undo.\n"; }
        else { UndoRecord r = undo.popUndo(); applyRecord(r, true); cout << "  [~] Undo applied.\n"; }
    }
    else if (c == 2) {
        if (!undo.canRedo()) { cout << "  [i] Nothing to redo.\n"; }
        else { UndoRecord r = undo.popRedo(); applyRecord(r, false); cout << "  [~] Redo applied.\n"; }
    }
    pause();
}
