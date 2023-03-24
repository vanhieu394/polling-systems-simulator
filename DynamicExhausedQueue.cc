#include <omnetpp.h>
using namespace omnetpp;

//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class DynamicExhausedQueue : public cSimpleModule {
private:
    cQueue buffer;                      // Buffer to save all packets
    int n;                              // Buffer's current length
    int ownIndex;                       // Queue's index

    simtime_t currWaitingTime;          // The waiting time of the current packet
    simtime_t currSojTime;              // The sojourn time of the current packet
    simtime_t currIntervisitTime;       // The current intervisit time
    simtime_t curCycleTime;             // The duration of the current cycle
    simtime_t pollingMoment;            // The last polling moment of this queue
    simtime_t leavingMoment;            // The moment when server completes serving this queue and leaves it

    cMessage *emptyQueueEvent;          // Queue is empty at the polling moment
    cMessage *packetServiceEvent;       // Take the packet out of the buffer and service it
    cMessage *checkQueueEvent;          // After serving the last packet, check if queue is empty
    cMessage *queueIsServicedEvent;     // After serving the entire queue, send this to the server

    cStdDev waitingTime;                // The waiting time sequence of all packets
    cStdDev sojTime;                    // The sojourn time sequence of all packets
    cStdDev intervisitTime;             // The intervisitTime sequence of all packets
    cStdDev cycleTime;                  // The cycle time sequence

public:
    DynamicExhausedQueue();
    virtual~ DynamicExhausedQueue();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(DynamicExhausedQueue);

DynamicExhausedQueue::DynamicExhausedQueue(){
    emptyQueueEvent = nullptr;
    packetServiceEvent = nullptr;
    checkQueueEvent = nullptr;
    queueIsServicedEvent = nullptr;
}

DynamicExhausedQueue::~DynamicExhausedQueue(){
    cancelAndDelete(emptyQueueEvent);
    cancelAndDelete(packetServiceEvent);
    cancelAndDelete(checkQueueEvent);
    cancelAndDelete(queueIsServicedEvent);
}

void DynamicExhausedQueue::initialize(){
    n = 0;
    ownIndex = par("ownIndex");

    currWaitingTime = 0;
    currSojTime = 0;
    currIntervisitTime = 0;
    curCycleTime = 0;
    pollingMoment = 0;
    leavingMoment = 0;

    emptyQueueEvent = new cMessage("Queue is empty");
    packetServiceEvent = new cMessage("Servicing packet");
    checkQueueEvent = new cMessage("Checking queue");
    queueIsServicedEvent = new cMessage("Queue is serviced");
}

void DynamicExhausedQueue::finish(){
    recordScalar("Mean waiting time in DynamicExhausedQueue", waitingTime.getMean());
    recordScalar("Mean sojourn time in DynamicExhausedQueue", sojTime.getMean());
    recordScalar("Mean cycle time in DynamicExhausedQueue", cycleTime.getMean());
    recordScalar("Mean intervisit time in DynamicExhausedQueue", intervisitTime.getMean());
    recordScalar("Number of cycle in DynamicExhausedQueue", intervisitTime.getCount()-1);
}

void DynamicExhausedQueue::handleMessage(cMessage *msg){
    cMessage *pkt;
    // Receive "Server is ready" message from the server
    if(msg->arrivedOn("out$i")) {
        // Collect the statistics
        currIntervisitTime = simTime() - leavingMoment;
        intervisitTime.collect(currIntervisitTime);
        curCycleTime = simTime() - pollingMoment;
        cycleTime.collect(curCycleTime);
        pollingMoment = simTime();
        delete msg;
        n = buffer.getLength();

        if (n == 0) {
            scheduleAt(simTime(), emptyQueueEvent);
        }
        else {
            scheduleAt(simTime(), packetServiceEvent);
        }

    }

    // If the queue is empty at the polling moment, send a message to sever that queue is empty
    else if (msg == emptyQueueEvent) {
        send(new cMessage("Queue is empty", ownIndex), "out$o");
        leavingMoment = simTime();
    }

    // Take the packet out of the buffer and service it
    else if (msg == packetServiceEvent) {
        pkt = (cMessage*)buffer.pop();
        double currServiceTime = par("serviceTime");

        // Collect the the statistics
        currWaitingTime = simTime() - pkt->getCreationTime();
        waitingTime.collect(currWaitingTime);
        currSojTime = simTime() + currServiceTime - pkt->getCreationTime();
        sojTime.collect(currSojTime);

        // Send out the packet to the server
        send(pkt,"out$o");
        n--;

        if (n == 0) {
            scheduleAt(simTime(), checkQueueEvent);
        }
        else {
            scheduleAt(simTime() + currServiceTime, packetServiceEvent);
        }
    }

    // Check the queue if it have packets
    else if (msg == checkQueueEvent) {
        n = buffer.getLength();
        if (n == 0) {
             scheduleAt(simTime(), queueIsServicedEvent);
        }
        else {
            scheduleAt(simTime(), packetServiceEvent);
        }
    }

    // Queue is serviced
    else if (msg == queueIsServicedEvent) {
        send(new cMessage("Queue is serviced", ownIndex), "out$o");
        leavingMoment = simTime();
    }

    // Insert packets from the generator to the buffer
    else {
        buffer.insert(msg);
    }
}
