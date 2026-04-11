#ifndef __CLIENTNODE_H
#define __CLIENTNODE_H

#include <omnetpp.h>
#include <map>
#include <set>
#include <vector>
#include "ChordAppMsg_m.h"

using namespace omnetpp;

class ClientNode : public cSimpleModule
{
  protected:
    int myId;
    int N;
    int x;
    int k;
    
    int numTasksReceived;
    int overallMax;
    
    std::set<std::pair<int, double>> receivedGossips;
    int uniqueGossips;
    bool terminated;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    
    void routeMessage(int destId, ChordAppMsg *msg);
    void handleTaskMsg(ChordAppMsg *msg);
    void handleResultMsg(ChordAppMsg *msg);
    void handleGossipMsg(ChordAppMsg *msg);
    
    void initiateTask();
    void logOutput(std::string text);
};

#endif
