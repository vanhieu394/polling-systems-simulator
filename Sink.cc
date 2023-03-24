#include <omnetpp.h>
#include <ctime>

using namespace omnetpp;

class Sink: public cSimpleModule{
private:
    // online stats
    int x = 0;
    double W_i = 0;
    double W_i_1;

    clock_t time = 0;
    clock_t time2;

    cStdDev sojTimeStats;               // Sojourn time stats
    cOutVector sojTimeVector;           // Sojourn time vector

public:
    Sink();
    virtual~ Sink();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};
Define_Module(Sink);
Sink::Sink(){
}
Sink::~Sink(){
}
void Sink::initialize() {
    sojTimeStats.setName("Total sojourn time");
    sojTimeVector.setName("Sojourn time");
}
void Sink::finish() {
    recordScalar("Mean sojourn time",sojTimeStats.getMean());
    recordScalar("Number of packets",sojTimeStats.getCount());
    recordScalar("Working time", (double)time/CLOCKS_PER_SEC);
}
void Sink::handleMessage(cMessage *msg) {
    if (x == 1) {
        time = time + clock() - time2;
    }
    else {
        x = 1;
    }
    time2 = clock();

    // compute queueing delay
    simtime_t delay = simTime() - msg->getCreationTime();

    // update stats
    sojTimeStats.collect(delay);
//    sojTimeVector.record(sojTimeStats.getMean());

    W_i_1 = W_i;
    W_i = sojTimeStats.getMean();
    if (fabs(W_i - W_i_1)/W_i < pow(10,-13))
        {
            time = time + clock() - time2;
            endSimulation();
        }
    //delete msg
    delete(msg);
}
