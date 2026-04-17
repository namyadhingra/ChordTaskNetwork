# CSL3080 Computer Networks Assignment--2
## Distributed Task Execution and Gossip Protocol Simulation (OMNeT++)

**Group Members**
- Namya Dhingra (B23CS1040)
- Bhargav Shekokar (B23CS1008)

This project simulates a fully peer-to-peer network in OMNeT++ inspired by the CHORD DHT overlay. Each client splits a task into $x$ subtasks (with $x>N$), routes each subtask $i$ to client $(i \bmod N)$ using CHORD-like shortcut links, aggregates results, and then disseminates a completion notice using a loop-free gossip protocol.

If you want the formal write-up (diagrams + pseudocode), see `report.tex` (compiled output: `report.pdf`).


## Demo (GIF preview)

GitHub reliably auto-plays animated GIFs in a README, so the demo is shown below as a GIF preview. Click it to open the full MP4:

[![Demo video preview](demo.gif)](results/omnetpp%20video.mp4)

---

## 0. Quick Start (TL;DR)

Build once:
```bash
export OMNETPP_HOME=~/omnetpp-6.0.3
source "$OMNETPP_HOME/setenv"

python3 generate_topo.py 16
opp_makemake -f --deep -e ChordTaskNetwork
make
```

Run (pick one):
```bash
# Headless / VM / SSH
./ChordTaskNetwork -u Cmdenv

# GUI (requires a display server)
./ChordTaskNetwork -u Qtenv
```

Output is written to `outputfile.txt`.

---

## 1. What the simulation does

### 1.1 Distributed task execution (max finding)
- Each client generates an array of size $k$.
- It partitions the array into $x$ chunks such that $k/x \ge 2$.
- Subtask $i$ is assigned to destination node $d=i \bmod N$.
- The destination computes the local maximum of its chunk and returns it to the initiator.
- The initiator computes the global maximum as the max over all $x$ returned values.

### 1.2 Gossip completion dissemination
After a client completes its full task (not a subtask), it generates a gossip message:

`<timestamp>:<origin_id_as_IP>:<client_id>`

Gossip forwarding is loop-free using a local message log (ML): on first receipt, forward to all neighbors except the sender; on duplicates, ignore.

---

## 2. Theory / design choices

### 2.1 Why add CHORD-style shortcuts?
With only a ring, sending a message to a particular node can take $\Theta(N)$ hops in the worst case. We reduce this by adding ``finger'' links (shortcuts) so that greedy forwarding can make large progress toward the destination.

### 2.2 Overlay model used here
The topology is a ring plus finger edges of the form:

- Ring: $(i,(i+1)\bmod N)$
- Fingers: $(i,(i+2^j)\bmod N)$ for $j \in \{0,1,\dots,\lfloor \log_2 N \rfloor\}$

The edge list is stored in `topo.txt` so evaluators can modify the overlay without changing code.

### 2.3 Routing rule (greedy distance)
Routing uses clockwise ring distance:

$\text{dist}(a,b)=(b-a+N)\bmod N$.

At each hop, a node forwards to a neighbor that reduces the remaining distance to the destination if possible; otherwise it uses the neighbor that provides the best fallback progress.

---

## 3. Project structure (key files)

| File | Purpose |
|---|---|
| `Network.ned` | Instantiates `client[N]` and the `coordinator` module |
| `ClientNode.ned` | Declares variable-size `port[]` gates (overlay degree can vary) |
| `Coordinator.cc` | Reads `topo.txt` and wires bidirectional links using delay channels |
| `ClientNode.cc/.h` | Task split, greedy routing, aggregation, gossip forwarding, termination |
| `ChordAppMsg.msg` | Defines `ChordAppMsg` with `TASK_MSG`, `RESULT_MSG`, `GOSSIP_MSG` |
| `generate_topo.py` | Generates ring + finger edges in `topo.txt` and writes `config.txt` |
| `config.txt` | Stores `N`, `k`, and `x` (read at runtime by clients) |
| `outputfile.txt` | Output log produced during a run |

---

## 4. Build and run

### 4.1 Prerequisites
- OMNeT++ 6.x (or newer)
- Python 3

### 4.2 Environment setup
```bash
export OMNETPP_HOME=~/omnetpp-6.0.3
source "$OMNETPP_HOME/setenv"
```

Confirm tools are visible:
```bash
which opp_makemake
```

### 4.3 Generate topology + config
```bash
python3 generate_topo.py 16
```

Notes:
- `generate_topo.py` writes `config.txt` and `topo.txt`.
- The current script keeps `k` and `x` fixed (defaults: `k=100`, `x=20`). If you want different values, edit `config.txt` (and ensure `k >= 2*x`).

### 4.4 Build
```bash
opp_makemake -f --deep -e ChordTaskNetwork
make
```

### 4.5 Run (choose one UI)

Headless / VM / SSH (recommended):
```bash
./ChordTaskNetwork -u Cmdenv
```

GUI (needs a display server):
```bash
./ChordTaskNetwork -u Qtenv
```

Interactive mode (OMNeT++ menu):
```bash
./ChordTaskNetwork
```

### 4.6 UI mode selection guide

| Situation | Recommended UI | Command |
|---|---|---|
| Headless VM / WSL / SSH | Cmdenv | `./ChordTaskNetwork -u Cmdenv` |
| Local desktop with display | Qtenv | `./ChordTaskNetwork -u Qtenv` |
| Not sure if GUI works | Cmdenv first | `./ChordTaskNetwork -u Cmdenv` |

To quickly check if a display is available on Linux:
```bash
echo $DISPLAY
```

### 4.7 (Optional) Set a default UI in `omnetpp.ini`

Add one of the following under `[General]`:
```ini
preferred-user-interface = Cmdenv
```
or
```ini
preferred-user-interface = Qtenv
```

### 4.8 View output
```bash
cat outputfile.txt
```

---

## 5. Copy/paste workflow (end-to-end)

```bash
export OMNETPP_HOME=~/omnetpp-6.0.3
source "$OMNETPP_HOME/setenv"

python3 generate_topo.py 16
opp_makemake -f --deep -e ChordTaskNetwork
make

./ChordTaskNetwork -u Cmdenv
cat outputfile.txt
```

---

## 6. Configuration and topology editing

### 5.1 Changing `N`
If you change `N`, keep these consistent:
- `omnetpp.ini`: set `*.N = <N>`
- `config.txt`: ensure `N=<N>`
- `topo.txt`: ensure all node IDs are in `[0, N-1]` and the graph remains connected

### 5.2 Editing `topo.txt`
`topo.txt` is an undirected edge list. The coordinator will create bidirectional links for each line `u v`.

Practical constraints:
- Keep the overlay connected (otherwise gossip may not reach every node).
- Ensure each node has at least one neighbor.

### 6.3 Changing `k` and `x`

Edit `config.txt` and keep the constraint `k >= 2*x` (enforced at runtime by `ClientNode.cc`).

---

## 7. Output format (what to look for)

You should see:
- Task initiation: `Node X initiating task, splitting k elements into x parts.`
- Local subtask computation: `Node D computed local max ... for subtask i from initiator X`
- Result receipts: `Node X received result ... for subtask i`
- Completion: `Task Complete on Node X. Conclusive Max: ...`
- Gossip receipts (first time only):

  `Node U recvd GOSSIP message <t>:<origin>:<origin>, Local Timestamp: ..., Received From: ...`

Typical runtime for the default configuration (`N=16, k=100, x=20`) is a few seconds on a modest machine.

---

## 8. Troubleshooting

### 7.1 Build issues
| Symptom | Likely cause | Fix |
|---|---|---|
| `opp_makemake: command not found` | OMNeT++ env not sourced | `source "$OMNETPP_HOME/setenv"` |
| `Permission denied: ./ChordTaskNetwork` | Executable bit missing | `chmod +x ChordTaskNetwork` |
| Compile fails after changing `.msg` | Generated message files out of date | `make clean && opp_makemake -f --deep -e ChordTaskNetwork && make` |

### 7.2 Runtime issues
| Symptom | Likely cause | Fix |
|---|---|---|
| GUI fails (EGL/MESA / no window) | No display server | Run `./ChordTaskNetwork -u Cmdenv` |
| Running without `-u` seems to hang | OMNeT++ is waiting for UI selection / GUI startup issues | Use `./ChordTaskNetwork -u Cmdenv` |
| `Configuration error: k must be >= 2*x` | `config.txt` invalid | Increase `k` or decrease `x` |
| `Node X has no route to destination Y` | Overlay disconnected / missing neighbors | Fix `topo.txt` to keep the graph connected |
| Nodes never terminate | Gossip not reaching all nodes | Ensure `topo.txt` is connected and `N` matches everywhere |

---

## 9. Simulation walkthrough (event timeline)

This section explains the run sequence at a high level and maps it to the main modules.

1. **Topology wiring (Coordinator, init stage 0)**
	- `Coordinator.cc` reads `topo.txt` and connects `client[u].port[] <-> client[v].port[]` with a small delay channel.
	- This makes the overlay editable without regenerating NED files.

2. **Staggered start (ClientNode initialization)**
	- Each node schedules a self-message `startTask` at `simTime() + uniform(0,1)`.
	- Staggering reduces synchronized bursts at time 0 and produces cleaner traces.

3. **Task creation and subtask routing**
	- On `startTask`, a node generates `k` random values and partitions them into `x` chunks.
	- For each subtask `i`, destination is `i % N` and the payload is routed using greedy clockwise distance over the overlay.

4. **Subtask compute and result return**
	- The destination node computes the local max of the received chunk and sends a `RESULT_MSG` back to the initiator.
	- Return routing uses the same greedy forwarding.

5. **Aggregation and completion gossip**
	- After receiving all `x` results, the initiator logs the global maximum and emits a `GOSSIP_MSG`.
	- Each node forwards each unique gossip once (ML-based duplicate suppression) and terminates after collecting gossips from all `N` origins.

---

## 10. Submission packaging (as per handout)

Create the archive in the required format (update roll numbers accordingly):
```bash
tar -czvf rollno1-rollno2.tar.gz .
```

During evaluation, only `topo.txt` is intended to be modified; changes to code may be penalized per assignment rules.

---

## 11. References

1. OMNeT++ Documentation: https://doc.omnetpp.org/
2. Stoica et al., "Chord: A Scalable Peer-to-peer Lookup Service for Internet Applications", SIGCOMM 2001.
3. CSL3080 Assignment--2 handout (Distributed Task Execution and Gossip Protocol Simulation).
