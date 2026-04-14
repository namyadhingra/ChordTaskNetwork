// Coordinator.cc - Initializes network topology from topo.txt
// Reads edge list and creates bidirectional gate connections between all client nodes
// Enables CHORD O(log N) routing by establishing ring and finger table links

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

// Stage 0 initialization: read topo.txt and create network connections dynamically
void Coordinator::initialize(int stage)
{
    if (stage == 0) {
        // Read topology edges from topo.txt and create gate connections
        int N = getParentModule()->par("N").intValue();
        std::ifstream topoFile("topo.txt");
        if (!topoFile.is_open()) {
            throw cRuntimeError("Error opening topo.txt");
        }

        // Process each edge in the topology file
        int u, v;
        while (topoFile >> u >> v) {
            // Validate node IDs are within range
            if (u >= N || v >= N) {
                EV << "Skipping invalid edge: " << u << " " << v << "\n";
                continue;
            }

            // Get module references for endpoints
            cModule *modU = getParentModule()->getSubmodule("client", u);
            cModule *modV = getParentModule()->getSubmodule("client", v);

            if (!modU || !modV) {
                throw cRuntimeError("Modules not found!");
            }

            // Create bidirectional connection: u -> v with delay channel
            cGate *uOut = modU->getOrCreateFirstUnconnectedGate("port$o", 0, false, true);
            cGate *vIn  = modV->getOrCreateFirstUnconnectedGate("port$i", 0, false, true);
            cDelayChannel *ch1 = cDelayChannel::create("channel");
            ch1->setDelay(0.01);
            uOut->connectTo(vIn, ch1);
            ch1->callInitialize();

            // Create bidirectional connection: v -> u with delay channel
            cGate *vOut = modV->getOrCreateFirstUnconnectedGate("port$o", 0, false, true);
            cGate *uIn  = modU->getOrCreateFirstUnconnectedGate("port$i", 0, false, true);
            cDelayChannel *ch2 = cDelayChannel::create("channel");
            ch2->setDelay(0.01);
            vOut->connectTo(uIn, ch2);
            ch2->callInitialize();
        }
        topoFile.close();
    }
}

// Handle messages (none expected during topology setup)
void Coordinator::handleMessage(cMessage *msg)
{
    delete msg;
}
