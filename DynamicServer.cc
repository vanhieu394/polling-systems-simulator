#include <omnetpp.h>
using namespace omnetpp;

//using namespace std;
//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class DynamicServer : public cSimpleModule {
private:
    int numQueues;                             // The number of the queues
    int gateInId;                              // Index of the input gates
    std::vector<int> q;                        // Will poll i-queue if q[i] = 1 and skip i-queue if q[i] = 0
    int sum;                                   // Sum of all q[i] values
    std::vector<double> arrayDouble;           // Vector of switchingTime

    simtime_t beginOfCycle;                    // The begin of cycle moment
    simtime_t currCycleTime;                   // The duration of the current cycle

    cMessage *startCycleEvent;                 // Start a new cycle
    cMessage *serviceEvent;                    // Start servicing
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

DynamicServer::DynamicServer(){
    startCycleEvent = nullptr;
    serviceEvent = nullptr;
    switchToQueueEvent = nullptr;
    stopCycleEvent = nullptr;
}
DynamicServer::~DynamicServer(){
    cancelAndDelete(startCycleEvent);
    cancelAndDelete(serviceEvent);
    cancelAndDelete(switchToQueueEvent);
    cancelAndDelete(stopCycleEvent);
}
void DynamicServer::initialize(){
    numQueues = par("numQueues");
    gateInId = 0;
    sum = 0;

    for (int i = 0; i < numQueues; i++)
        q.push_back(1);
    const char* arrayString = par("switchingTime").stringValue();
    arrayDouble = cStringTokenizer(arrayString).asDoubleVector();

    beginOfCycle = 0;
    currCycleTime = 0;

    startCycleEvent = new cMessage("Starting a new cycle");
    serviceEvent = new cMessage("Starting service");
    switchToQueueEvent = new cMessage("Switching to the queue");
    stopCycleEvent = new cMessage("End of Cycle");

    scheduleAt(simTime(), startCycleEvent);
}
void DynamicServer::finish(){
    recordScalar("Mean cycle time in the server", cycleTime.getMean());
    recordScalar("Number of cycle", cycleTime.getCount());
}
void DynamicServer::handleMessage(cMessage *msg){
    // Start a new cycle
    if (msg == startCycleEvent) {
        beginOfCycle = simTime();
        sum = 0;
        for (int i = 0; i < numQueues; i++)
            sum += q[i];

        EV << "Begin of cycle " << cycleTime.getCount()+1 << "\n";

        // Check if all queues are empty
        if (sum == 0) {
            for (int i = 0; i < numQueues; i++)
                q[i] = 1;
            scheduleAt(simTime() + par("restTime"), stopCycleEvent);
        }
        else
            scheduleAt(simTime(), serviceEvent);
    }

    // End of cycle
    else if(msg == stopCycleEvent) {
        // Collect the statistics
        currCycleTime = simTime() - beginOfCycle;
        cycleTime.collect(currCycleTime);

        EV << "End of cycle " << cycleTime.getCount() << "\n";

        scheduleAt(simTime(), startCycleEvent);
    }

    // Start servicing
    else if (msg == serviceEvent) {
        // If q[i] = 1, switch to this queue to service it
        if (q[gateInId] == 1) {
            double switchingTime = exponential(arrayDouble[gateInId]);
            scheduleAt(simTime() + switchingTime, switchToQueueEvent);
        }

        // If q[i] = 0, set it to 1, then skip this queue
        else {
            q[gateInId] = 1;
            gateInId = (gateInId + 1) % numQueues;
            if(gateInId == 0)
                scheduleAt(simTime(), stopCycleEvent);
            else
                scheduleAt(simTime(), serviceEvent);
        }
    }

    // Switch to the queue
    else if (msg == switchToQueueEvent)
        send(new cMessage("Polling"), "in$o", gateInId);

    // If the queue is empty
    else if (msg->getFullName() == empty) {
        q[msg->getKind()] = 0;
        gateInId = (msg->getKind() + 1) % numQueues;
        delete(msg);

        if(gateInId == 0)
            scheduleAt(simTime(), stopCycleEvent);
        else
            scheduleAt(simTime(), serviceEvent);
    }

    // If the queue is fully serviced, switch to another queue
    else if (msg->getFullName() == done) {
        gateInId = (msg->getKind() + 1) % numQueues;
        delete(msg);

        if(gateInId == 0)
            scheduleAt(simTime(), stopCycleEvent);
        else
            scheduleAt(simTime(), serviceEvent);
    }

    // Send out the serviced packets to the sink immediately
    // because they were serviced in the queue module
    else
        send(msg,"out");
}


