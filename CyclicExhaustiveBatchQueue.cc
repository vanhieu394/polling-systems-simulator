#include <omnetpp.h>
#include <StateMessage_m.h>

using namespace omnetpp;

//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor


class CyclicExhaustiveBatchQueue : public cSimpleModule {
private:
    int ownIndex;                       // Queue's index
    cQueue buffer;                      // Buffer to save all packets
    double queueCapacity;               // Maximum space of the queue
    double currQueueLen;                // Current queue length
    int batchSize;                      // Batch size of this queue
    int sizeOfCurrBatch;                // Size of the current batch
    long int cycleNumber;               // Current cycle number
    double numPacket;                   // Number of arrival packet (all packets that were generated from generator)
    double effNumPacket;                // Efficient number of packets (that actual enter to the queue)

    simtime_t pollingMoment;            // The last polling moment of this queue
    simtime_t leavingMoment;            // The moment when server completes serving this queue and leaves it

    cMessage *takeBatchEvent;           // Take a batch of packet out of the buffer
    cMessage *batchServDoneEvent;       // A batch of packet has been served
    std::vector<cMessage *> batch;      // Current servicing batch

    // Output statistics
    double lossRate;                    // Loss rate of the queue
    double effArrivalRate;              // Effective arrival rate of the queue
    double queueLen;                    // The length of queue at any time
    cStdDev queueLenAtPollingMoment;    // Queue length at its polling moment
    cStdDev waitingTime;                // The waiting time sequence of all packets
    cStdDev sojTime;                    // The sojourn time sequence of all packets
    cStdDev intervisitTime;             // The intervisitTime sequence of all packets
    cStdDev cycleTime;                  // The cycle time sequence

public:
    CyclicExhaustiveBatchQueue();
    virtual~ CyclicExhaustiveBatchQueue();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(CyclicExhaustiveBatchQueue);

CyclicExhaustiveBatchQueue::CyclicExhaustiveBatchQueue() {
    takeBatchEvent = nullptr;
    batchServDoneEvent = nullptr;
}

CyclicExhaustiveBatchQueue::~CyclicExhaustiveBatchQueue() {
    cancelAndDelete(takeBatchEvent);
    cancelAndDelete(batchServDoneEvent);
}

void CyclicExhaustiveBatchQueue::initialize() {
    ownIndex = par("ownIndex");
    buffer.setName("buffer");
    queueCapacity = par("queueCapacity");
    currQueueLen = 0;
    batchSize = par("batchSize");
    sizeOfCurrBatch = 0;
    cycleNumber = 0;
    numPacket = 0;
    effNumPacket = 0;

    pollingMoment = 0;
    leavingMoment = 0;

    lossRate = 0;
    effArrivalRate = 0;
    queueLen = 0;

    takeBatchEvent = new cMessage("Taking a batch of packet out of the buffer");
    batchServDoneEvent = new cMessage("A batch of packet has been served");

    for (int i = 0; i < batchSize; i++)
        batch.push_back(nullptr);
}

void CyclicExhaustiveBatchQueue::finish() {
    std::string lossRateNameString = "Mean loss rate in queue[" + std::to_string(ownIndex) + "]";
    const char *lossRateName = lossRateNameString.c_str();
    lossRate = 1 - effNumPacket/numPacket;
    recordScalar(lossRateName, lossRate);

    std::string effArrivalRateNameString = "Mean effective arrival rate in queue[" + std::to_string(ownIndex) + "]";
    const char *effArrivalRateName = effArrivalRateNameString.c_str();
    effArrivalRate = effNumPacket/simTime();
    recordScalar(effArrivalRateName, effArrivalRate);

    std::string queueLenNameString = "Mean queue length in queue[" + std::to_string(ownIndex) + "]";
    const char *queueLenName = queueLenNameString.c_str();
    // Formula Little: L_i = W_i * lambdaE_i
    queueLen = sojTime.getMean()*effArrivalRate;
    recordScalar(queueLenName, queueLen);

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

void CyclicExhaustiveBatchQueue::handleMessage(cMessage *msg) {
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

        currQueueLen = buffer.getLength();
        if (currQueueLen == 0) {
            // Send "Queue is empty" msg to the server
            send(new cMessage("Queue is empty", ownIndex), "server$o");
            leavingMoment = simTime();
        }
        else {
            // Start servicing
            scheduleAt(simTime(), takeBatchEvent);
        }
    }

    // Serve the batch
    else if (msg == takeBatchEvent) {
        // Get the size of the current batch
        if (currQueueLen >= batchSize)
            sizeOfCurrBatch = batchSize;
        else
            sizeOfCurrBatch = currQueueLen;

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
            currQueueLen--;
        }

        // Check if queue is empty
        if (currQueueLen == 0) {
            send(new cMessage("Queue has been served", ownIndex), "server$o");
            leavingMoment = simTime();
        }
        else
            scheduleAt(simTime(), takeBatchEvent);
    }

    // Insert packet from the generator to the buffer if there is space for it
    else {
        numPacket++;

        if (currQueueLen < queueCapacity) {
            buffer.insert(msg);
            currQueueLen++;
            effNumPacket++;
        }
        else
            delete(msg);
    }
}
