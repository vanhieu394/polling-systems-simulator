#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class CyclicExhaustiveBatchQueueWithMonitor : public cSimpleModule {
private:
    int ownIndex;                       // Queue's index
    cQueue buffer;                      // Buffer to save all packets
    double queueCapacity;               // Maximum space of the queue
    double queueLen;                    // Current queue length
    int batchSize;                      // Batch size of this queue
    int sizeOfCurrBatch;                // Size of the current batch
    long int cycleNumber;               // Current cycle number

    simtime_t pollingMoment;            // The last polling moment of this queue
    simtime_t leavingMoment;            // The moment when server completes serving this queue and leaves it

    cMessage *takeBatchEvent;           // Take a batch of packet out of the buffer
    cMessage *batchServDoneEvent;       // A batch of packet has been served
    std::vector<cMessage *> batch;      // Current servicing batch

    cStdDev queueLenAtPollingMoment;    // Queue length at its polling moment
    cStdDev waitingTime;                // The waiting time sequence of all packets
    cStdDev sojTime;                    // The sojourn time sequence of all packets
    cStdDev intervisitTime;             // The intervisitTime sequence of all packets
    cStdDev cycleTime;                  // The cycle time sequence

public:
    CyclicExhaustiveBatchQueueWithMonitor();
    virtual~ CyclicExhaustiveBatchQueueWithMonitor();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(CyclicExhaustiveBatchQueueWithMonitor);

CyclicExhaustiveBatchQueueWithMonitor::CyclicExhaustiveBatchQueueWithMonitor() {
    takeBatchEvent = nullptr;
    batchServDoneEvent = nullptr;
}

CyclicExhaustiveBatchQueueWithMonitor::~CyclicExhaustiveBatchQueueWithMonitor() {
    cancelAndDelete(takeBatchEvent);
    cancelAndDelete(batchServDoneEvent);
}

void CyclicExhaustiveBatchQueueWithMonitor::initialize() {
    ownIndex = par("ownIndex");
    buffer.setName("buffer");
    queueCapacity = par("queueCapacity");
    queueLen = 0;
    batchSize = par("batchSize");
    sizeOfCurrBatch = 0;
    cycleNumber = 0;

    pollingMoment = 0;
    leavingMoment = 0;

    takeBatchEvent = new cMessage("Taking a batch of packet out of the buffer");
    batchServDoneEvent = new cMessage("A batch of packet has been served");

    for (int i = 0; i < batchSize; i++)
        batch.push_back(nullptr);
}

void CyclicExhaustiveBatchQueueWithMonitor::finish() {
    std::string queueLenAtPollingMomentNameString = "Mean queue length at its polling moment in queue[" + std::to_string(ownIndex) + "]";
    const char *queueLenAtPollingMomentName = queueLenAtPollingMomentNameString.c_str();
    recordScalar(queueLenAtPollingMomentName, queueLenAtPollingMoment.getMean());

    std::string waitingTimeNameString = "Mean waiting time in queue[" + std::to_string(ownIndex) + "]";
    const char *waitingTimeName = waitingTimeNameString.c_str();
    recordScalar(waitingTimeName, waitingTime.getMean());

    std::string sojTimeNameString = "Mean sojourn time in queue[" + std::to_string(ownIndex) + "]";
    const char *sojTimeName = sojTimeNameString.c_str();
    recordScalar(sojTimeName, sojTime.getMean());

    std::string cycleTimeNameString = "Mean cycle time in queue[" + std::to_string(ownIndex) + "]";
    const char *cycleTimeName = cycleTimeNameString.c_str();
    recordScalar(cycleTimeName, cycleTime.getMean());

    std::string intervisitTimeNameString = "Mean intervisit time in queue[" + std::to_string(ownIndex) + "]";
    const char *intervisitTimeName = intervisitTimeNameString.c_str();
    recordScalar(intervisitTimeName, intervisitTime.getMean());

    std::string numCycleNameString = "Number of cycle in queue[" + std::to_string(ownIndex) + "]";
    const char *numCycleName = numCycleNameString.c_str();
    recordScalar(numCycleName, cycleTime.getCount());
}

void CyclicExhaustiveBatchQueueWithMonitor::handleMessage(cMessage *msg) {
    // Receive "Polling" message from the server
    if (msg->arrivedOn("server$i")) {
        // Collect the queue length at the polling moment characteristic
        queueLenAtPollingMoment.collect(buffer.getLength());

        // Collect the intervisit time and cycle time from cycle 2
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
        if (queueLen == 0) {
            // Send "Queue is empty" msg to the server
            send(new cMessage("Queue is empty", ownIndex), "server$o");
            leavingMoment = simTime();
        }
        else
            // Start servicing
            scheduleAt(simTime(), takeBatchEvent);
    }

    // Serve the batch
    else if (msg == takeBatchEvent) {
        // Get the size of the current batch
        if (queueLen >= batchSize)
            sizeOfCurrBatch = batchSize;
        else
            sizeOfCurrBatch = queueLen;

        // Update system state
        StateMessage *stateMsg = new StateMessage("SERVICE phase");
        stateMsg->setMsgType(SET_SERVER_PHASE);
        stateMsg->setServerPhase(SERVICE);
        stateMsg->setQueueIndex(ownIndex);
        stateMsg->setKc(sizeOfCurrBatch);
        send(stateMsg, "toMonitor");

        // Take out the batch from buffer and serve it
        for (int i = 0; i < sizeOfCurrBatch; i++) {
            batch[i] = (cMessage*)buffer.pop();

            // Collect the the statistics
            waitingTime.collect(simTime() - batch[i]->getCreationTime());
        }

        scheduleAt(simTime() + par("serviceTime"), batchServDoneEvent);
    }

    // Finish servicing the batch
    else if (msg == batchServDoneEvent) {
        for (int i = 0; i < sizeOfCurrBatch; i++) {
            // Collect the the statistics
            sojTime.collect(simTime() - batch[i]->getCreationTime());

            // Send out the batch to the server
            send(batch[i], "server$o");
            queueLen--;
        }

        // Send msg to the monitor to update system state
        StateMessage *stateMsg = new StateMessage("Set n[i]");
        stateMsg->setMsgType(SET_N);
        stateMsg->setQueueIndex(ownIndex);
        stateMsg->setN(queueLen);
        send(stateMsg, "toMonitor");

        // Check if queue is empty
        if (queueLen == 0) {
            send(new cMessage("Queue has been served", ownIndex), "server$o");
            leavingMoment = simTime();
        }
        else
            scheduleAt(simTime(), takeBatchEvent);
    }

    // Insert packet from the generator to the buffer if there is space for it
    else
        if (queueLen < queueCapacity) {
            buffer.insert(msg);
            queueLen++;

            // Send msg to the monitor to update system state
            StateMessage *stateMsg = new StateMessage("Set n[i]");
            stateMsg->setMsgType(SET_N);
            stateMsg->setQueueIndex(ownIndex);
            stateMsg->setN(queueLen);
            send(stateMsg, "toMonitor");
        }
        else
            delete(msg);
}
