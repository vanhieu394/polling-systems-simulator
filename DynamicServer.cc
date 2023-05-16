#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

//using namespace std;
//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class DynamicServer : public cSimpleModule {
private:
    int numQueues;                             // The number of the queues
    int gateInId;                              // Index of the input gates
    int sum;                                   // Sum of all q[i] values

    std::vector<int> q;                        // Will poll i-queue if q[i] = 1 and skip i-queue if q[i] = 0
    std::vector<double> arrayDouble;           // Vector of switchoverTime

    simtime_t startOfCycle;                    // The start of cycle moment
    simtime_t currCycleTime;                   // The duration of the current cycle

    cMessage *startCycleEvent;                 // Start a new cycle
    cMessage *checkQueueFlagEvent;             // Check queue's flag
    cMessage *switchToQueueEvent;              // Server switches to the queue
    cMessage *stopCycleEvent;                  // End of cycle

    std::string empty = "Queue is empty";      // string to compare with msg from the queue
    std::string done = "Queue is serviced";    // string to compare with msg from the queue

    cStdDev cycleTime;                         // The cycle time sequence

public:
    DynamicServer();
    virtual~ DynamicServer();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(DynamicServer);

DynamicServer::DynamicServer() {
    startCycleEvent = nullptr;
    checkQueueFlagEvent = nullptr;
    switchToQueueEvent = nullptr;
    stopCycleEvent = nullptr;
}
DynamicServer::~DynamicServer(){
    cancelAndDelete(startCycleEvent);
    cancelAndDelete(checkQueueFlagEvent);
    cancelAndDelete(switchToQueueEvent);
    cancelAndDelete(stopCycleEvent);
}
void DynamicServer::initialize() {
    numQueues = par("numQueues");
    gateInId = 0;
    sum = 0;

    for (int i = 0; i < numQueues; i++)
        q.push_back(1);
    const char* arrayString = par("switchoverTime").stringValue();
    arrayDouble = cStringTokenizer(arrayString).asDoubleVector();

    startOfCycle = 0;
    currCycleTime = 0;

    startCycleEvent = new cMessage("New cycle is started");
    checkQueueFlagEvent = new cMessage("Checking queue's flag");
    switchToQueueEvent = new cMessage("Switched to the queue");
    stopCycleEvent = new cMessage("End of cycle");

    scheduleAt(simTime(), startCycleEvent);
}
void DynamicServer::finish() {
    recordScalar("Mean cycle time in the server", cycleTime.getMean());
    recordScalar("Number of cycle", cycleTime.getCount());
}
void DynamicServer::handleMessage(cMessage *msg) {
    // Start a new cycle
    if (msg == startCycleEvent) {
        startOfCycle = simTime();
        sum = 0;
        for (int i = 0; i < numQueues; i++)
            sum += q[i];

        EV << "Start of cycle " << cycleTime.getCount()+1 << "\n";

        // Check if all queues are empty
        if (sum == 0) {
            // Jump to idling phase and reset all q[i] to 1
            StateMessage *stateMsg = new StateMessage("IDLING phase");
            stateMsg->setMsgType(SET_SERVER_PHASE);
            stateMsg->setServerPhase(IDLING);
            send(stateMsg, "toMonitor");

            for (int i = 0; i < numQueues; i++)
                q[i] = 1;

            scheduleAt(simTime() + par("vacationTime"), stopCycleEvent);
        }
        else
            scheduleAt(simTime(), checkQueueFlagEvent);
    }

    // End of cycle
    else if(msg == stopCycleEvent) {
        // Collect the statistics
        currCycleTime = simTime() - startOfCycle;
        cycleTime.collect(currCycleTime);

        EV << "End of cycle " << cycleTime.getCount() << "\n";

        scheduleAt(simTime(), startCycleEvent);
    }

    // Checking queue's flag
    else if (msg == checkQueueFlagEvent) {
        if (q[gateInId] == 1) {
            // Update system state
            StateMessage *stateMsg = new StateMessage("CONNECTING phase");
            stateMsg->setMsgType(SET_SERVER_PHASE);
            stateMsg->setServerPhase(CONNECTING);
            stateMsg->setQueueIndex(gateInId);
            send(stateMsg, "toMonitor");

            // Switch to this queue
            double switchoverTime = exponential(arrayDouble[gateInId]);
            scheduleAt(simTime() + switchoverTime, switchToQueueEvent);
        }

        // If q[i] = 0, reset it to 1, then skip this queue
        else {
            q[gateInId] = 1;

            // Update system state
            StateMessage *stateMsg = new StateMessage("Set q[i] = 1");
            stateMsg->setMsgType(SET_Q);
            stateMsg->setQueueIndex(gateInId);
            stateMsg->setQ(1);
            send(stateMsg, "toMonitor");

            gateInId = (gateInId + 1) % numQueues;
            if(gateInId == 0)
                scheduleAt(simTime(), stopCycleEvent);
            else
                scheduleAt(simTime(), checkQueueFlagEvent);
        }
    }

    // Poll the queue
    else if (msg == switchToQueueEvent)
        send(new cMessage("Polling"), "queue$o", gateInId);

    // If the queue is empty, set q[i] = 0 and jump to the next one
    else if (msg->getFullName() == empty) {
        q[msg->getKind()] = 0;

        // Update system state
        StateMessage *stateMsg = new StateMessage("Set q[i] = 0");
        stateMsg->setMsgType(SET_Q);
        stateMsg->setQueueIndex(msg->getKind());
        stateMsg->setQ(0);
        send(stateMsg, "toMonitor");

        gateInId = (msg->getKind() + 1) % numQueues;
        delete(msg);

        if(gateInId == 0)
            scheduleAt(simTime(), stopCycleEvent);
        else
            scheduleAt(simTime(), checkQueueFlagEvent);
    }

    // If the queue is fully serviced, switch to another queue
    else if (msg->getFullName() == done) {
        gateInId = (msg->getKind() + 1) % numQueues;
        delete(msg);

        if(gateInId == 0)
            scheduleAt(simTime(), stopCycleEvent);
        else
            scheduleAt(simTime(), checkQueueFlagEvent);
    }

    // Send out the serviced packets to the sink immediately
    // because they were serviced in the queue module
    else
        send(msg, "toSink");
}


