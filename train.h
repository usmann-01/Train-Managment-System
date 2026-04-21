#pragma once
// ============================================================
//  train.h  —  Core struct definitions for The Iron Nexus
// ============================================================
#include <string>
using std::string;

// ── Coach ──────────────────────────────────────────────────
struct Coach {
    int    coachID;
    string coachType;    // "Economy" | "Business" | "VIP" | "Sleeper"
    int    totalSeats;
    int    coachPosition; // 1-based sequential position in the train

    Coach() : coachID(0), coachType("Economy"), totalSeats(50), coachPosition(0) {}
    Coach(int id, const string& type, int seats, int pos)
        : coachID(id), coachType(type), totalSeats(seats), coachPosition(pos) {
    }
};

// ── Seat ───────────────────────────────────────────────────
struct Seat {
    int    seatNumber;    // unique across the entire train
    int    coachID;
    bool   isBooked;
    string passengerName;
    string passengerID;   // CNIC / passport

    Seat() : seatNumber(0), coachID(0), isBooked(false), passengerName(""), passengerID("") {}
    Seat(int num, int cid) : seatNumber(num), coachID(cid), isBooked(false), passengerName(""), passengerID("") {}
};

// ── Train ──────────────────────────────────────────────────
struct Train {
    int    trainID;
    string trainName;
    string source;
    string destination;
    string departureTime; // "HH:MM"
    string arrivalTime;   // "HH:MM"
    int    totalCoaches;
    string status;        // "Active" | "Maintenance" | "Decommissioned"

    Train() : trainID(0), trainName(""), source(""), destination(""),
        departureTime("00:00"), arrivalTime("00:00"),
        totalCoaches(0), status("Active") {
    }
    Train(int id, const string& name, const string& src, const string& dst,
        const string& dep, const string& arr)
        : trainID(id), trainName(name), source(src), destination(dst),
        departureTime(dep), arrivalTime(arr), totalCoaches(0), status("Active") {
    }
};

// ── LogEntry ───────────────────────────────────────────────
struct LogEntry {
    string actionName;
    string description;
    string dateCreated;
    string dateModified;
    int    relatedID;     // trainID / coachID / seatNumber this action concerns

    LogEntry() : actionName(""), description(""), dateCreated(""), dateModified(""), relatedID(-1) {}
    LogEntry(const string& action, const string& desc,
        const string& created, const string& modified, int id = -1)
        : actionName(action), description(desc),
        dateCreated(created), dateModified(modified), relatedID(id) {
    }
};

// ── UndoRecord ─────────────────────────────────────────────
// Stores enough information to reverse or re-apply one operation
enum class UndoOp {
    REGISTER_TRAIN,
    DECOMMISSION_TRAIN,
    ATTACH_COACH,
    DETACH_COACH,
    BOOK_SEAT,
    CANCEL_SEAT,
    ADD_TRACK,
    REMOVE_TRACK
};

struct UndoRecord {
    UndoOp  op;
    Train   trainSnapshot;
    Coach   coachSnapshot;
    Seat    seatSnapshot;
    // Network edge info
    string  edgeFrom;
    string  edgeTo;
    int     edgeDist;
    int     edgeTime;

    UndoRecord() : op(UndoOp::REGISTER_TRAIN), edgeDist(0), edgeTime(0) {}
};
