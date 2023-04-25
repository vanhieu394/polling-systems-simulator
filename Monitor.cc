#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

class Monitor : public cSimpleModule {
private:
    int numQueues;                          // The number of the queues
    int serverPhase;                        // Server phase
    int i;                                  // Queue's index
    std::vector<int> n;                     // Current number of packets in the queues
    std::vector<int> q;                     // Queue's flags

    StateMessage *statsCollectionEvent;     // Start collecting statistics

    // Phase C1
    cStdDev C1;                             // State C1
    bool isC1;                              // Is system in state C1
    simtime_t startOfC1;                    // Start of state C1

    cStdDev C1_n1_n2_1_1;                   // State C1_n1_n2_1_1
    bool isC1_n1_n2_1_1;                    // Is system in state C1,n1,n2,1,1
    simtime_t startOfC1_n1_n2_1_1;          // Start of state C1,n1,n2,1,1

    cStdDev C1_n1_n2_1_0;                   // State C1_n1_n2_1_0
    bool isC1_n1_n2_1_0;                    // Is system in state C1,n1,n2,1,0
    simtime_t startOfC1_n1_n2_1_0;          // Start of state C1,n1,n2,1,0

    // Phase C2
    cStdDev C2;                             // State C1
    bool isC2;                              // Is system in state C2
    simtime_t startOfC2;                    // Start of state C2

    cStdDev C2_n1_n2_1_1;                   // State C2_n1_n2_1_1
    bool isC2_n1_n2_1_1;                    // Is system in state C2,n1,n2,1,1
    simtime_t startOfC2_n1_n2_1_1;          // Start of state C2,n1,n2,1,1

    cStdDev C2_n1_n2_0_1;                   // State C2,n1,n2,0,1
    bool isC2_n1_n2_0_1;                    // Is system in state C2,n1,n2,0,1
    simtime_t startOfC2_n1_n2_0_1;          // Start of state C2,n1,n2,0,1

    // Phase S1
    cStdDev S1;                             // State S1
    bool isS1;                              // Is system in state S1
    simtime_t startOfS1;                    // Start of state S1

    cStdDev S1_n1_n2_1_1;                   // State S1,n1,n2,1,1
    bool isS1_n1_n2_1_1;                    // Is system in state S1,n1,n2,1,1
    simtime_t startOfS1_n1_n2_1_1;          // Start of state S1,n1,n2,1,1

    cStdDev S1_n1_n2_1_0;                   // State S1,n1,n2,1,0
    bool isS1_n1_n2_1_0;                    // Is system in state S1,n1,n2,1,0
    simtime_t startOfS1_n1_n2_1_0;          // Start of state S1,n1,n2,1,0

    // Phase S2
    cStdDev S2;                             // State S2
    bool isS2;                              // Is system in state S2
    simtime_t startOfS2;                    // Start of state S2

    cStdDev S2_n1_n2_1_1;                   // State S2_n1_n2_1_1
    bool isS2_n1_n2_1_1;                    // Is system in state S2,n1,n2,1,1
    simtime_t startOfS2_n1_n2_1_1;          // Start of state S2,n1,n2,1,1

    cStdDev S2_n1_n2_0_1;                   // State S2,n1,n2,0,1
    bool isS2_n1_n2_0_1;                    // Is system in state S2,n1,n2,0,1
    simtime_t startOfS2_n1_n2_0_1;          // Start of state S2,n1,n2,0,1

    // Phase I
    cStdDev I;                              // State I
    bool isI;                               // Is system in state I
    simtime_t startOfI;                     // Start of state I

    // Bonus
    cStdDev C1_n1_n2_0_1;                   // State C1,n1,n2,0,1
    bool isC1_n1_n2_0_1;                    // Is system in state C1,n1,n2,0,1
    simtime_t startOfC1_n1_n2_0_1;          // Start of state C1,n1,n2,0,1

    cStdDev C1_0_0_0_1;                     // State C1,0,0,0,1
    bool isC1_0_0_0_1;                      // Is system in state C1,0,0,0,1
    simtime_t startOfC1_0_0_0_1;            // Start of state C1,0,0,0,1

public:
    Monitor();
    virtual~ Monitor();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Monitor);

Monitor::Monitor() {
    statsCollectionEvent = nullptr;
}

Monitor::~Monitor() {
    cancelAndDelete(statsCollectionEvent);
}

void Monitor::initialize() {
    numQueues = par("numQueues");
    serverPhase = IDLING;
    i = 0;
    for (int i = 0; i < numQueues; i++)
        n.push_back(0);
    for (int i = 0; i < numQueues; i++)
        q.push_back(1);

    // Phase C1
    isC1 = false;
    startOfC1 = 0;
    isC1_n1_n2_1_1 = false;
    startOfC1_n1_n2_1_1 = 0;
    isC1_n1_n2_1_0 = false;
    startOfC1_n1_n2_1_0 = 0;

    // Phase C2
    isC2 = false;
    startOfC2 = 0;
    isC2_n1_n2_1_1 = false;
    startOfC2_n1_n2_1_1 = 0;
    isC2_n1_n2_0_1 = false;
    startOfC2_n1_n2_0_1 = 0;

    // Phase S1
    isS1 = false;
    startOfS1 = 0;
    isS1_n1_n2_1_1 = false;
    startOfS1_n1_n2_1_1 = 0;
    isS1_n1_n2_1_0 = false;
    startOfS1_n1_n2_1_0 = 0;

    // Phase S2
    isS2 = false;
    startOfS2 = 0;
    isS2_n1_n2_1_1 = false;
    startOfS2_n1_n2_1_1 = 0;
    isS2_n1_n2_0_1 = false;
    startOfS2_n1_n2_0_1 = 0;

    // Phase I
    isI = false;
    startOfI = 0;

    // Bonus
    isC1_n1_n2_0_1 = false;
    startOfC1_n1_n2_0_1 = 0;
    isC1_0_0_0_1 = false;
    startOfC1_0_0_0_1 = 0;

    statsCollectionEvent = new StateMessage("Collecting statistics");
}

void Monitor::finish() {
    // Phase C1
    recordScalar("Probability C1", C1.getSum() * 100 / simTime());
    recordScalar("Probability C1_n1_n2_1_1", C1_n1_n2_1_1.getSum() * 100 / simTime());
    recordScalar("Probability C1_n1_n2_1_0", C1_n1_n2_1_0.getSum() * 100 / simTime());

    // Phase C2
    recordScalar("Probability C2", C2.getSum() * 100 / simTime());
    recordScalar("Probability C2_n1_n2_1_1", C2_n1_n2_1_1.getSum() * 100 / simTime());
    recordScalar("Probability C2_n1_n2_0_1", C2_n1_n2_0_1.getSum() * 100 / simTime());

    // Phase S1
    recordScalar("Probability S1", S1.getSum() * 100 / simTime());
    recordScalar("Probability S1_n1_n2_1_1", S1_n1_n2_1_1.getSum() * 100 / simTime());
    recordScalar("Probability S1_n1_n2_1_0", S1_n1_n2_1_0.getSum() * 100 / simTime());

    // Phase S2
    recordScalar("Probability S2", S2.getSum() * 100 / simTime());
    recordScalar("Probability S2_n1_n2_1_1", S2_n1_n2_1_1.getSum() * 100 / simTime());
    recordScalar("Probability S2_n1_n2_0_1", S2_n1_n2_0_1.getSum() * 100 / simTime());

    // Phase I
    recordScalar("Probability I", I.getSum() * 100 / simTime());

    // Bonus
    recordScalar("Probability C1_n1_n2_0_1", C1_n1_n2_0_1.getSum() * 100 / simTime());
    recordScalar("Probability C1_0_0_0_1", C1_0_0_0_1.getSum() * 100 / simTime());
}

void Monitor::handleMessage(cMessage *msg){
    StateMessage *stateMsg = check_and_cast<StateMessage *>(msg);

    // msgType = SET_SERVER_PHASE
    if (stateMsg->getMsgType() == SET_SERVER_PHASE) {
        serverPhase = stateMsg->getServerPhase();
        if (serverPhase == IDLING) {
            q[0] = 1;
            q[1] = 1;
        }
        else
            i = stateMsg->getI();
        delete stateMsg;

        EV << serverPhase << "[" <<
                i+1 << "], " <<
                n[0] << ", " <<
                n[1] << ", " <<
                q[0] << ", " <<
                q[1] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // msgType = SET_N
    else if (stateMsg->getMsgType() == SET_N) {
        n[stateMsg->getI()] = stateMsg->getN();
        delete stateMsg;

        EV << serverPhase << "[" <<
                i+1 << "], " <<
                n[0] << ", " <<
                n[1] << ", " <<
                q[0] << ", " <<
                q[1] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // msgType = SET_Q
    else if (stateMsg->getMsgType() == SET_Q) {
        q[stateMsg->getI()] = stateMsg->getQ();
        delete stateMsg;

        EV << serverPhase << "[" <<
                i+1 << "], " <<
                n[0] << ", " <<
                n[1] << ", " <<
                q[0] << ", " <<
                q[1] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // Stats collection ------------------------------------------------------------
    else if (stateMsg == statsCollectionEvent) {
        // C1 ----------------------------------------------------------------------
        if (serverPhase == CONNECTING && i == 0 && isC1 == false) {
            startOfC1 = simTime();
            isC1 = true;
        }
        else if (!(serverPhase == CONNECTING && i == 0) && isC1 == true) {
            C1.collect(simTime() - startOfC1);
            isC1 = false;
        }

        // C1_n1_n2_1_1
        if (serverPhase == CONNECTING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isC1_n1_n2_1_1 == false) {
            startOfC1_n1_n2_1_1 = simTime();
            isC1_n1_n2_1_1 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isC1_n1_n2_1_1 == true) {
            C1_n1_n2_1_1.collect(simTime() - startOfC1_n1_n2_1_1);
            isC1_n1_n2_1_1 = false;
        }

        // C1_n1_n2_1_0
        if (serverPhase == CONNECTING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 0 &&
                isC1_n1_n2_1_0 == false) {
            startOfC1_n1_n2_1_0 = simTime();
            isC1_n1_n2_1_0 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 0) &&
                isC1_n1_n2_1_0 == true) {
            C1_n1_n2_1_0.collect(simTime() - startOfC1_n1_n2_1_0);
            isC1_n1_n2_1_0 = false;
        }

        // C2 ----------------------------------------------------------------------
        if (serverPhase == CONNECTING && i == 1 && isC2 == false) {
            startOfC2 = simTime();
            isC2 = true;
        }
        else if (!(serverPhase == CONNECTING && i == 1) && isC2 == true) {
            C2.collect(simTime() - startOfC2);
            isC2 = false;
        }

        // C2_n1_n2_1_1
        if (serverPhase == CONNECTING &&
                i == 1 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isC2_n1_n2_1_1 == false) {
            startOfC2_n1_n2_1_1 = simTime();
            isC2_n1_n2_1_1 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                i == 1 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isC2_n1_n2_1_1 == true) {
            C2_n1_n2_1_1.collect(simTime() - startOfC2_n1_n2_1_1);
            isC2_n1_n2_1_1 = false;
        }

        // C2_n1_n2_0_1
        if (serverPhase == CONNECTING &&
                i == 1 &&
                q[0] == 0 &&
                q[1] == 1 &&
                isC2_n1_n2_0_1 == false) {
            startOfC2_n1_n2_0_1 = simTime();
            isC2_n1_n2_0_1 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                i == 1 &&
                q[0] == 0 &&
                q[1] == 1) &&
                isC2_n1_n2_0_1 == true) {
            C2_n1_n2_0_1.collect(simTime() - startOfC2_n1_n2_0_1);
            isC2_n1_n2_0_1 = false;
        }

        // S1 -----------------------------------------------------------------------
        if (serverPhase == SERVICING && i == 0 && isS1 == false) {
            startOfS1 = simTime();
            isS1 = true;
        }
        else if (!(serverPhase == SERVICING && i == 0) && isS1 == true) {
            S1.collect(simTime() - startOfS1);
            isS1 = false;
        }

        // S1_n1_n2_1_1
        if (serverPhase == SERVICING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isS1_n1_n2_1_1 == false) {
            startOfS1_n1_n2_1_1 = simTime();
            isS1_n1_n2_1_1 = true;
        }
        else if (!(serverPhase == SERVICING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isS1_n1_n2_1_1 == true) {
            S1_n1_n2_1_1.collect(simTime() - startOfS1_n1_n2_1_1);
            isS1_n1_n2_1_1 = false;
        }

        // S1_n1_n2_1_0
        if (serverPhase == SERVICING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 0 &&
                isS1_n1_n2_1_0 == false) {
            startOfS1_n1_n2_1_0 = simTime();
            isS1_n1_n2_1_0 = true;
        }
        else if (!(serverPhase == SERVICING &&
                i == 0 &&
                q[0] == 1 &&
                q[1] == 0) &&
                isS1_n1_n2_1_0 == true) {
            S1_n1_n2_1_0.collect(simTime() - startOfS1_n1_n2_1_0);
            isS1_n1_n2_1_0 = false;
        }

        // S2 -----------------------------------------------------------------------
        if (serverPhase == SERVICING && i == 1 && isS2 == false) {
            startOfS2 = simTime();
            isS2 = true;
        }
        else if (!(serverPhase == SERVICING && i == 1) && isS2 == true) {
            S2.collect(simTime() - startOfS2);
            isS2 = false;
        }

        // S2_n1_n2_1_1
        if (serverPhase == SERVICING &&
                i == 1 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isS2_n1_n2_1_1 == false) {
            startOfS2_n1_n2_1_1 = simTime();
            isS2_n1_n2_1_1 = true;
        }
        else if (!(serverPhase == SERVICING &&
                i == 1 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isS2_n1_n2_1_1 == true) {
            S2_n1_n2_1_1.collect(simTime() - startOfS2_n1_n2_1_1);
            isS2_n1_n2_1_1 = false;
        }

        // S2_n1_n2_0_1
        if (serverPhase == SERVICING &&
                i == 1 &&
                q[0] == 0 &&
                q[1] == 1 &&
                isS2_n1_n2_0_1 == false) {
            startOfS2_n1_n2_0_1 = simTime();
            isS2_n1_n2_0_1 = true;
        }
        else if (!(serverPhase == SERVICING &&
                i == 1 &&
                q[0] == 0 &&
                q[1] == 1) &&
                isS2_n1_n2_0_1 == true) {
            S2_n1_n2_0_1.collect(simTime() - startOfS2_n1_n2_0_1);
            isS2_n1_n2_0_1 = false;
        }

        // I ------------------------------------------------------------------------
        if (serverPhase == IDLING && isI == false) {
            startOfI = simTime();
            isI = true;
        }
        else if (serverPhase != IDLING && isI == true) {
            I.collect(simTime() - startOfI);
            isI = false;
        }


        // Bonus ---------------------------------------------------------------------
        // C1_0_0_0_1
        if (serverPhase == CONNECTING &&
                i == 0 &&
                n[0] == 0 &&
                n[1] == 0 &&
                q[0] == 0 &&
                q[1] == 1 &&
                isC1_0_0_0_1 == false) {
            startOfC1_0_0_0_1 = simTime();
            isC1_0_0_0_1 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                i == 0 &&
                n[0] == 0 &&
                n[1] == 0 &&
                q[0] == 0 &&
                q[1] == 1) &&
                isC1_0_0_0_1 == true) {
            C1_0_0_0_1.collect(simTime() - startOfC1_0_0_0_1);
            isC1_0_0_0_1 = false;
        }
    }
}

