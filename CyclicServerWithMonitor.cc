#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

//using namespace std;
//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class CyclicServerWithMonitor : public cSimpleModule {
private:
    int numQueues;                             // The number of the queues
    int gateInId;                              // Index of the input gates

    std::vector<double> arrayDouble;           // Vector of switchoverTime

    simtime_t startOfCycle;                    // The start of cycle moment
    simtime_t currCycleTime;                   // The duration of the current cycle

    cMessage *startCycleEvent;                 // Start a new cycle
    cMessage *switchToQueueEvent;              // Server switches to the queue
    cMessage *stopCycleEvent;                  // End of cycle

    std::string empty = "Queue is empty";      // string to compare with msg from the queue
    std::string served = "Queue has been served";    // string to compare with msg from the queue

    cStdDev cycleTime;                         // The cycle time sequence

public:
    CyclicServerWithMonitor();
    virtual~ CyclicServerWithMonitor();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(CyclicServerWithMonitor);

CyclicServerWithMonitor::CyclicServerWithMonitor() {
    startCycleEvent = nullptr;
    switchToQueueEvent = nullptr;
    stopCycleEvent = nullptr;
}
CyclicServerWithMonitor::~CyclicServerWithMonitor(){
    cancelAndDelete(startCycleEvent);
    cancelAndDelete(switchToQueueEvent);
    cancelAndDelete(stopCycleEvent);
}
void CyclicServerWithMonitor::initialize() {
    numQueues = par("numQueues");
    gateInId = 0;

    const char* arrayString = par("switchoverTime").stringValue();
    arrayDouble = cStringTokenizer(arrayString).asDoubleVector();

    startOfCycle = 0;
    currCycleTime = 0;

    startCycleEvent = new cMessage("New cycle is started");
    switchToQueueEvent = new cMessage("Switched to the queue");
    stopCycleEvent = new cMessage("End of cycle");

    // Start a new cycle
    scheduleAt(simTime(), startCycleEvent);
}
void CyclicServerWithMonitor::finish() {
    recordScalar("Mean cycle time in the server", cycleTime.getMean());
    recordScalar("Number of cycle", cycleTime.getCount());
}
void CyclicServerWithMonitor::handleMessage(cMessage *msg) {
    // Start a new cycle
    if (msg == startCycleEvent) {
        startOfCycle = simTime();

        EV << "Start of cycle " << cycleTime.getCount()+1 << "\n";

        // Switch to the first queue
        // Update system state
        StateMessage *stateMsg = new StateMessage("CONNECTION phase");
        stateMsg->setMsgType(SET_SERVER_PHASE);
        stateMsg->setServerPhase(CONNECTION);
        stateMsg->setQueueIndex(gateInId);
        send(stateMsg, "toMonitor");

        double switchoverTime = exponential(arrayDouble[gateInId]);
        scheduleAt(simTime() + switchoverTime, switchToQueueEvent);
    }

    // Poll the queue
    else if (msg == switchToQueueEvent)
        send(new cMessage("Polling"), "queue$o", gateInId);

    // If the queue is empty or fully serviced, jump to the next one
    else if (msg->getFullName() == empty || msg->getFullName() == served) {
        gateInId = (msg->getKind() + 1) % numQueues;
        delete(msg);

        // Check if cycle is ended and switch to the next queue
        if(gateInId == 0)
            scheduleAt(simTime(), stopCycleEvent);
        else {
            // Update system state
            StateMessage *stateMsg = new StateMessage("CONNECTION phase");
            stateMsg->setMsgType(SET_SERVER_PHASE);
            stateMsg->setServerPhase(CONNECTION);
            stateMsg->setQueueIndex(gateInId);
            send(stateMsg, "toMonitor");

            double switchoverTime = exponential(arrayDouble[gateInId]);
            scheduleAt(simTime() + switchoverTime, switchToQueueEvent);
        }
    }

    // End of cycle
    else if(msg == stopCycleEvent) {
        // Collect the statistics
        currCycleTime = simTime() - startOfCycle;
        cycleTime.collect(currCycleTime);

        EV << "End of cycle " << cycleTime.getCount() << "\n";

        scheduleAt(simTime(), startCycleEvent);
    }

    // Send out the serviced packets to the sink immediately
    // because they were serviced in the queue module
    else
        send(msg, "toSink");
}


