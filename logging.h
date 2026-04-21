#pragma once
// ============================================================
//  modules/logging.h  —  Module 5: Operation Logging
//  DLL operated as a LIFO stack — newest entries at front
// ============================================================
#include "../Data_Strucutres_PROJECT/dataSt.h"
#include "../Data_Strucutres_PROJECT/train.h"
#include <iostream>
#include <iomanip>
#include <string>
using std::cout;
using std::setw;
using std::left;
using std::endl;
using std::string;

class OperationLogger {
    DoublyLinkedList<LogEntry> logs; // head = most recent

    // Simple ordering rules: some actions require a precondition log to exist
    // e.g., ATTACH_COACH requires a prior REGISTER_TRAIN log for same trainID
    bool validateLogic(const LogEntry& entry) {
        if (entry.relatedID == -1) return true; // system-level log

        string action = entry.actionName;
        // Actions that require train to already be registered
        if (action == "ATTACH_COACH" || action == "DETACH_COACH" ||
            action == "BOOK_SEAT" || action == "CANCEL_SEAT") {
            // Scan existing logs for REGISTER_TRAIN with same relatedID
            bool foundRegister = false;
            logs.traverseForward([&](const LogEntry& e) {
                if (e.actionName == "REGISTER_TRAIN" && e.relatedID == entry.relatedID)
                    foundRegister = true;
                });
            if (!foundRegister) {
                bool foundDecommission = false;
                logs.traverseForward([&](const LogEntry& e) {
                    if (e.actionName == "DECOMMISSION_TRAIN" && e.relatedID == entry.relatedID)
                        foundDecommission = true;
                    });
                if (foundDecommission) {
                    cout << "  [!] Log rejected: train " << entry.relatedID
                        << " is decommissioned.\n";
                    return false;
                }
            }
        }
        return true;
    }

public:
    // Push a new log entry (LIFO — pushed to front)
    bool logAction(const string& action, const string& desc,
        const string& date, int relatedID = -1) {
        LogEntry entry(action, desc, date, date, relatedID);
        if (!validateLogic(entry)) return false;
        logs.insertFront(entry);
        return true;
    }

    // View most recent N logs (forward from head)
    void viewRecentLogs(int n = 10) const {
        if (logs.isEmpty()) { cout << "  [i] No logs recorded.\n"; return; }
        cout << "\n  ══ Recent Operations ══\n";
        cout << "  " << setw(22) << left << "Action"
            << setw(40) << left << "Description"
            << "Date\n";
        cout << "  " << string(72, '-') << "\n";
        int count = 0;
        logs.traverseForward([&](const LogEntry& e) {
            if (count < n) {
                cout << "  " << setw(22) << left << e.actionName
                    << setw(40) << left << e.description.substr(0, 38)
                    << e.dateCreated << "\n";
                count++;
            }
            });
    }

    // View oldest N logs (backward from tail)
    void viewOldestLogs(int n = 10) const {
        if (logs.isEmpty()) { cout << "  [i] No logs recorded.\n"; return; }
        cout << "\n  ══ Oldest Operations ══\n";
        cout << "  " << setw(22) << left << "Action"
            << setw(40) << left << "Description"
            << "Date\n";
        cout << "  " << string(72, '-') << "\n";
        int count = 0;
        logs.traverseBackward([&](const LogEntry& e) {
            if (count < n) {
                cout << "  " << setw(22) << left << e.actionName
                    << setw(40) << left << e.description.substr(0, 38)
                    << e.dateCreated << "\n";
                count++;
            }
            });
    }

    // Pop the most recent log (for undo support)
    LogEntry popLatest() {
        if (logs.isEmpty()) return LogEntry{};
        LogEntry e = logs.head->data;
        logs.deleteAt(1);
        return e;
    }

    void clearLogs() { logs.clear(); }
    int  logCount()  const { return logs.size; }

    // Expose raw list for file I/O
    const DoublyLinkedList<LogEntry>& getRawList() const { return logs; }
    DoublyLinkedList<LogEntry>& getRawList() { return logs; }
};
