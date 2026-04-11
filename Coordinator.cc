#include <omnetpp.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace omnetpp;

class Coordinator : public cSimpleModule
{
  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Coordinator);

void Coordinator::initialize(int stage)
{
    if (stage == 0) {
        // Read topology from topo.txt and create connections dynamically
        int N = getParentModule()->par("N").intValue();
        std::ifstream topoFile("topo.txt");
        if (!topoFile.is_open()) {
            throw cRuntimeError("Error opening topo.txt");
        }

        int u, v;
        while (topoFile >> u >> v) {
            if (u >= N || v >= N) {
                EV << "Skipping invalid edge: " << u << " " << v << "\n";
                continue;
            }

            cModule *modU = getParentModule()->getSubmodule("client", u);
            cModule *modV = getParentModule()->getSubmodule("client", v);

            if (!modU || !modV) {
                throw cRuntimeError("Modules not found!");
            }

            // Create bidirectional connection using inout gates
            // u -> v
            cGate *uOut = modU->getOrCreateFirstUnconnectedGate("port$o", 0, false, true);
            cGate *vIn  = modV->getOrCreateFirstUnconnectedGate("port$i", 0, false, true);
            cDelayChannel *ch1 = cDelayChannel::create("channel");
            ch1->setDelay(0.01);
            uOut->connectTo(vIn, ch1);

            // v -> u
            cGate *vOut = modV->getOrCreateFirstUnconnectedGate("port$o", 0, false, true);
            cGate *uIn  = modU->getOrCreateFirstUnconnectedGate("port$i", 0, false, true);
            cDelayChannel *ch2 = cDelayChannel::create("channel");
            ch2->setDelay(0.01);
            vOut->connectTo(uIn, ch2);
        }
        topoFile.close();
    }
}

void Coordinator::handleMessage(cMessage *msg)
{
    delete msg;
}
