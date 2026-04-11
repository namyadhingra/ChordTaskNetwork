cplusplus {{
#include <vector>
typedef std::vector<int> IntVector;
}}

class noncobject IntVector;

enum MsgType {
    TASK_MSG = 1;
    RESULT_MSG = 2;
    GOSSIP_MSG = 3;
}

packet ChordAppMsg {
    int msgType; // MsgType
    
    // Common routing
    int sourceId;
    int destId;
    
    // For TaskMsg
    int initiatorId;
    int subtaskId;
    IntVector data; // The array elements
    
    // For ResultMsg
    int maxResult;
    
    // For GossipMsg
    int gossipOriginIP; // We use node ID as IP (e.g. 192.168.0.x)
    double gossipTimestamp;
    int gossipClientId;
}
