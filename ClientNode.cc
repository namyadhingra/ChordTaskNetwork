// ClientNode.cc - Implements a peer-to-peer client node in a Chord-based distributed system
// Handles task distribution, result aggregation, and gossip message propagation
// Features: O(logN) message routing, distributed task processing, gossip-based consensus

#include "ClientNode.h"
#include <fstream>
#include <algorithm>
#include <string>
#include <iostream>

using namespace chord;

Define_Module(ClientNode);

// Initialize the client node: set parameters, load config, and schedule first task
void ClientNode::initialize()
{
    // Get node ID and network size
    myId = par("clientId").intValue();
    N = getParentModule()->par("N").intValue();
    
    // Initialize task tracking variables
    numTasksReceived = 0;      // Counter for received results
    overallMax = -1;           // Track maximum value across all subtasks
    uniqueGossips = 0;         // Count of unique gossip messages received
    terminated = false;        // Flag indicating node has received all gossips
    
    x = 20; 
    k = 100;
    
    std::ifstream config("config.txt");
    std::string line;
    if (config.is_open()) {
        while(std::getline(config, line)) {
            if(line.find("x=") == 0) x = std::stoi(line.substr(2));
            if(line.find("k=") == 0) k = std::stoi(line.substr(2));
        }
        config.close();
    }
    
    // Clear output file at init for node 0 to avoid appending ad infinitum
    if (myId == 0) {
        std::ofstream out("outputfile.txt", std::ios_base::trunc);
        out.close();
    }
    
    // Schedule self task randomly to stagger load (uniform 0 to 1 sec)
    cMessage *initTimer = new cMessage("startTask");
    scheduleAt(simTime() + uniform(0, 1.0), initTimer);
}

// Log messages to both console and output file
void ClientNode::logOutput(std::string text)
{
    EV << text << "\n";
    std::ofstream out("outputfile.txt", std::ios_base::app);
    out << text << "\n";
    out.close();
}
// Initiate a task: divide array into x subtasks and route each to target client
void ClientNode::initiateTask()
{
    logOutput("Node " + std::to_string(myId) + " initiating task, splitting " + std::to_string(k) + " elements into " + std::to_string(x) + " parts.");
    
    // Divide k elements approximately equally among x subtaskslogOutput("Node " + std::to_string(myId) + " initiating task, splitting " + std::to_string(k) + " elements into " + std::to_string(x) + " parts.");
    
    int elementsPerTask = k / x;
    // Create and send x subtasks
    for (int i = 0; i < x; ++i) {
        ChordAppMsg *msg = new ChordAppMsg("TaskMsg");
        msg->setMsgType(TASK_MSG);
        msg->setSourceId(myId);
        
        // Route subtask i to node (i % N) using Chord routing
        int destTarget = i % N;
        msg->setDestId(destTarget);
        msg->setInitiatorId(myId);
        msg->setSubtaskId(i);
        
        // Distribute data elements: some subtasks get one extra if k is not divisible by xmsg->setInitiatorId(myId);
        msg->setSubtaskId(i);
        
        int count = elementsPerTask + (i < remaining ? 1 : 0);
        msg->setDataArraySize(count);
        for (int c = 0; c < count; ++c) {
            msg->setData(c, intuniform(1, 1000));
// Route a message towards destination using greedy Chord routing
// Selects neighbor closest to destination in ring topology
void ClientNode::routeMessage(int destId, ChordAppMsg *msg)
{
    // If destination is self, deliver immediately
    if (myId == destId) {
        scheduleAt(simTime(), msg);
        return;
    }
    
    // Calculate distance to destination in ring
    int bestDist = (destId - myId + N) % N;
    int bestGateIndex = -1;
    int maxNDist = -1;
    
    // Find neighbor that is closest to destination (greedy routing)
    for (int g = 0; g < gateSize("port"); ++g) {
        cGate *outGate = gate("port$o", g);
        if (outGate->isConnected()) {
            cModule *neighbor = outGate->getPathEndGate()->getOwnerModule();
            int neighborId = neighbor->par("clientId").intValue();
            
            // Distance from this node to neighbor
            int nDist = (neighborId - myId + N) % N;
            
            // Select neighbor that moves closest to destination without overshooting
            if (nDist > 0 && nDist <= bestDist) {
                if (nDist > maxNDist) {
                    maxNDist = nDist;
                    bestGateIndex = g;
                }
            }
        }
    }
    
    // Send message to selected neighbor
    if (bestGateIndex != -1) {
        msg->setSourceId(myId);
// Handle incoming task message: compute local max and send result back to initiator
void ClientNode::handleTaskMsg(ChordAppMsg *msg)
{
    // If this node is the destination, process the subtask
    if (myId == msg->getDestId()) {
        // Find maximum value in the assigned data array
        int localMax = -1;
        int dataSize = msg->getDataArraySize();
        for (int i = 0; i < dataSize; ++i) {
            int value = msg->getData(i);
            if (value > localMax) localMax = value;
        }
        
        logOutput("Node " + std::to_string(myId) + " computed local max " + std::to_string(localMax) + 
                  " for subtask " + std::to_string(msg->getSubtaskId()) + " from initiator " + std::to_string(msg->getInitiatorId()));
        
        // Create and route result message back to initiator
        ChordAppMsg *res = new ChordAppMsg("ResultMsg");
        res->setMsgType(RESULT_MSG);
        res->setInitiatorId(msg->getInitiatorId());
        res->setSubtaskId(msg->getSubtaskId());
        res->setMaxResult(localMax);
        res->setSourceId(myId);
        res->setDestId(msg->getInitiatorId());
        
        routeMessage(res->getDestId(), res);
        delete msg;
    } else {
        // Not the destination, forward the messageutput("Node " + std::to_string(myId) + " computed local max " + std::to_string(localMax) + 
                  " for subtask " + std::to_string(msg->getSubtaskId()) + " from initiator " + std::to_string(msg->getInitiatorId()));
        
        ChordAppMsg *res = new ChordAppMsg("ResultMsg");
// Handle result message: aggregate results and initiate gossip when all results received
void ClientNode::handleResultMsg(ChordAppMsg *msg)
{
    // If this node is the task initiator, aggregate the result
    if (myId == msg->getDestId()) { 
        numTasksReceived++;
        int val = msg->getMaxResult();
        
        // Track overall maximum across all subtasks
        if (val > overallMax) overallMax = val;
        
        logOutput("Node " + std::to_string(myId) + " received result " + std::to_string(val) + " for subtask " + std::to_string(msg->getSubtaskId()));
        
        // When all results received, task is complete - initiate gossip protocol
        if (numTasksReceived == x) {
            logOutput("Task Complete on Node " + std::to_string(myId) + ". Conclusive Max: " + std::to_string(overallMax));
            
            // Create gossip message with timestamp and send to neighbors
            ChordAppMsg *gMsg = new ChordAppMsg("GossipMsg");
            gMsg->setMsgType(GOSSIP_MSG);
            gMsg->setGossipOriginIP(myId);
            gMsg->setGossipTimestamp(simTime().dbl());
            gMsg->setGossipClientId(myId);
            gMsg->setSourceId(myId); 
            
            handleGossipMsg(gMsg);
        }
        delete msg;
    } else {
        // Not the initiator, forward the result message
        if (numTasksReceived == x) {
            logOutput("Task Complete on Node " + std::to_string(myId) + ". Conclusive Max: " + std::to_string(overallMax));
            
            ChordAppMsg *gMsg = new ChordAppMsg("GossipMsg");
            gMsg->setMsgType(GOSSIP_MSG);
            gMsg->setGossipOriginIP(myId);
// Handle gossip message: track unique gossips and propagate using flooding (except backward)
void ClientNode::handleGossipMsg(ChordAppMsg *msg)
{
    // Create unique identifier for this gossip message using origin and original timestamp
    std::pair<int, double> hashM = std::make_pair(msg->getGossipOriginIP(), msg->getGossipTimestamp());
    
    // Check if this is a new gossip message (not seen before)
    if (receivedGossips.find(hashM) == receivedGossips.end()) {
        receivedGossips.insert(hashM);
        uniqueGossips++;
        
        // Log record: format <localTimestamp>:<originIP>:<clientID>
        // Using local timestamp (simTime()) of when this node receives the gossip
        logOutput("Node " + std::to_string(myId) + " recvd GOSSIP <" + std::to_string(simTime().dbl()) + ">:<192.168.0." + 
                  std::to_string(msg->getGossipOriginIP()) + ">:<" + std::to_string(msg->getGossipClientId()) + ">" +
                  " FROM:" + std::to_string(msg->getSourceId()));
        
        // Check if node has received gossips from all N nodes
        if (uniqueGossips == N) {
            logOutput("Node " + std::to_string(myId) + " received gossips from all N nodes. Terminating.");
            terminated = true;
            delete msg;
            return;
        }
        
        // Propagate gossip to all neighbors except the sender (standard gossip protocol)
        int senderId = msg->getSourceId();
        
        for (int g = 0; g < gateSize("port"); ++g) {
            cGate *outGate = gate("port$o", g);
            if (outGate->isConnected()) {
                cModule *neighbor = outGate->getPathEndGate()->getOwnerModule();
                int neighborId = neighbor->par("clientId").intValue();
                
                // Forward to all neighbors except sender (prevents loops)inated = true;
            delete msg;
            return;
        }
        
        int senderId = msg->getSourceId();
        
        for (int g = 0; g < gateSize("port"); ++g) {
            cGate *outGate = gate("port$o", g);
// Main message handler: routes messages to appropriate handlers based on type
void ClientNode::handleMessage(cMessage *msg)
{
    // Ignore messages after node has terminated
    if (terminated) {
        delete msg;
        return;
    }
    
    // Handle self-messages (scheduled tasks)
    if (msg->isSelfMessage() && std::string(msg->getName()) == "startTask") {
        initiateTask();
        delete msg;
    } else {
        // Route application messages to appropriate handler
    }
    
    delete msg;
}

void ClientNode::handleMessage(cMessage *msg)
{
    if (terminated) {
        delete msg;
        return;
    }
    
    if (msg->isSelfMessage() && std::string(msg->getName()) == "startTask") {
        initiateTask();
        delete msg;
    } else {
        ChordAppMsg *cmsg = check_and_cast<ChordAppMsg *>(msg);
        if (cmsg->getMsgType() == TASK_MSG) {
            handleTaskMsg(cmsg);
        } else if (cmsg->getMsgType() == RESULT_MSG) {
            handleResultMsg(cmsg);
        } else if (cmsg->getMsgType() == GOSSIP_MSG) {
            handleGossipMsg(cmsg);
        } else {
            delete msg;
        }
    }
}
