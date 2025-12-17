#include <omnetpp.h>
#include <cmath>

using namespace omnetpp;

class Node : public cSimpleModule
{
  private:
    int nodeId;
    int hopLimit;
    simsignal_t delaySignal;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void forwardMessage(cMessage *msg);
};

Define_Module(Node);

void Node::initialize()
{
    nodeId = getIndex();
    hopLimit = par("hopLimit");
    delaySignal = registerSignal("packetDelay");

    int startId = par("startId");
    int destinationId = par("destinationId");

    bool isPacket = par("isPacket");

    // Node under startId starts the first message
    if (nodeId == startId) {
        if(isPacket){
            cPacket *pckt = new cPacket("RingPacket");
            int packetSize = par("packetSize");
            pckt->setByteLength(packetSize);
            pckt->addPar("hops") = 0;
            pckt->addPar("startId") = startId;
            pckt->addPar("destinationId") = destinationId;
            pckt->addPar("startTime") = simTime().dbl();

            scheduleAt(simTime() + 1.0, pckt);
        }else{
            cMessage *msg = new cMessage("RingMessage");
            msg->addPar("hops") = 0;
            msg->addPar("startId") = startId;
            msg->addPar("destinationId") = destinationId;
            msg->addPar("startTime") = simTime().dbl();

            scheduleAt(simTime() + 1.0, msg);
        }
    }

    // positioning
    int totalNodes = getParentModule()->par("numNodes");
    double angle = 2 * M_PI * nodeId / totalNodes;
    double radius = 200.0; // radius for display
    double cx = 300 + radius * cos(angle);  // 300 offset
    double cy = 300 + radius * sin(angle);
    getDisplayString().setTagArg("p", 0, cx);
    getDisplayString().setTagArg("p", 1, cy);
}

void Node::handleMessage(cMessage *msg)
{
    int hops = msg->par("hops");
    int start = msg->par("startId");
    int dest = msg->par("destinationId");
    simtime_t startTime = simtime_t(msg->par("startTime").doubleValue());

    EV << "Node " << nodeId << " received message. Hops=" << hops << "\n";

    if(nodeId == dest && hops > 0){
        simtime_t delay = simTime() - startTime;
        EV << "Message has reached its destination at Node " << dest << " after " << hops << " hops. Delay=" << delay << "\n";
        emit(delaySignal, delay);
        delete msg;
        return;
    }

    //if (nodeId == 0 && hops > 0) {
    if(nodeId == start && hops > 0) {
        simtime_t delay = simTime() - startTime;
        EV << "Message returned to starting Node after " << hops << " hops. Delay=" << delay << "\n";
        emit(delaySignal, delay);
        delete msg;
        return;
    }

    if (hops >= hopLimit) {
        EV << "Hop limit reached, deleting message.\n";
        delete msg;
        return;
    }

    msg->par("hops") = hops + 1;
    forwardMessage(msg);
}

void Node::forwardMessage(cMessage *msg)
{
    EV << "Node " << nodeId << " forwarding message to next node.\n";
    send(msg, "out", 0);
}
