#pragma once
// ============================================================
//  undo.h  —  Undo / Redo Manager
//  Uses two hand-rolled Stacks (backed by DLL).
//  Scope: train register/decommission, coach attach/detach,
//         seat book/cancel, track add/remove.
// ============================================================
#include "dataSt.h"
#include "train.h"

class UndoManager {
    Stack<UndoRecord> undoStack;
    Stack<UndoRecord> redoStack;

    static const int MAX_HISTORY = 100;

public:
    // ── Push a snapshot before every mutating operation ───
    void recordTrain(UndoOp op, const Train& t) {
        UndoRecord r;
        r.op = op;
        r.trainSnapshot = t;
        undoStack.push(r);
        // A new action clears redo history
        while (!redoStack.isEmpty()) redoStack.pop();
    }

    void recordCoach(UndoOp op, int trainID, const Coach& c) {
        UndoRecord r;
        r.op = op;
        r.trainSnapshot.trainID = trainID;
        r.coachSnapshot = c;
        undoStack.push(r);
        while (!redoStack.isEmpty()) redoStack.pop();
    }

    void recordSeat(UndoOp op, int trainID, const Seat& s) {
        UndoRecord r;
        r.op = op;
        r.trainSnapshot.trainID = trainID;
        r.seatSnapshot = s;
        undoStack.push(r);
        while (!redoStack.isEmpty()) redoStack.pop();
    }

    void recordTrack(UndoOp op, const string& from, const string& to, int dist, int time) {
        UndoRecord r;
        r.op = op;
        r.edgeFrom = from;
        r.edgeTo = to;
        r.edgeDist = dist;
        r.edgeTime = time;
        undoStack.push(r);
        while (!redoStack.isEmpty()) redoStack.pop();
    }

    bool canUndo() const { return !undoStack.isEmpty(); }
    bool canRedo() const { return !redoStack.isEmpty(); }

    // Pop the topmost undo record; caller applies the reverse
    UndoRecord popUndo() {
        UndoRecord r = undoStack.pop();
        redoStack.push(r);
        return r;
    }

    // Pop the topmost redo record; caller re-applies the operation
    UndoRecord popRedo() {
        UndoRecord r = redoStack.pop();
        undoStack.push(r);
        return r;
    }

    int undoDepth() const { return undoStack.getSize(); }
    int redoDepth() const { return redoStack.getSize(); }
};
