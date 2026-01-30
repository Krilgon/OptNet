# OptNet
Network Optimization Simulator

An interactive network optimization and visualization project that combines graph theory, genetic algorithms, and real-time animated network simulation in the browser.

The project focuses on exploring optimal routing paths, visualizing traffic flow, and providing an intuitive way to understand how network topology and parameters influence performance.

🚀 Features

Interactive Network Visualization

Graph-based network representation

Multiple node types (PC, Compute, Server, Storage, Gateway)

Clear color-coding and labeling for each node category

Path Optimization Visualization

Best path highlighting based on optimization results

Smooth animated traversal using a single moving tracer

Progressive edge and node highlighting along the route

Real-Time Packet Flow Simulation

Lightweight animated packets flowing between nodes

Packet speed depends on edge latency

Packet generation rate depends on edge bandwidth

Performance-friendly design (no uncontrolled particle spawning)

Physics-Based Layout

Automatic graph layout using force-directed physics

Stable, readable topology even for dense networks

Modular Architecture

Decoupled visualization and optimization logic

Ready for backend integration (WebSocket / API)

Easy to extend with new algorithms or metrics

🧠 Optimization Concept

The simulator is designed to visualize results produced by a Genetic Algorithm (GA) or other optimization techniques.

Typical optimization goals include:

Minimizing total latency

Avoiding congested links

Balancing traffic across the network

Finding robust and efficient routing paths

Currently, optimization results are loaded from JSON files, but the architecture is prepared for live backend-driven optimization.

🖥️ Tech Stack

Frontend

Vanilla JavaScript

HTML5 / CSS3

vis-network
 for graph rendering

Data Format

JSON-based graph and solution descriptions

Backend (planned)

WebSocket-based GA execution

Real-time updates of optimization generations

⚙️ How It Works

The network topology is loaded from input_graph.json

Optimization results (best path) are loaded from best_path.json

The graph is rendered with physics-based layout

Users can:

Animate the optimized path

Toggle real-time packet flow

Inspect nodes and connections interactively

🧪 Current Status

✅ Core visualization complete

✅ Path animation and packet simulation implemented

⚠️ Genetic Algorithm execution currently offline

🛠 Backend integration planned

🧩 Planned Improvements

Live GA execution via WebSockets

Visualization of multiple competing paths

Edge load heatmaps

Traffic congestion indicators

UI controls for algorithm parameters

Step-by-step GA generation playback

📜 License

This project is intended for educational and research purposes.
License will be added later.
