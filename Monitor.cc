#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

class Monitor : public cSimpleModule {
private:
    // Variables to describe system states
    int numQueues;                          // The number of the queues
    int serverPhase;                        // Server phase
    int queueIndex;                         // Queue's index
    std::vector<int> q;                     // Queue's flags
    std::vector<int> n;                     // Current number of packets in the queues

    StateMessage *statsCollectionEvent;     // Start collecting statistics

    // Variables to collect system states
    // States with only serverPhase variable
    cStdDev C1, C2, S1, S2, I;
    bool isC1, isC2, isS1, isS2, isI;       // Is system in these states?
    simtime_t startOfC1, startOfC2, startOfS1, startOfS2, startOfI;

    // States with serverPhase, q1, q2
    cStdDev C1_1_1, C1_1_0;
    bool isC1_1_1, isC1_1_0;
    simtime_t startOfC1_1_1, startOfC1_1_0;

    cStdDev C2_1_1, C2_0_1;
    bool isC2_1_1, isC2_0_1;
    simtime_t startOfC2_1_1, startOfC2_0_1;

    cStdDev S1_1_1, S1_1_0;
    bool isS1_1_1, isS1_1_0;
    simtime_t startOfS1_1_1, startOfS1_1_0;

    cStdDev S2_1_1, S2_0_1;
    bool isS2_1_1, isS2_0_1;
    simtime_t startOfS2_1_1, startOfS2_0_1;

    // States with all variables: serverPhase, q1, q2, n1, n2
    /*
    constexpr static int n1Max = 20;                    // Maximum of n1
    constexpr static int n2Max = 20;                    // Maximum of n2

    cStdDev C1_1_1_n1_n2[n1Max][n2Max];                 // State C1, q1 = 1, q2 = 1, n1 = i, n2 = j
    bool isC1_1_1_n1_n2[n1Max][n2Max];                  // Is system in state C1, q1 = 1, q2 = 1, n1 = i, n2 = j
    simtime_t startOfC1_1_1_n1_n2[n1Max][n2Max];        // Start of state C1, q1 = 1, q2 = 1, n1 = i, n2 = j

    cStdDev C1_1_0_n1_n2[n1Max][n2Max];                 // State C1, q1 = 1, q2 = 0, n1 = i, n2 = j
    bool isC1_1_0_n1_n2[n1Max][n2Max];                  // Is system in state C1, q1 = 1, q2 = 0, n1 = i, n2 = j
    simtime_t startOfC1_1_0_n1_n2[n1Max][n2Max];        // Start of state C1, q1 = 1, q2 = 0, n1 = i, n2 = j

    cStdDev C2_1_1_n1_n2[n1Max][n2Max];                 // State C2, q1 = 1, q2 = 1, n1 = i, n2 = j
    bool isC2_1_1_n1_n2[n1Max][n2Max];                  // Is system in state C2, q1 = 1, q2 = 1, n1 = i, n2 = j
    simtime_t startOfC2_1_1_n1_n2[n1Max][n2Max];        // Start of state C2, q1 = 1, q2 = 1, n1 = i, n2 = j

    cStdDev C2_0_1_n1_n2[n1Max][n2Max];                 // State C2, q1 = 0, q2 = 1, n1 = i, n2 = j
    bool isC2_0_1_n1_n2[n1Max][n2Max];                  // Is system in state C2, q1 = 0, q2 = 1, n1 = i, n2 = j
    simtime_t startOfC2_0_1_n1_n2[n1Max][n2Max];        // Start of state C2, q1 = 0, q2 = 1, n1 = i, n2 = j

    cStdDev S1_1_1_n1_n2[n1Max][n2Max];                 // State S1, q1 = 1, q2 = 1, n1 = i, n2 = j
    bool isS1_1_1_n1_n2[n1Max][n2Max];                  // Is system in state S1, q1 = 1, q2 = 1, n1 = i, n2 = j
    simtime_t startOfS1_1_1_n1_n2[n1Max][n2Max];        // Start of state S1, q1 = 1, q2 = 1, n1 = i, n2 = j

    cStdDev S1_1_0_n1_n2[n1Max][n2Max];                 // State S1, q1 = 1, q2 = 0, n1 = i, n2 = j
    bool isS1_1_0_n1_n2[n1Max][n2Max];                  // Is system in state S1, q1 = 1, q2 = 0, n1 = i, n2 = j
    simtime_t startOfS1_1_0_n1_n2[n1Max][n2Max];        // Start of state S1, q1 = 1, q2 = 0, n1 = i, n2 = j

    cStdDev S2_1_1_n1_n2[n1Max][n2Max];                 // State S2, q1 = 1, q2 = 1, n1 = i, n2 = j
    bool isS2_1_1_n1_n2[n1Max][n2Max];                  // Is system in state S2, q1 = 1, q2 = 1, n1 = i, n2 = j
    simtime_t startOfS2_1_1_n1_n2[n1Max][n2Max];        // Start of state C1, S2 = 1, q2 = 1, n1 = i, n2 = j

    cStdDev S2_0_1_n1_n2[n1Max][n2Max];                 // State S2, q1 = 0, q2 = 1, n1 = i, n2 = j
    bool isS2_0_1_n1_n2[n1Max][n2Max];                  // Is system in state S2, q1 = 0, q2 = 1, n1 = i, n2 = j
    simtime_t startOfS2_0_1_n1_n2[n1Max][n2Max];        // Start of state S2, q1 = 0, q2 = 1, n1 = i, n2 = j

    cStdDev I_n1_n2[n1Max][n2Max];                      // State I, n1 = i, n2 = j
    bool isI_n1_n2[n1Max][n2Max];                       // Is system in state I, n1 = i, n2 = j
    simtime_t startOfI_n1_n2[n1Max][n2Max];             // Start of state I, n1 = i, n2 = j
    */

    // Some specific states for testing
    cStdDev C1_0_1_0_0;                                 // State C1,0,1,0,0
    bool isC1_0_1_0_0;                                  // Is system in state C1,0,1,0,0
    simtime_t startOfC1_0_1_0_0;                        // Start of state C1,0,1,0,0

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
    queueIndex = 0;
    for (int i = 0; i < numQueues; i++)
        n.push_back(0);
    for (int i = 0; i < numQueues; i++)
        q.push_back(1);

    // States with only serverPhase variable
    isC1 = isC2 = isS1 = isS2 = isI = false;
    startOfC1 = startOfC2 = startOfS1 = startOfS2 = startOfI = 0;

    // States with serverPhase, q1, q2
    isC1_1_1 = isC1_1_0 = false;
    startOfC1_1_1 = startOfC1_1_0 = 0;
    isC2_1_1 = isC2_0_1 = false;
    startOfC2_1_1 = startOfC2_0_1 = 0;
    isS1_1_1 = isS1_1_0 = false;
    startOfS1_1_1 = startOfS1_1_0 = 0;
    isS2_1_1 = isS2_0_1 = false;
    startOfS2_1_1 = startOfS2_0_1 = 0;

    // States with all variables: serverPhase, q1, q2, n1, n2
/*
    for (int i = 0; i < n1Max; i++)
        for (int j = 0; j < n2Max; j++) {
            isC1_1_1_n1_n2[i][j] = false;
            startOfC1_1_1_n1_n2[i][j] = 0;

            isC1_1_0_n1_n2[i][j] = false;
            startOfC1_1_0_n1_n2[i][j] = 0;

            isC2_1_1_n1_n2[i][j] = false;
            startOfC2_1_1_n1_n2[i][j] = 0;

            isC2_0_1_n1_n2[i][j] = false;
            startOfC2_0_1_n1_n2[i][j] = 0;

            isS1_1_1_n1_n2[i][j] = false;
            startOfS1_1_1_n1_n2[i][j] = 0;

            isS1_1_0_n1_n2[i][j] = false;
            startOfS1_1_0_n1_n2[i][j] = 0;

            isS2_1_1_n1_n2[i][j] = false;
            startOfS2_1_1_n1_n2[i][j] = 0;

            isS2_0_1_n1_n2[i][j] = false;
            startOfS2_0_1_n1_n2[i][j] = 0;

            isS2_0_1_n1_n2[i][j] = false;
            startOfS2_0_1_n1_n2[i][j] = 0;

            isI_n1_n2[i][j] = false;
            startOfI_n1_n2[i][j] = 0;
        }
*/

    // Some specific states for testing
    isC1_0_1_0_0 = false;
    startOfC1_0_1_0_0 = 0;

    statsCollectionEvent = new StateMessage("Collecting statistics");
}

void Monitor::finish() {
    // States with only serverPhase variable
    recordScalar("Probability C1", C1.getSum() * 100 / simTime());
    recordScalar("Probability C2", C2.getSum() * 100 / simTime());
    recordScalar("Probability S1", S1.getSum() * 100 / simTime());
    recordScalar("Probability S2", S2.getSum() * 100 / simTime());
    recordScalar("Probability I", I.getSum() * 100 / simTime());

    // States with serverPhase, q1, q2
    recordScalar("Probability C1_1_1", C1_1_1.getSum() * 100 / simTime());
    recordScalar("Probability C1_1_0", C1_1_0.getSum() * 100 / simTime());
    recordScalar("Probability C2_1_1", C2_1_1.getSum() * 100 / simTime());
    recordScalar("Probability C2_0_1", C2_0_1.getSum() * 100 / simTime());
    recordScalar("Probability S1_1_1", S1_1_1.getSum() * 100 / simTime());
    recordScalar("Probability S1_1_0", S1_1_0.getSum() * 100 / simTime());
    recordScalar("Probability S2_1_1", S2_1_1.getSum() * 100 / simTime());
    recordScalar("Probability S2_0_1", S2_0_1.getSum() * 100 / simTime());

    // States with all variables: serverPhase, q1, q2, n1, n2
/*
    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability C1_1_1_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, C1_1_1_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability C1_1_0_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, C1_1_0_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability C2_1_1_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, C2_1_1_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability C2_0_1_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, C2_0_1_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability S1_1_1_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, S1_1_1_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability S1_1_0_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, S1_1_0_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability S2_1_1_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, S2_1_1_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability S2_0_1_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, S2_0_1_n1_n2[i][j].getSum() * 100 / simTime());
            }

    for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                std::string nameString = "Probability I_" + std::to_string(i) + "_" + std::to_string(j);
                const char *name = nameString.c_str();
                recordScalar(name, I_n1_n2[i][j].getSum() * 100 / simTime());
            }
*/

    // Some specific states for testing
    recordScalar("Probability C1_0_1_0_0", C1_0_1_0_0.getSum() * 100 / simTime());

}

void Monitor::handleMessage(cMessage *msg) {
    StateMessage *stateMsg = check_and_cast<StateMessage *>(msg);

    // msgType = SET_SERVER_PHASE
    if (stateMsg->getMsgType() == SET_SERVER_PHASE) {
        serverPhase = stateMsg->getServerPhase();
        if (serverPhase == IDLING)
            for (int i = 0; i < numQueues; i++)
                q[i] = 1;
        else
            queueIndex = stateMsg->getQueueIndex();
        delete stateMsg;

        EV << serverPhase << "[" <<
                queueIndex+1 << "], " <<
                q[0] << ", " <<
                q[1] << ", " <<
                n[0] << ", " <<
                n[1] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // msgType = SET_N
    else if (stateMsg->getMsgType() == SET_N) {
        n[stateMsg->getQueueIndex()] = stateMsg->getN();
        delete stateMsg;

        EV << serverPhase << "[" <<
                queueIndex+1 << "], " <<
                q[0] << ", " <<
                q[1] << ", " <<
                n[0] << ", " <<
                n[1] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // msgType = SET_Q
    else if (stateMsg->getMsgType() == SET_Q) {
        q[stateMsg->getQueueIndex()] = stateMsg->getQ();
        delete stateMsg;

        EV << serverPhase << "[" <<
                queueIndex+1 << "], " <<
                q[0] << ", " <<
                q[1] << ", " <<
                n[0] << ", " <<
                n[1] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // Stats collection -------------------------------------------------------------------
    else if (stateMsg == statsCollectionEvent) {
        // States with only serverPhase variable ------------------------------------------
        // C1
        if (serverPhase == CONNECTING && queueIndex == 0 && isC1 == false) {
            startOfC1 = simTime();
            isC1 = true;
        }
        else if (!(serverPhase == CONNECTING && queueIndex == 0) && isC1 == true) {
            C1.collect(simTime() - startOfC1);
            isC1 = false;
        }

        // C2
        if (serverPhase == CONNECTING && queueIndex == 1 && isC2 == false) {
            startOfC2 = simTime();
            isC2 = true;
        }
        else if (!(serverPhase == CONNECTING && queueIndex == 1) && isC2 == true) {
            C2.collect(simTime() - startOfC2);
            isC2 = false;
        }

        // S1
        if (serverPhase == SERVICING && queueIndex == 0 && isS1 == false) {
            startOfS1 = simTime();
            isS1 = true;
        }
        else if (!(serverPhase == SERVICING && queueIndex == 0) && isS1 == true) {
            S1.collect(simTime() - startOfS1);
            isS1 = false;
        }

        // S2
        if (serverPhase == SERVICING && queueIndex == 1 && isS2 == false) {
            startOfS2 = simTime();
            isS2 = true;
        }
        else if (!(serverPhase == SERVICING && queueIndex == 1) && isS2 == true) {
            S2.collect(simTime() - startOfS2);
            isS2 = false;
        }

        // I
        if (serverPhase == IDLING && isI == false) {
            startOfI = simTime();
            isI = true;
        }
        else if (serverPhase != IDLING && isI == true) {
            I.collect(simTime() - startOfI);
            isI = false;
        }

        // States with serverPhase, q1, q2 -----------------------------------------------
        // C1_1_1
        if (serverPhase == CONNECTING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isC1_1_1 == false) {
            startOfC1_1_1 = simTime();
            isC1_1_1 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isC1_1_1 == true) {
            C1_1_1.collect(simTime() - startOfC1_1_1);
            isC1_1_1 = false;
        }

        // C1_1_0
        if (serverPhase == CONNECTING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 0 &&
                isC1_1_0 == false) {
            startOfC1_1_0 = simTime();
            isC1_1_0 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 0) &&
                isC1_1_0 == true) {
            C1_1_0.collect(simTime() - startOfC1_1_0);
            isC1_1_0 = false;
        }

        // C2_1_1
        if (serverPhase == CONNECTING &&
                queueIndex == 1 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isC2_1_1 == false) {
            startOfC2_1_1 = simTime();
            isC2_1_1 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                queueIndex == 1 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isC2_1_1 == true) {
            C2_1_1.collect(simTime() - startOfC2_1_1);
            isC2_1_1 = false;
        }

        // C2_0_1
        if (serverPhase == CONNECTING &&
                queueIndex == 1 &&
                q[0] == 0 &&
                q[1] == 1 &&
                isC2_0_1 == false) {
            startOfC2_0_1 = simTime();
            isC2_0_1 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                queueIndex == 1 &&
                q[0] == 0 &&
                q[1] == 1) &&
                isC2_0_1 == true) {
            C2_0_1.collect(simTime() - startOfC2_0_1);
            isC2_0_1 = false;
        }

        // S1_1_1
        if (serverPhase == SERVICING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isS1_1_1 == false) {
            startOfS1_1_1 = simTime();
            isS1_1_1 = true;
        }
        else if (!(serverPhase == SERVICING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isS1_1_1 == true) {
            S1_1_1.collect(simTime() - startOfS1_1_1);
            isS1_1_1 = false;
        }

        // S1_1_0
        if (serverPhase == SERVICING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 0 &&
                isS1_1_0 == false) {
            startOfS1_1_0 = simTime();
            isS1_1_0 = true;
        }
        else if (!(serverPhase == SERVICING &&
                queueIndex == 0 &&
                q[0] == 1 &&
                q[1] == 0) &&
                isS1_1_0 == true) {
            S1_1_0.collect(simTime() - startOfS1_1_0);
            isS1_1_0 = false;
        }

        // S2_1_1
        if (serverPhase == SERVICING &&
                queueIndex == 1 &&
                q[0] == 1 &&
                q[1] == 1 &&
                isS2_1_1 == false) {
            startOfS2_1_1 = simTime();
            isS2_1_1 = true;
        }
        else if (!(serverPhase == SERVICING &&
                queueIndex == 1 &&
                q[0] == 1 &&
                q[1] == 1) &&
                isS2_1_1 == true) {
            S2_1_1.collect(simTime() - startOfS2_1_1);
            isS2_1_1 = false;
        }

        // S2_0_1
        if (serverPhase == SERVICING &&
                queueIndex == 1 &&
                q[0] == 0 &&
                q[1] == 1 &&
                isS2_0_1 == false) {
            startOfS2_0_1 = simTime();
            isS2_0_1 = true;
        }
        else if (!(serverPhase == SERVICING &&
                queueIndex == 1 &&
                q[0] == 0 &&
                q[1] == 1) &&
                isS2_0_1 == true) {
            S2_0_1.collect(simTime() - startOfS2_0_1);
            isS2_0_1 = false;
        }

        //  States with all variables: serverPhase, q1, q2, n1, n2 --------------------------------
/*
        for (int i = 0; i < n1Max; i++)
            for (int j = 0; j < n2Max; j++) {
                // C1_1_1_n1_n2
                if (serverPhase == CONNECTING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j &&
                        isC1_1_1_n1_n2[i][j] == false) {
                    startOfC1_1_1_n1_n2[i][j] = simTime();
                    isC1_1_1_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == CONNECTING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isC1_1_1_n1_n2[i][j] == true) {
                    C1_1_1_n1_n2[i][j].collect(simTime() - startOfC1_1_1_n1_n2[i][j]);
                    isC1_1_1_n1_n2[i][j] = false;
                }

                // C1_1_0_n1_n2
                if (serverPhase == CONNECTING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 0 &&
                        n[0] == i &&
                        n[1] == j &&
                        isC1_1_0_n1_n2[i][j] == false) {
                    startOfC1_1_0_n1_n2[i][j] = simTime();
                    isC1_1_0_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == CONNECTING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 0 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isC1_1_0_n1_n2[i][j] == true) {
                    C1_1_0_n1_n2[i][j].collect(simTime() - startOfC1_1_0_n1_n2[i][j]);
                    isC1_1_0_n1_n2[i][j] = false;
                }

                // C2_1_1_n1_n2
                if (serverPhase == CONNECTING &&
                        queueIndex == 1 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j &&
                        isC2_1_1_n1_n2[i][j] == false) {
                    startOfC2_1_1_n1_n2[i][j] = simTime();
                    isC2_1_1_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == CONNECTING &&
                        queueIndex == 1 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isC2_1_1_n1_n2[i][j] == true) {
                    C2_1_1_n1_n2[i][j].collect(simTime() - startOfC2_1_1_n1_n2[i][j]);
                    isC2_1_1_n1_n2[i][j] = false;
                }

                // C2_0_1_n1_n2
                if (serverPhase == CONNECTING &&
                        queueIndex == 1 &&
                        q[0] == 0 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j &&
                        isC2_0_1_n1_n2[i][j] == false) {
                    startOfC2_0_1_n1_n2[i][j] = simTime();
                    isC2_0_1_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == CONNECTING &&
                        queueIndex == 1 &&
                        q[0] == 0 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isC2_0_1_n1_n2[i][j] == true) {
                    C2_0_1_n1_n2[i][j].collect(simTime() - startOfC2_0_1_n1_n2[i][j]);
                    isC2_0_1_n1_n2[i][j] = false;
                }

                // S1_1_1_n1_n2
                if (serverPhase == SERVICING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j &&
                        isS1_1_1_n1_n2[i][j] == false) {
                    startOfS1_1_1_n1_n2[i][j] = simTime();
                    isS1_1_1_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == SERVICING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isS1_1_1_n1_n2[i][j] == true) {
                    S1_1_1_n1_n2[i][j].collect(simTime() - startOfS1_1_1_n1_n2[i][j]);
                    isS1_1_1_n1_n2[i][j] = false;
                }

                // S1_1_0_n1_n2
                if (serverPhase == SERVICING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 0 &&
                        n[0] == i &&
                        n[1] == j &&
                        isS1_1_0_n1_n2[i][j] == false) {
                    startOfS1_1_0_n1_n2[i][j] = simTime();
                    isS1_1_0_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == SERVICING &&
                        queueIndex == 0 &&
                        q[0] == 1 &&
                        q[1] == 0 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isS1_1_0_n1_n2[i][j] == true) {
                    S1_1_0_n1_n2[i][j].collect(simTime() - startOfS1_1_0_n1_n2[i][j]);
                    isS1_1_0_n1_n2[i][j] = false;
                }

                // S2_1_1_n1_n2
                if (serverPhase == SERVICING &&
                        queueIndex == 1 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j &&
                        isS2_1_1_n1_n2[i][j] == false) {
                    startOfS2_1_1_n1_n2[i][j] = simTime();
                    isS2_1_1_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == SERVICING &&
                        queueIndex == 1 &&
                        q[0] == 1 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isS2_1_1_n1_n2[i][j] == true) {
                    S2_1_1_n1_n2[i][j].collect(simTime() - startOfS2_1_1_n1_n2[i][j]);
                    isS2_1_1_n1_n2[i][j] = false;
                }

                // S2_0_1_n1_n2
                if (serverPhase == SERVICING &&
                        queueIndex == 1 &&
                        q[0] == 0 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j &&
                        isS2_0_1_n1_n2[i][j] == false) {
                    startOfS2_0_1_n1_n2[i][j] = simTime();
                    isS2_0_1_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == SERVICING &&
                        queueIndex == 1 &&
                        q[0] == 0 &&
                        q[1] == 1 &&
                        n[0] == i &&
                        n[1] == j) &&
                        isS2_0_1_n1_n2[i][j] == true) {
                    S2_0_1_n1_n2[i][j].collect(simTime() - startOfS2_0_1_n1_n2[i][j]);
                    isS2_0_1_n1_n2[i][j] = false;
                }

                // I_n1_n2
                if (serverPhase == IDLING &&
                        n[0] == i &&
                        n[1] == j &&
                        isI_n1_n2[i][j] == false) {
                    startOfI_n1_n2[i][j] = simTime();
                    isI_n1_n2[i][j] = true;
                }
                else if (!(serverPhase == IDLING &&
                        n[0] == i &&
                        n[1] == j) &&
                        isI_n1_n2[i][j] == true) {
                    I_n1_n2[i][j].collect(simTime() - startOfI_n1_n2[i][j]);
                    isI_n1_n2[i][j] = false;
                }


            }
*/

        // Some specific states for testing --------------------------------------------
        // C1_0_1_0_0
        if (serverPhase == CONNECTING &&
                queueIndex == 0 &&
                q[0] == 0 &&
                q[1] == 1 &&
                n[0] == 0 &&
                n[1] == 0 &&
                isC1_0_1_0_0 == false) {
            startOfC1_0_1_0_0 = simTime();
            isC1_0_1_0_0 = true;
        }
        else if (!(serverPhase == CONNECTING &&
                queueIndex == 0 &&
                q[0] == 0 &&
                q[1] == 1 &&
                n[0] == 0 &&
                n[1] == 0) &&
                isC1_0_1_0_0 == true) {
            C1_0_1_0_0.collect(simTime() - startOfC1_0_1_0_0);
            isC1_0_1_0_0 = false;
        }



    }
}
