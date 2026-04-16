#include "ClientNode.h"

#include <algorithm>
#include <fstream>
#include <limits>
#include <string>

using namespace chord;

Define_Module(ClientNode);

namespace {
int ringDistance(int fromId, int toId, int networkSize)
{
    return (toId - fromId + networkSize) % networkSize;
}
}

void ClientNode::initialize()
{
    myId = par("clientId").intValue();
    N = getParentModule()->par("N").intValue();

    numTasksReceived = 0;
    overallMax = std::numeric_limits<int>::min();
    uniqueGossips = 0;
    terminated = false;

    x = 20;
    k = 100;

    std::ifstream config("config.txt");
    std::string line;
    while (std::getline(config, line)) {
        if (line.rfind("x=", 0) == 0) {
            x = std::stoi(line.substr(2));
        } else if (line.rfind("k=", 0) == 0) {
            k = std::stoi(line.substr(2));
        }
    }

    if (x <= 0) {
        throw cRuntimeError("Configuration error: x must be positive");
    }
    if (k < 2 * x) {
        throw cRuntimeError("Configuration error: k must be >= 2*x. Current k=%d, x=%d (k/x=%d)", k, x, k / x);
    }

    if (myId == 0) {
        std::ofstream out("outputfile.txt", std::ios_base::trunc);
    }

    scheduleAt(simTime() + uniform(0, 1.0), new cMessage("startTask"));
}

void ClientNode::logOutput(std::string text)
{
    EV << text << "\n";

    std::ofstream out("outputfile.txt", std::ios_base::app);
    out << text << "\n";
}

void ClientNode::initiateTask()
{
    logOutput("Node " + std::to_string(myId) + " initiating task, splitting " + std::to_string(k) + " elements into " + std::to_string(x) + " parts.");

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
        msg->setDataArraySize(count);
        for (int c = 0; c < count; ++c) {
            msg->setData(c, intuniform(1, 1000));
        }

        routeMessage(msg->getDestId(), msg);
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
        int neighborId = neighbor->par("clientId").intValue();

        if (neighborId == myId) {
            continue;
        }

        int distance = ringDistance(neighborId, destId, N);

        if (neighborId == avoidId && avoidGateIndex == -1) {
            avoidGateIndex = g;
        }

        if (neighborId != avoidId && distance < bestDist) {
            bestDist = distance;
            bestGateIndex = g;
        }

        if (distance < fallbackDist) {
            fallbackDist = distance;
            fallbackGateIndex = g;
        }
    }

    if (bestGateIndex == -1) {
        bestGateIndex = fallbackGateIndex;
    }
    if (bestGateIndex == -1) {
        bestGateIndex = avoidGateIndex;
    }

    if (bestGateIndex == -1) {
        throw cRuntimeError("Node %d has no route to destination %d", myId, destId);
    }

    msg->setSourceId(myId);
    send(msg, "port$o", bestGateIndex);
}

void ClientNode::handleTaskMsg(ChordAppMsg *msg)
{
    if (myId == msg->getDestId()) {
        int localMax = std::numeric_limits<int>::min();
        int dataSize = msg->getDataArraySize();

        for (int i = 0; i < dataSize; ++i) {
            localMax = std::max(localMax, msg->getData(i));
        }

        logOutput("Node " + std::to_string(myId) + " computed local max " + std::to_string(localMax) +
                  " for subtask " + std::to_string(msg->getSubtaskId()) +
                  " from initiator " + std::to_string(msg->getInitiatorId()));

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

void ClientNode::handleResultMsg(ChordAppMsg *msg)
{
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