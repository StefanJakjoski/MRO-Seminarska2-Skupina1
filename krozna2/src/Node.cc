#include <omnetpp.h>
#include <cmath>

using namespace omnetpp;

class Node : public cSimpleModule
{
  private:
    int nodeId;
    int hopLimit;
    simsignal_t delaySignal;
    cMessage *sendTimer;
    cMessage *endTxEvent;
    cPacketQueue txQueue;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void forwardMessage(cMessage *msg);
    ~Node();
    cDatarateChannel *getOutChannel();
    void updateDisplay();
};

Define_Module(Node);

void Node::initialize()
{
    nodeId = getIndex();
    hopLimit = par("hopLimit");
    delaySignal = registerSignal("packetDelay");

    sendTimer = new cMessage("sendTimer");
    endTxEvent = new cMessage("endTxEvent");

    txQueue.setName("txQueue");

    // schedule first transmission at random time
    simtime_t firstSend = uniform(0, par("sendInterval"));
    scheduleAt(simTime() + firstSend, sendTimer);


    /*
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
    */

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
    //DELETE IF DOESNT WORK
    //message to signal packet generation
    if (msg == sendTimer) {
        int totalNodes = getParentModule()->par("numNodes");

        int destination;
        do {
            destination = intuniform(0, totalNodes - 1);
        } while (destination == nodeId);

        cPacket *pkt = new cPacket("RingPacket");
        pkt->setByteLength(par("packetSize"));
        pkt->addPar("hops") = 0;
        pkt->addPar("startId") = nodeId;
        pkt->addPar("destinationId") = destination;
        pkt->addPar("startTime") = simTime().dbl();

        txQueue.insert(pkt);
        updateDisplay();

        // try to send immediately if link is free
        cDatarateChannel *chan = getOutChannel();
        if (!chan->isBusy() && !endTxEvent->isScheduled()) {
            cPacket *next = (cPacket *)txQueue.pop();
            updateDisplay();
            send(next, "out", 0);
            scheduleAt(chan->getTransmissionFinishTime(), endTxEvent);
        }

        // schedule next generation
        scheduleAt(simTime() + exponential(par("sendInterval").doubleValue()), sendTimer);
        return;
    }

    //message to signal end of transmission event
    if (msg == endTxEvent) {
        if (!txQueue.isEmpty()) {
            cPacket *next = (cPacket *)txQueue.pop();
            updateDisplay();
            send(next, "out", 0);
            cDatarateChannel *chan = getOutChannel();
            scheduleAt(chan->getTransmissionFinishTime(), endTxEvent);
        }

        return;
    }

    //END

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
    int dest = msg->par("destinationId");
    EV << "Node " << nodeId << " forwarding message until at " << dest <<".\n";

    cPacket *pkt = check_and_cast<cPacket *>(msg);  //voodoo magic i found online
    txQueue.insert(pkt);
    updateDisplay();

    cDatarateChannel *chan = getOutChannel();
    if (!chan->isBusy() && !endTxEvent->isScheduled()) {
        cPacket *next = (cPacket *)txQueue.pop();
        updateDisplay();
        send(next, "out", 0);
        scheduleAt(chan->getTransmissionFinishTime(), endTxEvent);
    }
}



Node::~Node()
{
    cancelAndDelete(sendTimer);
    cancelAndDelete(endTxEvent);
    txQueue.clear();
}

cDatarateChannel *Node::getOutChannel()
{
    return check_and_cast<cDatarateChannel *>(gate("out", 0)->getChannel());
}

void Node::updateDisplay()
{
    char buffer[32];
    sprintf(buffer, "q=%d", txQueue.getLength());
    getDisplayString().setTagArg("t", 0, buffer);
}


