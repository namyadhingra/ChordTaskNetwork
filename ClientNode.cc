#include "ClientNode.h"

#include <algorithm>
#include <fstream>
#include <limits>
#include <string>

using namespace chord;
using namespace std;

Define_Module(ClientNode);

namespace {
int ringDistance(int fromId, int toId, int networkSize)  {
//given total network size it calculates the distance from fromId to toId in a ring topology
    return (toId - fromId + networkSize) % networkSize;
    }
}

void ClientNode::initialize()
{
    myId = par("clientId").intValue(); //each node has a unique clientId parameter defined in the NED file
    N = getParentModule()->par("N").intValue(); //total number of nodes in the network

    numTasksReceived = 0;
    overallMax = numeric_limits<int>::min(); 
    //overallMax is for storing the maximum value found across all subtasks for the initiated task
    //it starts at the minimum possible integer value to ensure any real result will be larger
    uniqueGossips = 0;
    terminated = false;

    x = 20;
    k = 100;

    ifstream config("config.txt");
    string line;
    while (getline(config, line)) {
        if (line.rfind("x=", 0) == 0) {
            x = stoi(line.substr(2));
        } else if (line.rfind("k=", 0) == 0) {
            k = stoi(line.substr(2));
        }
    }

    if (x <= 0) {
        throw cRuntimeError("Configuration error: x must be positive");
    }
    if (k < 2 * x) {
        throw cRuntimeError("Configuration error: k must be >= 2*x. Current k=%d, x=%d (k/x=%d)", k, x, k / x);
    }

    if (myId == 0) {
        ofstream out("outputfile.txt", ios_base::trunc);
    }

    scheduleAt(simTime() + uniform(0, 1.0), new cMessage("startTask"));
}

void ClientNode::logOutput(string text)
{
    EV << text << "\n";

    ofstream out("outputfile.txt", ios_base::app);
    out << text << "\n";
}

void ClientNode::initiateTask()
{
    logOutput("Node " + std::to_string(myId) + " initiating task, splitting " + std::to_string(k) + " elements into " + std::to_string(x) + " parts.");
    //logOutput stores the log message in both the simulation output and appends it to outputfile.txt for later analysis

    const int elementsPerTask = k / x;
    const int remainder = k % x;

    for (int i = 0; i < x; ++i) {
        ChordAppMsg *msg = new ChordAppMsg("TaskMsg");
        msg->setMsgType(TASK_MSG);
        msg->setSourceId(myId);
        msg->setInitiatorId(myId);
        msg->setSubtaskId(i);
        msg->setDestId(i % N);

        const int count = elementsPerTask + (i < remainder ? 1 : 0);
        msg->setDataArraySize(count); //hence msg becomes a variable-length message 
        //where the data array size is determined by how many elements this subtask should process 
        //(some subtasks may have one extra element if k is not perfectly divisible by x)
        for (int c = 0; c < count; ++c) {
            msg->setData(c, intuniform(1, 1000));
        }

        routeMessage(msg->getDestId(), msg); 
        //determines the best neighbor to forward the message towards its destination 
        //based on the ring topology and sends it there.
    }
}

void ClientNode::routeMessage(int destId, ChordAppMsg *msg, int avoidId)
{
    if (destId == myId) {
        msg->setSourceId(myId);
        scheduleAt(simTime(), msg);
        return;
    }

    int bestGateIndex = -1;
    int bestDist = ringDistance(myId, destId, N);
    int fallbackGateIndex = -1;
    int fallbackDist = N + 1;
    int avoidGateIndex = -1;

    for (int g = 0; g < gateSize("port"); ++g) {
        cGate *outGate = gate("port$o", g);
        if (!outGate->isConnected()) {
            continue;
        }

        cModule *neighbor = outGate->getPathEndGate()->getOwnerModule();
        //cModule gives access to the neighboring module connected to this gate
        //getPathEndGate() ensures we get the gate on the neighbor's side of the connection
        int neighborId = neighbor->par("clientId").intValue();

        if (neighborId == myId) {
            continue;
        }

        int distance = ringDistance(neighborId, destId, N);

        if (neighborId == avoidId && avoidGateIndex == -1) {
            avoidGateIndex = g;
        }

        //prioritises neighbors that are closer to the destination in the ring topology
        if (neighborId != avoidId && distance < bestDist) {
            bestDist = distance;
            bestGateIndex = g;
        }

        if (distance < fallbackDist) { //if no neighbor is closer than the current node
            //we can still forward to a neighbor that is not further away than the current node
            fallbackDist = distance;
            fallbackGateIndex = g;
        }
    }

    if (bestGateIndex == -1) {
        bestGateIndex = fallbackGateIndex;
        //if no neighbor is strictly closer to the destination than the current node, 
        //but we can still forward to a neighbor that is not further away
    }
    if (bestGateIndex == -1) {
        bestGateIndex = avoidGateIndex;
    }
    if (bestGateIndex == -1) {
        throw cRuntimeError("Node %d has no route to destination %d", myId, destId);
    }

    msg->setSourceId(myId);
    send(msg, "port$o", bestGateIndex); 
    //sends the message out through the selected gate towards the next hop in the route to the destination
}

void ClientNode::handleTaskMsg(ChordAppMsg *msg) {
    //this function does this:
    //1. Checks if the message is intended for this node (myId == msg->getDestId()). If so, it processes the task:
    //   - It computes the local maximum value from the data array in the message.
    //   - Logs the computed local maximum along with the subtask and initiator information.
    //   - Creates a new ResultMsg containing the local maximum result and sends it back to
    //     the initiator of the task using routeMessage.
    //2. If the message is not intended for this node, it forwards the message towards its destination using 
    //routeMessage.
    //This function is called when a TaskMsg is received. It either processes the task if it's the intended 
    //recipient or forwards it along the network towards the correct destination.
    if (myId == msg->getDestId()) {
        int localMax = numeric_limits<int>::min();
        int dataSize = msg->getDataArraySize();

        for (int i = 0; i < dataSize; ++i) {
            localMax = max(localMax, msg->getData(i));
        }

        logOutput("Node " + to_string(myId) + " computed local max " + to_string(localMax) +
                  " for subtask " + to_string(msg->getSubtaskId()) +
                  " from initiator " + to_string(msg->getInitiatorId()));

        ChordAppMsg *res = new ChordAppMsg("ResultMsg");
        res->setMsgType(RESULT_MSG);
        res->setInitiatorId(msg->getInitiatorId());
        res->setSubtaskId(msg->getSubtaskId());
        res->setMaxResult(localMax);
        res->setSourceId(myId);
        res->setDestId(msg->getInitiatorId());

        routeMessage(res->getDestId(), res, msg->getSourceId());
        delete msg;
        return;
    }

    routeMessage(msg->getDestId(), msg, msg->getSourceId());
}

void ClientNode::handleResultMsg(ChordAppMsg *msg) {
    //this function handles incoming ResultMsg messages. It checks if the message is intended for this node 
    //(myId == msg->getDestId()). If so, it processes the result:
    //   - It updates the overall maximum value found across all subtasks for the initiated task
    //   - Logs the received result along with the subtask information.
    //   - If this is the last expected result (numTasksReceived == x), it
    //     logs the completion of the task and initiates the gossip protocol to share the result with other nodes
    //if the message is not intended for this node, it forwards the message towards its destination
    //using routeMessage.
    if (myId == msg->getDestId()) {
        ++numTasksReceived;
        overallMax = std::max(overallMax, msg->getMaxResult());

        logOutput("Node " + std::to_string(myId) + " received result " + std::to_string(msg->getMaxResult()) +
                  " for subtask " + std::to_string(msg->getSubtaskId()));

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
        return;
    }

    routeMessage(msg->getDestId(), msg, msg->getSourceId());
}

void ClientNode::handleGossipMsg(ChordAppMsg *msg)
{
    const std::pair<int, double> gossipKey = std::make_pair(msg->getGossipOriginIP(), msg->getGossipTimestamp());

    if (receivedGossips.find(gossipKey) != receivedGossips.end()) {
        delete msg;
        return;
    }

    receivedGossips.insert(gossipKey);
    ++uniqueGossips;

    logOutput("Node " + std::to_string(myId) + " recvd GOSSIP message <" + std::to_string(msg->getGossipTimestamp()) + ">:<" +
              std::to_string(msg->getGossipOriginIP()) + ">:<" + std::to_string(msg->getGossipClientId()) + ">" +
              ", Local Timestamp: " + std::to_string(simTime().dbl()) + ", Received From: " + std::to_string(msg->getSourceId()));

    const bool shouldTerminate = (uniqueGossips >= N);

    int senderId = msg->getSourceId();
    for (int g = 0; g < gateSize("port"); ++g) {
        cGate *outGate = gate("port$o", g);
        if (!outGate->isConnected()) {
            continue;
        }

        cModule *neighbor = outGate->getPathEndGate()->getOwnerModule();
        int neighborId = neighbor->par("clientId").intValue();
        if (neighborId == senderId) {
            continue;
        }

        ChordAppMsg *copy = msg->dup();
        copy->setSourceId(myId);
        send(copy, "port$o", g);
    }

    delete msg;

    if (shouldTerminate) {
        logOutput("Node " + std::to_string(myId) + " received gossips from all N nodes. Terminating.");
        terminated = true;
    }
}

void ClientNode::handleMessage(cMessage *msg)
{
    if (terminated) {
        delete msg;
        return;
    }

    if (msg->isSelfMessage()) {
        if (std::string(msg->getName()) == "startTask") {
            initiateTask();
            delete msg;
            return;
        }
    }

    ChordAppMsg *cmsg = check_and_cast<ChordAppMsg *>(msg);
    if (cmsg->getMsgType() == TASK_MSG) {
        handleTaskMsg(cmsg);
    } else if (cmsg->getMsgType() == RESULT_MSG) {
        handleResultMsg(cmsg);
    } else if (cmsg->getMsgType() == GOSSIP_MSG) {
        handleGossipMsg(cmsg);
    } else {
        delete cmsg;
    }
}