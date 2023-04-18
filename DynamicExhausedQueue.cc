#include <omnetpp.h>
using namespace omnetpp;

//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class DynamicExhausedQueue : public cSimpleModule {
private:
    cQueue buffer;                      // Buffer to save all packets
    int n;                              // Buffer's current length
    int ownIndex;                       // Queue's index

    simtime_t pollingMoment;            // The last polling moment of this queue
    simtime_t leavingMoment;            // The moment when server completes serving this queue and leaves it

    cMessage *takePacketEvent;          // Take packet out of the buffer
    cMessage *packetServiceEvent;       // Take the packet out of the buffer and service it
    cMessage *checkQueueEvent;          // After serving the last packet, check if queue is empty
    cMessage *pkt;                      // Current servicing packet

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
    takePacketEvent = nullptr;
    packetServiceEvent = nullptr;
    checkQueueEvent = nullptr;
    pkt = nullptr;
}

DynamicExhausedQueue::~DynamicExhausedQueue(){
    cancelAndDelete(takePacketEvent);
    cancelAndDelete(packetServiceEvent);
    cancelAndDelete(checkQueueEvent);
}

void DynamicExhausedQueue::initialize(){
    n = 0;
    ownIndex = par("ownIndex");

    pollingMoment = 0;
    leavingMoment = 0;

    takePacketEvent = new cMessage("Taking packet out of the buffer");
    packetServiceEvent = new cMessage("Servicing packet");
    checkQueueEvent = new cMessage("Checking queue");
}

void DynamicExhausedQueue::finish(){
    recordScalar("Mean waiting time in DynamicExhausedQueue", waitingTime.getMean());
    recordScalar("Mean sojourn time in DynamicExhausedQueue", sojTime.getMean());
    recordScalar("Mean cycle time in DynamicExhausedQueue", cycleTime.getMean());
    recordScalar("Mean intervisit time in DynamicExhausedQueue", intervisitTime.getMean());
    recordScalar("Number of cycle in DynamicExhausedQueue", cycleTime.getCount()-1);
}

void DynamicExhausedQueue::handleMessage(cMessage *msg){
    // Receive "Polling" message from the server
    if(msg->arrivedOn("out$i")) {
        // Collect the statistics
        intervisitTime.collect(simTime() - leavingMoment);
        cycleTime.collect(simTime() - pollingMoment);
        pollingMoment = simTime();
        delete msg;

        n = buffer.getLength();
        if (n == 0) {
            // Send "Queue is empty" msg to the server
            send(new cMessage("Queue is empty", ownIndex), "out$o");
            leavingMoment = simTime();
        }
        else {
            scheduleAt(simTime(), takePacketEvent);
        }
    }

    // Take a packet out of the buffer
    else if (msg == takePacketEvent) {
        pkt = (cMessage*)buffer.pop();

        // Collect the the statistics
        waitingTime.collect(simTime() - pkt->getCreationTime());

        scheduleAt(simTime() + par("serviceTime"), packetServiceEvent);
    }

    // Service the packet
    else if (msg == packetServiceEvent) {
        // Collect the the statistics
        sojTime.collect(simTime() - pkt->getCreationTime());

        // Send out the packet to the server
        send(pkt,"out$o");
        n--;

        if (n == 0) {
            scheduleAt(simTime(), checkQueueEvent);
        }
        else {
            scheduleAt(simTime(), takePacketEvent);
        }

    }

    // Check the queue if it have packets
    else if (msg == checkQueueEvent) {
        n = buffer.getLength();
        if (n == 0) {
            send(new cMessage("Queue is serviced", ownIndex), "out$o");
            leavingMoment = simTime();
        }
        else {
            scheduleAt(simTime(), takePacketEvent);
        }
    }

    // Insert packets from the generator to the buffer
    else {
        buffer.insert(msg);
    }
}
