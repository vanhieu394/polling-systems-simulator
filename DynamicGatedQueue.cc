#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class DynamicGatedQueue : public cSimpleModule {
private:
    cQueue buffer;                      // Buffer to save all packets
    int queueLen;                       // Current queue length
    int servLen;                        // The number of packets that the server have to service at this cycle
    int ownIndex;                       // Queue's index
    long int cycleNumber;               // Current cycle number

    simtime_t pollingMoment;            // The last polling moment of this queue
    simtime_t leavingMoment;            // The moment when server completes serving this queue and leaves it

    cMessage *takePacketEvent;          // Take packet out of the buffer
    cMessage *packetServiceEvent;       // Take the packet out of the buffer and service it
    cMessage *pkt;                      // Current servicing packet

    cStdDev waitingTime;                // The waiting time sequence of all packets
    cStdDev sojTime;                    // The sojourn time sequence of all packets
    cStdDev intervisitTime;             // The intervisitTime sequence of all packets
    cStdDev cycleTime;                  // The cycle time sequence

public:
    DynamicGatedQueue();
    virtual~ DynamicGatedQueue();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(DynamicGatedQueue);

DynamicGatedQueue::DynamicGatedQueue() {
    takePacketEvent = nullptr;
    packetServiceEvent = nullptr;
    pkt = nullptr;
}

DynamicGatedQueue::~DynamicGatedQueue() {
    cancelAndDelete(takePacketEvent);
    cancelAndDelete(packetServiceEvent);
}

void DynamicGatedQueue::initialize() {
    buffer.setName("buffer");
    queueLen = 0;
    servLen = 0;
    ownIndex = par("ownIndex");
    cycleNumber = 0;

    pollingMoment = 0;
    leavingMoment = 0;

    takePacketEvent = new cMessage("Taking packet out of the buffer");
    packetServiceEvent = new cMessage("A packet is serviced");
}

void DynamicGatedQueue::finish() {
    std::string waitingTimeNameString = "Mean waiting time in DynamicGatedQueue[" + std::to_string(ownIndex) + "]";
    const char *waitingTimeName = waitingTimeNameString.c_str();
    recordScalar(waitingTimeName, waitingTime.getMean());

    std::string sojTimeNameString = "Mean sojourn time in DynamicGatedQueue[" + std::to_string(ownIndex) + "]";
    const char *sojTimeName = sojTimeNameString.c_str();
    recordScalar(sojTimeName, sojTime.getMean());

    std::string cycleTimeNameString = "Mean cycle time in DynamicGatedQueue[" + std::to_string(ownIndex) + "]";
    const char *cycleTimeName = cycleTimeNameString.c_str();
    recordScalar(cycleTimeName, cycleTime.getMean());

    std::string intervisitTimeNameString = "Mean intervisit time in DynamicGatedQueue[" + std::to_string(ownIndex) + "]";
    const char *intervisitTimeName = intervisitTimeNameString.c_str();
    recordScalar(intervisitTimeName, intervisitTime.getMean());

    std::string numCycleNameString = "Number of cycle in DynamicGatedQueue[" + std::to_string(ownIndex) + "]";
    const char *numCycleName = numCycleNameString.c_str();
    recordScalar(numCycleName, cycleTime.getCount());
}

void DynamicGatedQueue::handleMessage(cMessage *msg) {
    // Receive "Polling" message from the server
    if (msg->arrivedOn("server$i")) {
        // Collect the statistics from cycle 2
        cycleNumber++;
        if (cycleNumber > 1) {
            intervisitTime.collect(simTime() - leavingMoment);
            cycleTime.collect(simTime() - pollingMoment);
        }

        pollingMoment = simTime();
        delete msg;

//        // Print out the stats
//        EV << "Mean cycle time in Q[" << ownIndex << "] = "
//                << cycleTime.getMean() << "\n";
//        EV << "Mean intervisit time in Q[" << ownIndex << "] = "
//                << intervisitTime.getMean() << "\n";
//        EV << "Current cycle in Q[" << ownIndex << "] = "
//                << cycleNumber << "\n";

        queueLen = buffer.getLength();
        servLen = buffer.getLength();
        if (queueLen == 0) {
            // Send "Queue is empty" msg to the server
            send(new cMessage("Queue is empty", ownIndex), "server$o");
            leavingMoment = simTime();
        }
        else {
            // Update system state
            StateMessage *stateMsg = new StateMessage("SERVICE phase");
            stateMsg->setMsgType(SET_SERVER_PHASE);
            stateMsg->setServerPhase(SERVICE);
            stateMsg->setQueueIndex(ownIndex);
            send(stateMsg, "toMonitor");

            // Start servicing
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
        send(pkt, "server$o");
        queueLen--;
        servLen--;

        // Send msg to the monitor to update system state
        StateMessage *stateMsg = new StateMessage("Set n[i]");
        stateMsg->setMsgType(SET_N);
        stateMsg->setQueueIndex(ownIndex);
        stateMsg->setN(queueLen);
        send(stateMsg, "toMonitor");

        if (servLen == 0) {
            send(new cMessage("Queue is serviced", ownIndex), "server$o");
            leavingMoment = simTime();
        }
        else {
            scheduleAt(simTime(), takePacketEvent);
        }

    }

    // Insert packets from the generator to the buffer
    else {
        buffer.insert(msg);
        queueLen++;

        // Send msg to the monitor to update system state
        StateMessage *stateMsg = new StateMessage("Set n[i]");
        stateMsg->setMsgType(SET_N);
        stateMsg->setQueueIndex(ownIndex);
        stateMsg->setN(queueLen);
        send(stateMsg, "toMonitor");
    }
}
