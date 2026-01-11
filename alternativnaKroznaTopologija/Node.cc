#include <omnetpp.h>
#include <cmath>

using namespace omnetpp;

class Node : public cSimpleModule
{
  private:
    int nodeId;
    int hopLimit;
    int queueLength;
    simsignal_t delaySignal;
    int num_packets;
    int lost_packets;
    simtime_t delay_total;
    cMessage *sendTimer;
    cMessage *endTxEvent;
    cPacketQueue txQueue;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    void forwardPacket(cPacket *pkt);
    ~Node();
    cDatarateChannel *getOutChannel();
    void updateDisplay();

    //cStdDev eedAvg;
};

Define_Module(Node);

void Node::initialize()
{
    nodeId = getIndex();
    hopLimit = par("hopLimit");
    queueLength = par("queueLength");
    delaySignal = registerSignal("packetDelay");

    delay_total = SIMTIME_ZERO;
    num_packets = 0;
    lost_packets = 0;

    sendTimer = new cMessage("sendTimer");
    endTxEvent = new cMessage("endTxEvent");

    txQueue.setName("txQueue");

    //eedAvg.setName("End-to-End Delay - average");

    // schedule first packet generation
    simtime_t firstSend = uniform(0, par("sendInterval"));
    scheduleAt(simTime() + firstSend, sendTimer);

    // positioning for display
    int totalNodes = getParentModule()->par("numNodes");
    double angle = 2 * M_PI * nodeId / totalNodes;
    double radius = 200.0;
    double cx = 300 + radius * cos(angle);
    double cy = 300 + radius * sin(angle);
    getDisplayString().setTagArg("p", 0, cx);
    getDisplayString().setTagArg("p", 1, cy);
}

void Node::handleMessage(cMessage *msg)
{
    // --- Handle self-messages first ---
    if (msg->isSelfMessage()) {
        if (msg == sendTimer) {
            // generate new packet
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

            if (queueLength == 0 || txQueue.getLength() < queueLength) {
                txQueue.insert(pkt);
                updateDisplay();
            } else {
                EV << "txQueue full, dropping new packet\n";
                delete pkt;
            }

            // try to send immediately
            if (!txQueue.isEmpty() && !getOutChannel()->isBusy() && !endTxEvent->isScheduled()) {
                cPacket *next = (cPacket *)txQueue.pop();
                send(next, "out", 0);
                scheduleAt(getOutChannel()->getTransmissionFinishTime(), endTxEvent);
                updateDisplay();
            }

            // schedule next packet generation
            scheduleAt(simTime() + exponential(par("sendInterval").doubleValue()), sendTimer);
            return;
        }
        else if (msg == endTxEvent) {
            // finish transmitting next packet
            if (!txQueue.isEmpty()) {
                cPacket *next = (cPacket *)txQueue.pop();
                send(next, "out", 0);
                scheduleAt(getOutChannel()->getTransmissionFinishTime(), endTxEvent);
                updateDisplay();
            }
            return;
        }
    }

    // --- Now msg is guaranteed to be a cPacket ---
    cPacket *pkt = check_and_cast<cPacket *>(msg);
    int hops = pkt->par("hops");
    EV << "hops_init: " << hops << "\n";
    int start = pkt->par("startId");
    EV << "start: " << start << "\n";
    int dest = pkt->par("destinationId");
    EV << "dest: " << dest << "\n";
    simtime_t startTime = simtime_t(pkt->par("startTime").doubleValue());

    EV << "Node " << nodeId << " received packet. Hops=" << hops << "\n";

    // check if destination reached
    if (nodeId == dest && hops > 0) {
        simtime_t delay = simTime() - startTime;
        EV << "Packet reached destination " << dest << " after " << hops << " hops. Delay=" << delay << "\n";
        emit(delaySignal, delay);
        delay_total += delay;
        num_packets++;
        delete pkt;
        return;
    }

    // check if returned to start
    if (nodeId == start && hops > 0) {
        simtime_t delay = simTime() - startTime;
        EV << "Packet returned to starting node after " << hops << " hops. Delay=" << delay << "\n";
        emit(delaySignal, delay);
        delete pkt;
        lost_packets++;
        return;
    }

    // check hop limit
    if (hops >= hopLimit) {
        EV << "Hop limit reached, deleting packet.\n";
        delete pkt;
        lost_packets++;
        return;
    }

    // increment hops and forward
    pkt->par("hops") = hops + 1;
    EV << "Checkpoint. phops: " << pkt->par("hops") << " hops: " << hops << "\n";
    forwardPacket(pkt);
}

void Node::forwardPacket(cPacket *pkt)
{
    int dest = pkt->par("destinationId");
    EV << "Node " << nodeId << " forwarding packet to " << dest << ".\n";

    // insert into queue if there's space
    if (queueLength == 0 || txQueue.getLength() < queueLength) {
        txQueue.insert(pkt);
        updateDisplay();
    } else {
        EV << "txQueue full, dropping packet\n";
        delete pkt;
        return;
    }

    // try sending immediately
    if (!txQueue.isEmpty() && !getOutChannel()->isBusy() && !endTxEvent->isScheduled()) {
        cPacket *next = (cPacket *)txQueue.pop();
        send(next, "out", 0);
        scheduleAt(getOutChannel()->getTransmissionFinishTime(), endTxEvent);
        updateDisplay();
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

void Node::finish() {
    double delay_average = 0;
    if (num_packets > 0) {
        delay_average = delay_total.dbl() / num_packets;
    }
    //eedAvg.collect(delay_average);
    //eedAvg.record();
    recordScalar("Average eed", delay_average);
    recordScalar("queue elements", txQueue.getLength());
    recordScalar("lost packets", lost_packets);

    EV << "Total Packets Received: " << num_packets << "\n";
    EV << "Total End-to-End Delay: " << delay_total << " seconds\n";
    EV << "Average Delay: " << delay_average << " seconds\n";
}
