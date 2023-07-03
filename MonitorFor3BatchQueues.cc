// Monitor module for batch service polling system with 3 queues (cyclic polling order) (DCCN 2023)

#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

class MonitorFor3BatchQueues : public cSimpleModule {
private:
    // Variables to describe system states
    int numQueues;                          // The number of the queues
    int serverPhase;                        // Server phase
    int queueIndex;                         // Queue's index
    std::vector<int> n;                     // Current number of packets in the queues

    StateMessage *statsCollectionEvent;     // Start collecting statistics

    // Variables to collect system states
    // States with only serverPhase variable
    cStdDev C1, C2, C3, S1, S2, S3;
    bool isC1, isC2, isC3, isS1, isS2, isS3;       // Is system in these states?
    simtime_t startOfC1, startOfC2, startOfC3, startOfS1, startOfS2, startOfS3;

    // States with all variables: serverPhase, n1, n2, n3
    constexpr static int qCap1 = 20;                    // Capacity of queues
    constexpr static int qCap2 = 20;
    constexpr static int qCap3 = 20;

    cStdDev C1_n1_n2_n3[qCap1][qCap2][qCap3];
    bool isC1_n1_n2_n3[qCap1][qCap2][qCap3];
    simtime_t startOfC1_n1_n2_n3[qCap1][qCap2][qCap3];

    cStdDev C2_n1_n2_n3[qCap1][qCap2][qCap3];
    bool isC2_n1_n2_n3[qCap1][qCap2][qCap3];
    simtime_t startOfC2_n1_n2_n3[qCap1][qCap2][qCap3];

    cStdDev C3_n1_n2_n3[qCap1][qCap2][qCap3];
    bool isC3_n1_n2_n3[qCap1][qCap2][qCap3];
    simtime_t startOfC3_n1_n2_n3[qCap1][qCap2][qCap3];

    cStdDev S1_n1_n2_n3[qCap1][qCap2][qCap3];
    bool isS1_n1_n2_n3[qCap1][qCap2][qCap3];
    simtime_t startOfS1_n1_n2_n3[qCap1][qCap2][qCap3];

    cStdDev S2_n1_n2_n3[qCap1][qCap2][qCap3];
    bool isS2_n1_n2_n3[qCap1][qCap2][qCap3];
    simtime_t startOfS2_n1_n2_n3[qCap1][qCap2][qCap3];

    cStdDev S3_n1_n2_n3[qCap1][qCap2][qCap3];
    bool isS3_n1_n2_n3[qCap1][qCap2][qCap3];
    simtime_t startOfS3_n1_n2_n3[qCap1][qCap2][qCap3];

public:
    MonitorFor3BatchQueues();
    virtual~ MonitorFor3BatchQueues();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(MonitorFor3BatchQueues);

MonitorFor3BatchQueues::MonitorFor3BatchQueues() {
    statsCollectionEvent = nullptr;
}

MonitorFor3BatchQueues::~MonitorFor3BatchQueues() {
    cancelAndDelete(statsCollectionEvent);
}

void MonitorFor3BatchQueues::initialize() {
    numQueues = par("numQueues");
    serverPhase = CONNECTION;
    queueIndex = 0;
    for (int i = 0; i < numQueues; i++)
        n.push_back(0);

    // States with only serverPhase variable
    isC1 = isC2 = isC3 = isS1 = isS2 = isS3 = false;
    startOfC1 = startOfC2 = startOfC3 = startOfS1 = startOfS2 = startOfS3 = 0;

    // States with all variables: serverPhase, n1, n2, n3
    for (int i = 0; i <= qCap1; i++)
        for (int j = 0; j <= qCap2; j++)
            for (int k = 0; k <= qCap3; k++) {
                isC1_n1_n2_n3[i][j][k] = false;
                startOfC1_n1_n2_n3[i][j][k] = 0;

                isC2_n1_n2_n3[i][j][k] = false;
                startOfC2_n1_n2_n3[i][j][k] = 0;

                isC3_n1_n2_n3[i][j][k] = false;
                startOfC3_n1_n2_n3[i][j][k] = 0;

                isS1_n1_n2_n3[i][j][k] = false;
                startOfS1_n1_n2_n3[i][j][k] = 0;

                isS2_n1_n2_n3[i][j][k] = false;
                startOfS2_n1_n2_n3[i][j][k] = 0;

                isS3_n1_n2_n3[i][j][k] = false;
                startOfS3_n1_n2_n3[i][j][k] = 0;
            }

    statsCollectionEvent = new StateMessage("Collecting statistics");
}

void MonitorFor3BatchQueues::finish() {
    // States with only serverPhase variable
    recordScalar("Probability C1", C1.getSum() * 100 / simTime());
    recordScalar("Probability C2", C2.getSum() * 100 / simTime());
    recordScalar("Probability C3", C3.getSum() * 100 / simTime());
    recordScalar("Probability S1", S1.getSum() * 100 / simTime());
    recordScalar("Probability S2", S2.getSum() * 100 / simTime());
    recordScalar("Probability S3", S3.getSum() * 100 / simTime());

    // States with all variables: serverPhase, n1, n2, n3
    for (int i = 0; i <= qCap1; i++)
        for (int j = 0; j <= qCap2; j++)
            for (int k = 0; k <= qCap3; k++) {
                    std::string nameString = "Probability C1_" + std::to_string(i)
                            + "_" + std::to_string(j) + "_" + std::to_string(k);
                    const char *name = nameString.c_str();
                    recordScalar(name, C1_n1_n2_n3[i][j][k].getSum() * 100 / simTime());
                }

    for (int i = 0; i <= qCap1; i++)
        for (int j = 0; j <= qCap2; j++)
            for (int k = 0; k <= qCap3; k++) {
                    std::string nameString = "Probability C2_" + std::to_string(i)
                            + "_" + std::to_string(j) + "_" + std::to_string(k);
                    const char *name = nameString.c_str();
                    recordScalar(name, C2_n1_n2_n3[i][j][k].getSum() * 100 / simTime());
                }

    for (int i = 0; i <= qCap1; i++)
        for (int j = 0; j <= qCap2; j++)
            for (int k = 0; k <= qCap3; k++) {
                    std::string nameString = "Probability C3_" + std::to_string(i)
                            + "_" + std::to_string(j) + "_" + std::to_string(k);
                    const char *name = nameString.c_str();
                    recordScalar(name, C3_n1_n2_n3[i][j][k].getSum() * 100 / simTime());
                }

    for (int i = 0; i <= qCap1; i++)
        for (int j = 0; j <= qCap2; j++)
            for (int k = 0; k <= qCap3; k++) {
                    std::string nameString = "Probability S1_" + std::to_string(i)
                            + "_" + std::to_string(j) + "_" + std::to_string(k);
                    const char *name = nameString.c_str();
                    recordScalar(name, S1_n1_n2_n3[i][j][k].getSum() * 100 / simTime());
                }

    for (int i = 0; i <= qCap1; i++)
        for (int j = 0; j <= qCap2; j++)
            for (int k = 0; k <= qCap3; k++) {
                    std::string nameString = "Probability S2_" + std::to_string(i)
                            + "_" + std::to_string(j) + "_" + std::to_string(k);
                    const char *name = nameString.c_str();
                    recordScalar(name, S2_n1_n2_n3[i][j][k].getSum() * 100 / simTime());
                }

    for (int i = 0; i <= qCap1; i++)
        for (int j = 0; j <= qCap2; j++)
            for (int k = 0; k <= qCap3; k++) {
                    std::string nameString = "Probability S3_" + std::to_string(i)
                            + "_" + std::to_string(j) + "_" + std::to_string(k);
                    const char *name = nameString.c_str();
                    recordScalar(name, S3_n1_n2_n3[i][j][k].getSum() * 100 / simTime());
                }

}

void MonitorFor3BatchQueues::handleMessage(cMessage *msg) {
    StateMessage *stateMsg = check_and_cast<StateMessage *>(msg);

    // msgType = SET_SERVER_PHASE
    if (stateMsg->getMsgType() == SET_SERVER_PHASE) {
        serverPhase = stateMsg->getServerPhase();
        queueIndex = stateMsg->getQueueIndex();
        delete stateMsg;

        EV << serverPhase << "[" <<
                queueIndex+1 << "], " <<
                n[0] << ", " <<
                n[1] << ", " <<
                n[2] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // msgType = SET_N
    else if (stateMsg->getMsgType() == SET_N) {
        n[stateMsg->getQueueIndex()] = stateMsg->getN();
        delete stateMsg;

        EV << serverPhase << "[" <<
                queueIndex+1 << "], " <<
                n[0] << ", " <<
                n[1] << ", " <<
                n[2] << "\n";

        scheduleAt(simTime(), statsCollectionEvent);
    }

    // Stats collection -------------------------------------------------------------------
    else if (stateMsg == statsCollectionEvent) {
        // States with only serverPhase variable ------------------------------------------
        // C1
        if (serverPhase == CONNECTION && queueIndex == 0 && isC1 == false) {
            startOfC1 = simTime();
            isC1 = true;
        }
        else if (!(serverPhase == CONNECTION && queueIndex == 0) && isC1 == true) {
            C1.collect(simTime() - startOfC1);
            isC1 = false;
        }

        // C2
        if (serverPhase == CONNECTION && queueIndex == 1 && isC2 == false) {
            startOfC2 = simTime();
            isC2 = true;
        }
        else if (!(serverPhase == CONNECTION && queueIndex == 1) && isC2 == true) {
            C2.collect(simTime() - startOfC2);
            isC2 = false;
        }

        // C3
        if (serverPhase == CONNECTION && queueIndex == 2 && isC3 == false) {
            startOfC3 = simTime();
            isC3 = true;
        }
        else if (!(serverPhase == CONNECTION && queueIndex == 2) && isC3 == true) {
            C3.collect(simTime() - startOfC3);
            isC3 = false;
        }

        // S1
        if (serverPhase == SERVICE && queueIndex == 0 && isS1 == false) {
            startOfS1 = simTime();
            isS1 = true;
        }
        else if (!(serverPhase == SERVICE && queueIndex == 0) && isS1 == true) {
            S1.collect(simTime() - startOfS1);
            isS1 = false;
        }

        // S2
        if (serverPhase == SERVICE && queueIndex == 1 && isS2 == false) {
            startOfS2 = simTime();
            isS2 = true;
        }
        else if (!(serverPhase == SERVICE && queueIndex == 1) && isS2 == true) {
            S2.collect(simTime() - startOfS2);
            isS2 = false;
        }

        // S3
        if (serverPhase == SERVICE && queueIndex == 2 && isS3 == false) {
            startOfS3 = simTime();
            isS3 = true;
        }
        else if (!(serverPhase == SERVICE && queueIndex == 2) && isS3 == true) {
            S3.collect(simTime() - startOfS3);
            isS3 = false;
        }


        //  States with all variables: serverPhase, n1, n2, n3 --------------------------------
        for (int i = 0; i < qCap1; i++)
            for (int j = 0; j < qCap2; j++)
                for (int k = 0; k < qCap3; k++) {
                    // C1_n1_n2_n3
                    if (serverPhase == CONNECTION &&
                            queueIndex == 0 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k &&
                            isC1_n1_n2_n3[i][j][k] == false) {
                        startOfC1_n1_n2_n3[i][j][k] = simTime();
                        isC1_n1_n2_n3[i][j][k] = true;
                    }
                    else if (!(serverPhase == CONNECTION &&
                            queueIndex == 0 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k) &&
                            isC1_n1_n2_n3[i][j][k] == true) {
                        C1_n1_n2_n3[i][j][k].collect(simTime() - startOfC1_n1_n2_n3[i][j][k]);
                        isC1_n1_n2_n3[i][j][k] = false;
                    }

                    // C2_n1_n2_n3
                    if (serverPhase == CONNECTION &&
                            queueIndex == 1 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k &&
                            isC2_n1_n2_n3[i][j][k] == false) {
                        startOfC2_n1_n2_n3[i][j][k] = simTime();
                        isC2_n1_n2_n3[i][j][k] = true;
                    }
                    else if (!(serverPhase == CONNECTION &&
                            queueIndex == 1 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k) &&
                            isC2_n1_n2_n3[i][j][k] == true) {
                        C2_n1_n2_n3[i][j][k].collect(simTime() - startOfC2_n1_n2_n3[i][j][k]);
                        isC2_n1_n2_n3[i][j][k] = false;
                    }

                    // C3_n1_n2_n3
                    if (serverPhase == CONNECTION &&
                            queueIndex == 2 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k &&
                            isC3_n1_n2_n3[i][j][k] == false) {
                        startOfC3_n1_n2_n3[i][j][k] = simTime();
                        isC3_n1_n2_n3[i][j][k] = true;
                    }
                    else if (!(serverPhase == CONNECTION &&
                            queueIndex == 2 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k) &&
                            isC3_n1_n2_n3[i][j][k] == true) {
                        C3_n1_n2_n3[i][j][k].collect(simTime() - startOfC3_n1_n2_n3[i][j][k]);
                        isC3_n1_n2_n3[i][j][k] = false;
                    }

                    // S1_n1_n2_n3
                    if (serverPhase == SERVICE &&
                            queueIndex == 0 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k &&
                            isS1_n1_n2_n3[i][j][k] == false) {
                        startOfS1_n1_n2_n3[i][j][k] = simTime();
                        isS1_n1_n2_n3[i][j][k] = true;
                    }
                    else if (!(serverPhase == SERVICE &&
                            queueIndex == 0 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k) &&
                            isS1_n1_n2_n3[i][j][k] == true) {
                        S1_n1_n2_n3[i][j][k].collect(simTime() - startOfS1_n1_n2_n3[i][j][k]);
                        isS1_n1_n2_n3[i][j][k] = false;
                    }

                    // S2_n1_n2_n3
                    if (serverPhase == SERVICE &&
                            queueIndex == 1 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k &&
                            isS2_n1_n2_n3[i][j][k] == false) {
                        startOfS2_n1_n2_n3[i][j][k] = simTime();
                        isS2_n1_n2_n3[i][j][k] = true;
                    }
                    else if (!(serverPhase == SERVICE &&
                            queueIndex == 1 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k) &&
                            isS2_n1_n2_n3[i][j][k] == true) {
                        S2_n1_n2_n3[i][j][k].collect(simTime() - startOfS2_n1_n2_n3[i][j][k]);
                        isS2_n1_n2_n3[i][j][k] = false;
                    }

                    // S3_n1_n2_n3
                    if (serverPhase == SERVICE &&
                            queueIndex == 2 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k &&
                            isS3_n1_n2_n3[i][j][k] == false) {
                        startOfS3_n1_n2_n3[i][j][k] = simTime();
                        isS3_n1_n2_n3[i][j][k] = true;
                    }
                    else if (!(serverPhase == SERVICE &&
                            queueIndex == 2 &&
                            n[0] == i &&
                            n[1] == j &&
                            n[2] == k) &&
                            isS3_n1_n2_n3[i][j][k] == true) {
                        S3_n1_n2_n3[i][j][k].collect(simTime() - startOfS3_n1_n2_n3[i][j][k]);
                        isS3_n1_n2_n3[i][j][k] = false;
                    }


                }
    }
}
