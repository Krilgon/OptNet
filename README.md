# OptNet

**Network Optimization Simulator**

OptNet is an interactive network optimization simulator focused on visualizing network topologies, routing paths, and traffic flow using graph-based models and optimization algorithms.

The project combines graph theory, optimization techniques (including genetic algorithms), and real-time browser-based visualization to provide an intuitive and extensible environment for studying network behavior and routing efficiency.

---

## Overview

The simulator represents a computer network as a graph consisting of heterogeneous node types and weighted edges.

- **Nodes** represent network entities such as PCs, compute nodes, servers, storage systems, and gateways.
- **Edges** represent communication links characterized by latency and bandwidth.

The system is designed to visualize both:
- static network structure, and
- dynamic processes such as optimized routing paths and packet-level traffic flow.

---

## Usage

Generate Network graph using GraphGenerator.exe,
Generate best path for this graph using GA.exe,

Start local python server from Powershell with next commands:
- cd (Directory of this project)
- py -m http.server
Then go to the page of local server to see visualization.

---

## Key Features

### Interactive Network Visualization
- Force-directed graph layout with physics-based stabilization
- Multiple node types with distinct shapes and colors
- Clear node labeling for improved readability
- Smooth zooming, panning, and hover interactions

### Routing Path Animation
- Visualization of optimized routing paths
- Single animated tracer moving node-to-node
- Progressive edge highlighting
- Visual emphasis of visited nodes without altering their base colors

### Packet Flow Simulation
- Lightweight packet animation along network links
- Packet speed derived from link latency
- Packet generation rate influenced by link bandwidth
- Built-in limits to prevent performance degradation

### Modular Architecture
- Separation of visualization and optimization logic
- JSON-based input for network topology and solutions
- Prepared for real-time backend integration

---

## Optimization Concept

OptNet is designed to visualize results produced by optimization algorithms, primarily **genetic algorithms**.

Typical optimization objectives include:
- Minimizing end-to-end latency
- Avoiding congested network links
- Balancing traffic distribution
- Identifying robust and efficient routing paths

At the current stage, optimization results are loaded from static JSON files.  
The architecture is prepared for future real-time optimization via a backend service.

---

## Technology Stack

### Frontend
- JavaScript (ES6)
- HTML5 / CSS3
- vis-network for graph rendering and interaction

### Data Representation
- JSON-based network topology definition
- JSON-based optimization result storage

### Planned Backend
- WebSocket-based communication
- Real-time genetic algorithm execution
- Streaming of intermediate optimization states

---


---

## How It Works

1. The network topology is loaded from a JSON file.
2. Optimization results are loaded independently.
3. The network is rendered using a physics-based layout.
4. Users can:
   - Animate the optimized routing path
   - Enable or disable packet flow simulation
   - Interactively explore the network structure

---

## Current Status

- Visualization system implemented and stable
- Path animation and packet flow simulation completed
- Genetic algorithm execution currently offline
- Backend integration planned

---

## Planned Improvements

- Live optimization execution via WebSockets
- Visualization of multiple competing paths
- Dynamic edge load and congestion indicators
- Adjustable algorithm parameters via UI
- Step-by-step playback of optimization generations
- Export of simulation results

---

## License

License information will be added at a later stage.


