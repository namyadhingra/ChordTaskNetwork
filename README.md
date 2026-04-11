# Distributed Task Execution and Gossip Protocol Simulation in OMNeT++

This project simulates a fully distributed, Peer-to-Peer network modeled after the CHORD Distributed Hash Table protocol. The primary objectives are to execute a distributed task (such as finding the maximum element in a large dataset) across multiple nodes simultaneously and to disseminate the results using an optimized Gossip protocol.

## 1. Code Structure Description

The project is structured around several distinct components that separate the network logic, message passing, and topological structure.

*   `generate_topo.py`: A Python script responsible for initially computing the topology. It generates a `config.txt` file containing the network parameters (number of nodes $N$, dataset size $k$, and subdivisions $x$) alongside a `topo.txt` file. The `topo.txt` file specifies the explicit edges establishing the optimal $O(\log N)$ CHORD routing links.
*   `ClientNode.ned` and `Coordinator.ned`: Network description files that define the modular structure of the nodes. Crucially, the `ClientNode` contains an unconstrained array of bidirectional gates called `port[]` to support variable topological connections at runtime.
*   `Network.ned` and `omnetpp.ini`: These files define the overarching network structure and specify runtime configurations.
*   `ChordAppMsg.msg`: The OMNeT++ language definition file orchestrating the packet structures. It supports three distinct message types: task delegation, result propagation, and gossip broadcast.
*   `Coordinator.h` and `Coordinator.cc`: A central class that intervenes during the zero stage of simulation initialization. It reads `topo.txt` and systematically constructs the physical gate connections between nodes using OMNeT++ delay channels to simulate physical linkages.
*   `ClientNode.h` and `ClientNode.cc`: The core behavioral logic for each peer. This class encapsulates distance-based greedy routing, dataset generation, task subdivision, local processing, result aggregation, and gossip dissemination.

## 2. Steps to Compile and Run

The codebase natively supports execution across both Linux and Windows environments operating OMNeT++.

### Step 2.1: Generating the Topological Data (Cross Platform)

It is highly recommended to build a fresh topological baseline before initiating the build process. Execute the Python script natively from any standard command prompt or terminal:

```bash
python generate_topo.py 16
```
*(On some Linux environments, you might need to invoke `python3` instead of `python`)*

This procedure constructs the `config.txt` and `topo.txt` datasets for 16 nodes. Evaluators are entirely permitted to manually edit `topo.txt` after this point to test customized graph link configurations without rerunning the generator.

### Step 2.2: Building the C++ Binaries

Depending on your platform, navigate to the root directory encompassing the simulation files and invoke the build sequences:

**For Windows Users:**
Open the specialized OMNeT++ shell (`mingwenv.cmd` located in your OMNeT++ installation directory), navigate to the project root, and execute:
```bash
opp_makemake -f --deep
make
```

**For Linux Users:**
Open your standard Linux terminal, ensure your OMNeT++ environment variables are sourced (e.g., executing `. setenv` from the OMNeT++ root), navigate to the project directory, and execute identical commands:
```bash
opp_makemake -f --deep
make
```

The first command recursively inspects header associations to construct a valid platform specific Makefile. The second command executes the compilation binding the components natively.

### Step 2.3: Executing the Simulation Environment

Simulation can be triggered visually via the OMNeT++ IDE by right-clicking the project and selecting "Run As -> OMNeT++ Simulation", or directly via the command line interfaces:

**For Windows:**
```bash
ChordTaskNetwork.exe
```

**For Linux:**
```bash
./ChordTaskNetwork
```

Once execution concludes, a file named `outputfile.txt` will be generated sequentially in the root directory. This file logs the exhaustive trace of internal behaviors corresponding to the computed responses and their associated gossip records natively.

## 3. Simulation Walkthrough: What, How, and Why

The following section explicitly defines the procedural timeline of the software.

### Step A: Network Initialization and Topology Binding

*   **What happens:** OMNeT++ initiates the simulation environment and triggers the `initialize()` function inside the `Coordinator` module.
*   **How it happens:** The Coordinator parses the `topo.txt` file and sequentially matches the specified node indices. Using the OMNeT++ `cGate::connectTo` library functions, the codebase programmatically bridges output and input ports between the specified client array indices.
*   **Why it happens:** Separating the edge linkage from the static `.ned` files allows evaluators to arbitrarily alter the network topology in simple text. It bypasses forcing the user to comprehend or rebuild rigid layout definitions.

### Step B: Staggered Task Initiation

*   **What happens:** Every individual client schedules an independent timer to begin the core task generation sequence.
*   **How it happens:** During their localized `initialize()` protocol, nodes instantiate a self executing message delayed by a randomized uniform interval, programmed as `scheduleAt(simTime() + uniform(0, 1.0))`.
*   **Why it happens:** If every component generated and mapped messages at precisely exact simulation second $t = 0$, the network would experience severe cascading congestion unrepresentative of distributed topologies. Implementing random staggering ensures robust and debuggable network traffic sequences.

### Step C: Task Distribution and Greedy Routing

*   **What happens:** When the staggered timer triggers, the client generates an arbitrary dataset of size $k$, partitions it into $x$ subtasks, and assigns subtask $i$ to destination node $i \pmod N$. 
*   **How it happens:** The node crafts a `TASK_MSG` packet encapsulating the array vector payload and launches the iterative routing mechanic. Conforming to the CHORD protocol, the node inspects its connected neighbor array rather than passing linearly adjacent items. It mathematically evaluates the numerical gap from its neighbors toward the strict destination and picks the connected peer providing the peak forward progress without bypassing the specific target.
*   **Why it happens:** This deterministic calculation ensures rigorous delivery strictly traversing maximum $O(\log N)$ hops utilizing the explicit bypass topology configured dynamically earlier, which curtails linear path latency.

### Step D: Subtask Execution and Aggregation Returning

*   **What happens:** The targeted client eventually intercepts the `TASK_MSG` structure, processes the requested mathematical maximum spanning the payload slice, and delegates out a `RESULT_MSG`. 
*   **How it happens:** Providing the destination parameter accurately signifies its identification limit, the client parses the array segment iteratively determining the climax boundary. Following extraction, it bounces the scalar variable backward recursively utilizing the identical distance routing hierarchy back to the initiator.
*   **Why it happens:** Dividing computation locally offloads central strains establishing fully capable distributed concurrent processing layers. 

### Step E: Global Consensus and Gossip Dissemination

*   **What happens:** The triggering node constantly tracks inward traveling `RESULT_MSG` structures. Provided the quantity equates to the designated task split size parameter $x$, it derives the globally conclusive maximum limit, writes trace definitions to output systems, and propels Gossip properties outward to identical peers.
*   **How it happens:** The initiator compiles a custom identifier broadcasting its origin IP parameters with literal execution timestamps. Provided surrounding nodes consume this format representing `GOSSIP_MSG` structures, they map the hash tuple into localized matrices checking against redundant duplications, record parameters locally, and copy identical packages to surrounding outputs.
*   **Why it happens:** Utilizing logical memory hash mechanisms eliminates infinite cyclical replications across undirected graphs strictly verifying only unique datasets continue replicating outward guaranteeing ubiquitous dissemination. 

### Step F: Complete Process Termination Handling

*   **What happens:** The framework establishes a silent block ceasing all future local computation parameters individually for node containers.
*   **How it happens:** Every node calculates the literal quantity of globally unique origin structures ingested. Once the peer verifies successfully acquiring gossip sequences originating natively from all $N$ distinct array originators, it activates termination triggers ignoring any incoming unverified packet.
*   **Why it happens:** Assures bounded memory operation indicating clear successful procedural validation directly across simulated outputs indicating operational finalization correctly.
