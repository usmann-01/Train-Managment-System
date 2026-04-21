// ============================================================
//  main.cpp  —  The Iron Nexus v1.0 Entry Point
// ============================================================
#include "../Data_Strucutres_PROJECT/menu.h"

int main() {
    // Instantiate all modules
    TrainRegistry  registry;
    CoachManager   coaches;
    RailwayNetwork network;
    SeatManager    seating;
    OperationLogger logger;
    UndoManager    undoMgr;
    StorageManager storage;

    // Log system startup
    logger.logAction("SYSTEM_START",
        "The Iron Nexus v1.0 initialised", "2026-04-21 01:34");

    // Hand control to the main menu loop
    mainMenu(registry, coaches, network, seating, logger, undoMgr, storage);

    return 0;
}
