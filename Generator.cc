#include <omnetpp.h>
using namespace omnetpp;

//#define OMNETPP_VERSION 0x600 // ver 6.0.1 - major*256+minor

class Generator : public cSimpleModule{
private:
    cMessage *sendMsgEvent;
public:
    Generator();
    virtual ~Generator();
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Generator);

Generator::Generator(){
    sendMsgEvent = nullptr;
}

Generator::~Generator(){
    cancelAndDelete(sendMsgEvent);
}

void Generator::initialize(){
    // Create and schedule the send-message-event
    sendMsgEvent = new cMessage("Generating a new packet");
    scheduleAt(simTime() + par("interArrivalTime"), sendMsgEvent);
}

void Generator::handleMessage(cMessage *msg){
    // Create and send packet
    cMessage *pkt;
    pkt = new cMessage("packet");
    send(pkt, "out");

    // Schedule the new packet generating
    scheduleAt(simTime() + par("interArrivalTime"), sendMsgEvent);
}

void Generator::finish(){}
