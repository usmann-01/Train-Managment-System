#pragma once
// ============================================================
//  modules/storage.h  —  Module 6: File I/O (fstream only)
// ============================================================
#include "../Data_Strucutres_PROJECT/dataSt.h"
#include "../Data_Strucutres_PROJECT/train.h"
#include "../Data_Strucutres_PROJECT/registry.h"
#include "../Data_Strucutres_PROJECT/coaches.h"
#include "../Data_Strucutres_PROJECT/network.h"
#include "../Data_Strucutres_PROJECT/seating.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using std::ofstream;
using std::ifstream;
using std::fstream;
using std::string;
using std::cout;
using std::endl;
using std::getline;
using std::istringstream;

// ── Delimiter used in flat files ──────────────────────────
static const char DELIM = '|';

// ── Helper: escape / unescape pipe chars in strings ──────
static string escapePipe(const string& s) {
    string out;
    for (char c : s) { if (c == '|') out += "\\P"; else out += c; }
    return out;
}
static string unescapePipe(const string& s) {
    string out;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '\\' && i + 1 < s.size() && s[i + 1] == 'P') { out += '|'; i++; }
        else out += s[i];
    }
    return out;
}

class StorageManager {
public:
    // ── Module 1: Train Registry ─────────────────────────
    bool saveRegistry(const TrainRegistry& reg, const string& filename = "trains.txt") {
        ofstream f(filename, std::ios::out | std::ios::trunc);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        reg.avl.preOrder([&](const Train& t) {
            f << t.trainID << DELIM
                << escapePipe(t.trainName) << DELIM
                << escapePipe(t.source) << DELIM
                << escapePipe(t.destination) << DELIM
                << t.departureTime << DELIM
                << t.arrivalTime << DELIM
                << t.totalCoaches << DELIM
                << escapePipe(t.status) << "\n";
            });
        f.close();
        cout << "  [>] Registry saved to " << filename << "\n";
        return true;
    }

    bool saveRegistryTree(const TrainRegistry& reg, const string& filename = "avl_tree.txt") {
        ofstream f(filename, std::ios::out | std::ios::trunc);
        if (!f) return false;
        f << "# AVL Tree Pre-Order Dump (load in this order to reconstruct shape)\n";
        reg.avl.preOrder([&](const Train& t) {
            f << t.trainID << DELIM << escapePipe(t.trainName) << DELIM
                << escapePipe(t.source) << DELIM << escapePipe(t.destination) << DELIM
                << t.departureTime << DELIM << t.arrivalTime << DELIM
                << t.totalCoaches << DELIM << escapePipe(t.status) << "\n";
            });
        f.close();
        cout << "  [>] AVL tree structure saved to " << filename << "\n";
        return true;
    }

    bool loadRegistry(TrainRegistry& reg, const string& filename = "trains.txt") {
        ifstream f(filename);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        string line;
        int count = 0;
        while (getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            istringstream ss(line);
            string tok;
            Train t;
            getline(ss, tok, DELIM); t.trainID = std::stoi(tok);
            getline(ss, tok, DELIM); t.trainName = unescapePipe(tok);
            getline(ss, tok, DELIM); t.source = unescapePipe(tok);
            getline(ss, tok, DELIM); t.destination = unescapePipe(tok);
            getline(ss, tok, DELIM); t.departureTime = tok;
            getline(ss, tok, DELIM); t.arrivalTime = tok;
            getline(ss, tok, DELIM); t.totalCoaches = std::stoi(tok);
            getline(ss, tok, DELIM); t.status = unescapePipe(tok);
            reg.avl.insert(t, t.trainID);
            count++;
        }
        f.close();
        cout << "  [<] Registry loaded: " << count << " trains from " << filename << "\n";
        return true;
    }

    // ── Module 2: Coaches ─────────────────────────────────
    bool saveCoaches(const CoachManager& cm, const string& filename = "coaches.txt") {
        ofstream f(filename, std::ios::out | std::ios::trunc);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        cm.trainCoaches.forEach([&](int trainID, DoublyLinkedList<Coach>* dll) {
            if (!dll) return;
            dll->traverseForward([&](const Coach& c) {
                f << trainID << DELIM
                    << c.coachID << DELIM
                    << escapePipe(c.coachType) << DELIM
                    << c.totalSeats << DELIM
                    << c.coachPosition << "\n";
                });
            });
        f.close();
        cout << "  [>] Coaches saved to " << filename << "\n";
        return true;
    }

    bool loadCoaches(CoachManager& cm, const string& filename = "coaches.txt") {
        ifstream f(filename);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        string line;
        int count = 0;
        while (getline(f, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string tok;
            int trainID, coachID, seats, pos;
            string type;
            getline(ss, tok, DELIM); trainID = std::stoi(tok);
            getline(ss, tok, DELIM); coachID = std::stoi(tok);
            getline(ss, tok, DELIM); type = unescapePipe(tok);
            getline(ss, tok, DELIM); seats = std::stoi(tok);
            getline(ss, tok, DELIM); pos = std::stoi(tok);
            cm.initTrain(trainID);
            auto* dll = cm.getList(trainID);
            Coach c(coachID, type, seats, pos);
            dll->insertBack(c);
            if (coachID >= cm.nextCoachID) cm.nextCoachID = coachID + 1;
            count++;
        }
        f.close();
        cout << "  [<] Coaches loaded: " << count << " records from " << filename << "\n";
        return true;
    }

    // ── Module 3: Network ─────────────────────────────────
    bool saveNetwork(const RailwayNetwork& net, const string& filename = "network.txt") {
        ofstream f(filename, std::ios::out | std::ios::trunc);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        f << net.graph.numCities << "\n";
        for (int i = 0; i < net.graph.numCities; i++)
            f << net.graph.cityNames[i] << "\n";
        for (int i = 0; i < net.graph.numCities; i++)
            for (int j = 0; j < net.graph.numCities; j++)
                if (net.graph.adj[i][j].exists)
                    f << i << DELIM << j << DELIM
                    << net.graph.adj[i][j].distanceKm << DELIM
                    << net.graph.adj[i][j].travelTimeMin << "\n";
        f.close();
        cout << "  [>] Network saved to " << filename << "\n";
        return true;
    }

    bool loadNetwork(RailwayNetwork& net, const string& filename = "network.txt") {
        ifstream f(filename);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        int nc;
        f >> nc;
        f.ignore();
        net.graph.numCities = 0;
        for (int i = 0; i < MAX_CITIES; i++) net.graph.cityNames[i] = "";
        for (int i = 0; i < MAX_CITIES; i++)
            for (int j = 0; j < MAX_CITIES; j++)
                net.graph.adj[i][j].exists = false;
        for (int i = 0; i < nc; i++) {
            string name;
            getline(f, name);
            net.graph.cityNames[net.graph.numCities++] = name;
        }
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string tok;
            getline(ss, tok, DELIM); int a = std::stoi(tok);
            getline(ss, tok, DELIM); int b = std::stoi(tok);
            getline(ss, tok, DELIM); int dist = std::stoi(tok);
            getline(ss, tok, DELIM); int time = std::stoi(tok);
            net.graph.adj[a][b].distanceKm = dist;
            net.graph.adj[a][b].travelTimeMin = time;
            net.graph.adj[a][b].exists = true;
        }
        f.close();
        cout << "  [<] Network loaded from " << filename << "\n";
        return true;
    }

    // ── Module 4: Seats ───────────────────────────────────
    bool saveSeats(const SeatManager& sm, const string& filename = "seats.txt") {
        ofstream f(filename, std::ios::out | std::ios::trunc);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        sm.stores.forEach([&](int trainID, const SeatManager::SeatStore& store) {
            if (!store.hashMap) return;
            store.hashMap->forEach([&](int, const Seat& s) {
                f << trainID << DELIM
                    << s.seatNumber << DELIM
                    << s.coachID << DELIM
                    << (s.isBooked ? 1 : 0) << DELIM
                    << escapePipe(s.passengerName) << DELIM
                    << escapePipe(s.passengerID) << "\n";
                });
            });
        f.close();
        cout << "  [>] Seats saved to " << filename << "\n";
        return true;
    }

    bool loadSeats(SeatManager& sm, const string& filename = "seats.txt") {
        ifstream f(filename);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        string line;
        int count = 0;
        while (getline(f, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string tok;
            int trainID, seatNum, coachID, booked;
            getline(ss, tok, DELIM); trainID = std::stoi(tok);
            getline(ss, tok, DELIM); seatNum = std::stoi(tok);
            getline(ss, tok, DELIM); coachID = std::stoi(tok);
            getline(ss, tok, DELIM); booked = std::stoi(tok);
            string pName, pID;
            getline(ss, tok, DELIM); pName = unescapePipe(tok);
            getline(ss, pID);        pID = unescapePipe(pID);
            SeatManager::SeatStore* sp = sm.stores.getRef(trainID);
            if (!sp) {
                sm.initTrain(trainID, 0);
                sp = sm.stores.getRef(trainID);
            }
            Seat seat(seatNum, coachID);
            seat.isBooked = booked != 0;
            seat.passengerName = pName;
            seat.passengerID = pID;
            sp->hashMap->insert(seatNum, seat);
            sp->bst->remove(seatNum);
            sp->bst->insert(seat, seatNum);
            count++;
        }
        f.close();
        cout << "  [<] Seats loaded: " << count << " records from " << filename << "\n";
        return true;
    }

    bool saveSeatBST(const SeatManager& sm, int trainID, const string& filename = "bst_seats.txt") {
        ofstream f(filename, std::ios::out | std::ios::trunc);
        if (!f) return false;
        f << "# BST Pre-Order dump for trainID=" << trainID << "\n";
        const SeatManager::SeatStore* sp = sm.stores.getRef(trainID);
        if (sp && sp->bst) {
            sp->bst->preOrder([&](const Seat& s) {
                f << s.seatNumber << DELIM << s.coachID << DELIM
                    << (s.isBooked ? 1 : 0) << DELIM
                    << escapePipe(s.passengerName) << DELIM
                    << escapePipe(s.passengerID) << "\n";
                });
        }
        f.close();
        cout << "  [>] BST seat structure saved to " << filename << "\n";
        return true;
    }

    // ── Module 5: Logs ────────────────────────────────────
    bool saveLogs(const OperationLogger& logger, const string& filename = "logs.txt") {
        ofstream f(filename, std::ios::out | std::ios::trunc);
        if (!f) { cout << "  [!] Cannot open " << filename << "\n"; return false; }
        logger.getRawList().traverseForward([&](const LogEntry& e) {
            f << escapePipe(e.actionName) << DELIM
                << escapePipe(e.description) << DELIM
                << e.dateCreated << DELIM
                << e.dateModified << DELIM
                << e.relatedID << "\n";
            });
        f.close();
        cout << "  [>] Logs saved to " << filename << "\n";
        return true;
    }

    bool loadLogs(OperationLogger& logger, const string& filename = "logs.txt") {   
        ifstream f(filename);
        if (!f) { 
            cout << "  [!] Cannot open " << filename << "\n"; 
            return false; 
        }

        string line;
        int count = 0;
        const int maxEntries = 10000;
        LogEntry* entries = new LogEntry[maxEntries]; // Dynamically allocate array

        while (getline(f, line)) {
            if (line.empty()) continue;
            if (count >= maxEntries) {
                cout << "  [!] Maximum log entry limit reached (" << maxEntries << ").\n";
                break;
            }
            istringstream ss(line);
            string tok;
            LogEntry e;
            getline(ss, tok, DELIM); e.actionName = unescapePipe(tok);
            getline(ss, tok, DELIM); e.description = unescapePipe(tok);
            getline(ss, tok, DELIM); e.dateCreated = tok;
            getline(ss, tok, DELIM); e.dateModified = tok;
            getline(ss, tok); e.relatedID = std::stoi(tok);
            entries[count++] = e;
        }
        f.close();

        logger.clearLogs();
        for (int i = 0; i < count; i++) {
            logger.getRawList().insertBack(entries[i]);
        }

        delete[] entries; // Free the dynamically allocated memory
        cout << "  [<] Logs loaded: " << count << " entries from " << filename << "\n";
        return true;
    }

    // ── Save / Load All ───────────────────────────────────
    void saveAll(const TrainRegistry& reg, const CoachManager& cm,
        const RailwayNetwork& net, const SeatManager& sm,
        const OperationLogger& log) {
        cout << "\n  ══ Saving All Modules ══\n";
        saveRegistry(reg);
        saveRegistryTree(reg);
        saveCoaches(cm);
        saveNetwork(net);
        saveSeats(sm);
        saveLogs(log);
        cout << "  ✔  All modules saved.\n";
    }

    void loadAll(TrainRegistry& reg, CoachManager& cm,
        RailwayNetwork& net, SeatManager& sm,
        OperationLogger& log) {
        cout << "\n  ══ Loading All Modules ══\n";
        loadRegistry(reg);
        loadCoaches(cm);
        loadNetwork(net);
        loadSeats(sm);
        loadLogs(log);
        cout << "  ✔  All modules loaded.\n";
    }
};