#include "ClientNode.h"
#include <fstream>
#include <algorithm>
#include <string>
#include <iostream>

Define_Module(ClientNode);

void ClientNode::initialize()
{
    myId = par("clientId").intValue();
    N = getParentModule()->par("N").intValue();
    
    numTasksReceived = 0;
    overallMax = -1;
    uniqueGossips = 0;
    terminated = false;
    
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

void ClientNode::logOutput(std::string text)
{
    EV << text << "\n";
    std::ofstream out("outputfile.txt", std::ios_base::app);
    out << text << "\n";
    out.close();
}

void ClientNode::initiateTask()
{
    logOutput("Node " + std::to_string(myId) + " initiating task, splitting " + std::to_string(k) + " elements into " + std::to_string(x) + " parts.");
    
    int elementsPerTask = k / x;
    int remaining = k % x;
    
    for (int i = 0; i < x; ++i) {
        ChordAppMsg *msg = new ChordAppMsg("TaskMsg");
        msg->setMsgType(TASK_MSG);
        msg->setSourceId(myId);
        int destTarget = i % N;
        msg->setDestId(destTarget);
        msg->setInitiatorId(myId);
        msg->setSubtaskId(i);
        
        int count = elementsPerTask + (i < remaining ? 1 : 0);
        IntVector dataVec;
        for (int c = 0; c < count; ++c) {
            dataVec.push_back(intuniform(1, 1000));
        }
        msg->setData(dataVec);
        
        routeMessage(destTarget, msg);
    }
}

void ClientNode::routeMessage(int destId, ChordAppMsg *msg)
{
    if (myId == destId) {
        scheduleAt(simTime(), msg);
        return;
    }
    
    int bestDist = (destId - myId + N) % N;
    int bestGateIndex = -1;
    int maxNDist = -1;
    
    for (int g = 0; g < gateSize("port"); ++g) {
        cGate *outGate = gate("port$o", g);
        if (outGate->isConnected()) {
            cModule *neighbor = outGate->getPathEndGate()->getOwnerModule();
            int neighborId = neighbor->par("clientId").intValue();
            
            int nDist = (neighborId - myId + N) % N;
            if (nDist > 0 && nDist <= bestDist) {
                if (nDist > maxNDist) {
                    maxNDist = nDist;
                    bestGateIndex = g;
                }
            }
        }
    }
    
    if (bestGateIndex != -1) {
        msg->setSourceId(myId); // Distance routing hop
        send(msg, "port$o", bestGateIndex);
    } else {
        EV << "Warning: Node " << myId << " failed to route towards " << destId << ". Dropping msg.\n";
        delete msg;
    }
}

void ClientNode::handleTaskMsg(ChordAppMsg *msg)
{
    if (myId == msg->getDestId()) {
        int localMax = -1;
        const IntVector& data = msg->getData();
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] > localMax) localMax = data[i];
        }
        
        logOutput("Node " + std::to_string(myId) + " computed local max " + std::to_string(localMax) + 
                  " for subtask " + std::to_string(msg->getSubtaskId()) + " from initiator " + std::to_string(msg->getInitiatorId()));
        
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
        routeMessage(msg->getDestId(), msg);
    }
}

void ClientNode::handleResultMsg(ChordAppMsg *msg)
{
    if (myId == msg->getDestId()) { 
        numTasksReceived++;
        int val = msg->getMaxResult();
        if (val > overallMax) overallMax = val;
        
        logOutput("Node " + std::to_string(myId) + " received result " + std::to_string(val) + " for subtask " + std::to_string(msg->getSubtaskId()));
        
        if (numTasksReceived == x) {
            logOutput("Task Complete on Node " + std::to_string(myId) + ". Conclusive Max: " + std::to_string(overallMax));
            
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
        routeMessage(msg->getDestId(), msg);
    }
}

void ClientNode::handleGossipMsg(ChordAppMsg *msg)
{
    std::pair<int, double> hashM = std::make_pair(msg->getGossipOriginIP(), msg->getGossipTimestamp());
    
    if (receivedGossips.find(hashM) == receivedGossips.end()) {
        receivedGossips.insert(hashM);
        uniqueGossips++;
        
        logOutput("Node " + std::to_string(myId) + " recvd GOSSIP IP:192.168.0." + std::to_string(msg->getGossipOriginIP()) + 
                  " TS:" + std::to_string(msg->getGossipTimestamp()) + " FROM:" + std::to_string(msg->getSourceId()));
        
        if (uniqueGossips == N) {
            logOutput("Node " + std::to_string(myId) + " received gossips from all N nodes. Terminating.");
            terminated = true;
            delete msg;
            return;
        }
        
        int senderId = msg->getSourceId();
        
        for (int g = 0; g < gateSize("port"); ++g) {
            cGate *outGate = gate("port$o", g);
            if (outGate->isConnected()) {
                cModule *neighbor = outGate->getPathEndGate()->getOwnerModule();
                int neighborId = neighbor->par("clientId").intValue();
                
                if (neighborId != senderId) {
                    ChordAppMsg *dup = msg->dup();
                    dup->setSourceId(myId);
                    send(dup, "port$o", g);
                }
            }
        }
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
