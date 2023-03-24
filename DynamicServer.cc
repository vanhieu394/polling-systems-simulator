#include <omnetpp.h>
using namespace omnetpp;

//using namespace std;
//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class DynamicServer : public cSimpleModule {
private:
    int numQueues;                             // The number of the queues
    int gateInId;                              // Index of the input gates
    long int cycleNumber;                      // Total cycle number in the server
    std::vector<int> a;                             // Will poll i-queue if a[i] = 1 and skip i-queue if a[i] = 0
    std::vector<double> arrayDouble;                // Vector of switchingTime

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
    cycleNumber = 0;

    for (int i = 0; i < numQueues; i++)
        a.push_back(1);
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
    recordScalar("Number of cycle", cycleTime.getCount()-1);
}
void DynamicServer::handleMessage(cMessage *msg){
    // Start a new cycle
    if (msg == startCycleEvent) {
        beginOfCycle = simTime();
        int sum = 0;
        for (int i = 0; i < numQueues; i++)
            sum += a[i];

        // If all queues are empty,
        // reset a[i] to 1 for all queues to serve them all in the next cycle
        // after that, server will take a rest
        if (sum == 0) {
            for (int i = 0; i < numQueues; i++)
                a[i] = 1;
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
        cycleNumber++;
        scheduleAt(simTime(), startCycleEvent);
    }

    // Start servicing
    else if (msg == serviceEvent) {
        // If a[i] = 1, switch to this queue to service it
        if (a[gateInId] == 1) {
            double switchingTime = exponential(arrayDouble[gateInId]);
            scheduleAt(simTime() + switchingTime, switchToQueueEvent);
        }

        // If a[i] = 0, set it to 1, then skip this queue
        else {
            a[gateInId] = 1;
            gateInId = (gateInId + 1) % numQueues;
            scheduleAt(simTime(), serviceEvent);
        }
    }

    // Switch to the queue
    else if (msg == switchToQueueEvent) {
        send(new cMessage("Server is ready"), "in$o", gateInId);
    }

    // If the queue is empty
    else if (msg->getFullName() == empty) {
        a[msg->getKind()] = 0;
        gateInId = (msg->getKind() + 1) % numQueues;
        delete(msg);
        if(gateInId == 0) {
            scheduleAt(simTime(), stopCycleEvent);
        }
        else {
            scheduleAt(simTime(), serviceEvent);
        }
    }

    // If the queue is fully serviced, switch to another queue
    else if (msg->getFullName() == done) {
        gateInId = (msg->getKind() + 1) % numQueues;
        delete(msg);
        if(gateInId == 0) {
            scheduleAt(simTime(), stopCycleEvent);
        }
        else {
            scheduleAt(simTime(), serviceEvent);
        }
    }

    // Send out the serviced packets to the sink immediately
    // because they were serviced in the queue module
    else {
        send(msg,"out");
    }
}

