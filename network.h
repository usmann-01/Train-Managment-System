#pragma once
// ============================================================
//  modules/network.h  —  Module 3: Railway Track Network
// ============================================================
#include "../Data_Strucutres_PROJECT/dataSt.h"
#include <iostream>
#include <iomanip>
#include <string>
using std::cout;
using std::endl;
using std::string;

class RailwayNetwork {
public:
    Graph graph;

    RailwayNetwork() {
        // Pre-load 15 Pakistani railway hubs
        const char* cities[] = {
            "Karachi","Lahore","Islamabad","Rawalpindi","Peshawar",
            "Quetta","Multan","Faisalabad","Hyderabad","Sukkur",
            "Larkana","Bahawalpur","Sialkot","Gujranwala","Khanewal"
        };
        for (int i = 0; i < 15; i++) graph.addCity(cities[i]);

        // Representative track connections [km, min]
        graph.addTrack("Karachi", "Hyderabad", 163, 150);
        graph.addTrack("Hyderabad", "Sukkur", 393, 360);
        graph.addTrack("Sukkur", "Larkana", 80, 70);
        graph.addTrack("Sukkur", "Bahawalpur", 380, 350);
        graph.addTrack("Bahawalpur", "Multan", 100, 90);
        graph.addTrack("Multan", "Khanewal", 86, 78);
        graph.addTrack("Khanewal", "Lahore", 257, 240);
        graph.addTrack("Lahore", "Faisalabad", 130, 120);
        graph.addTrack("Lahore", "Gujranwala", 64, 56);
        graph.addTrack("Gujranwala", "Sialkot", 65, 58);
        graph.addTrack("Lahore", "Rawalpindi", 365, 330);
        graph.addTrack("Rawalpindi", "Islamabad", 10, 15);
        graph.addTrack("Islamabad", "Peshawar", 165, 150);
        graph.addTrack("Rawalpindi", "Peshawar", 155, 140);
        graph.addTrack("Karachi", "Quetta", 683, 620);
        graph.addTrack("Quetta", "Multan", 670, 610);
        graph.addTrack("Multan", "Lahore", 340, 310);
        graph.addTrack("Faisalabad", "Khanewal", 180, 165);
    }

    // ── City management ───────────────────────────────────
    bool addStation(const string& name) {
        if (graph.addCity(name)) {
            cout << "  [+] Station '" << name << "' added to network.\n";
            return true;
        }
        cout << "  [!] Station '" << name << "' already exists or network is full.\n";
        return false;
    }

    bool removeStation(const string& name) {
        if (graph.removeCity(name)) {
            cout << "  [-] Station '" << name << "' removed from network.\n";
            return true;
        }
        cout << "  [!] Station '" << name << "' not found.\n";
        return false;
    }

    // ── Track management ──────────────────────────────────
    bool addTrack(const string& from, const string& to, int km, int mins,
        bool bidir = true) {
        if (graph.addTrack(from, to, km, mins, bidir)) {
            cout << "  [+] Track: " << from << " <-> " << to
                << "  [" << km << " km / " << mins << " min]\n";
            return true;
        }
        cout << "  [!] One or both stations not found.\n";
        return false;
    }

    bool removeTrack(const string& from, const string& to, bool bidir = true) {
        if (graph.removeTrack(from, to, bidir)) {
            cout << "  [-] Track " << from << " -- " << to << " removed.\n";
            return true;
        }
        cout << "  [!] Track not found.\n";
        return false;
    }

    // ── Shortest path ─────────────────────────────────────
    // mode: 0 = distance (km), 1 = travel time (min)
    void findShortestPath(const string& from, const string& to, int mode = 0) const {
        int path[MAX_CITIES];
        int pathLen = 0, cost = 0;
        if (!graph.shortestPath(from, to, mode, path, pathLen, cost)) {
            cout << "  [!] No path found between '" << from << "' and '" << to << "'.\n";
            return;
        }
        cout << "\n  ══ Shortest Path (" << (mode == 0 ? "Distance" : "Time") << ") ══\n";
        cout << "  Route: ";
        for (int i = 0; i < pathLen; i++) {
            cout << graph.cityNames[path[i]];
            if (i < pathLen - 1) cout << " -> ";
        }
        if (mode == 0) cout << "\n  Total: " << cost << " km\n";
        else            cout << "\n  Total: " << cost << " min\n";
    }

    void displayNetwork() const {
        cout << "\n  ══ Railway Network (" << graph.numCities << " stations) ══\n";
        for (int i = 0; i < graph.numCities; i++) {
            cout << "  [" << i << "] " << graph.cityNames[i] << "\n";
        }
        graph.displayMatrix();
    }

    void listStations() const {
        cout << "\n  Stations:\n";
        for (int i = 0; i < graph.numCities; i++)
            cout << "    " << (i + 1) << ". " << graph.cityNames[i] << "\n";
    }
};
