ToanMaze
An experimental maze pathfinding project exploring A and custom search strategies in C++.*

Overview
ToanMaze is an experimental project that began as an attempt to explore maze pathfinding performance.

The project started with the classic A* algorithm as a baseline. Two additional bidirectional search variants were then developed from my own ideas and implemented with the assistance of AI tools.

The original goal was to experiment with search strategies and compare their execution times on a computer. The initial results and implementation should be treated as early experiments, not as proof that the custom algorithms are universally faster or better than classic A*.

This repository preserves the original work as a starting point for future learning, testing, and redesign.

Algorithms
The current prototype contains three implementations:

Classic A* — the baseline pathfinding algorithm using the Manhattan distance heuristic.
AFastV1 Dual Search — an experimental bidirectional search approach.
V13 Pro — another experimental bidirectional search implementation based on the original design ideas.
The custom implementations are experimental. Their correctness, path optimality, and performance across different maze structures still require systematic verification.

Current Implementation
Language: C++
Maze representation: fixed-size 100 × 100 grid
Movement: four orthogonal directions
Maze generation: randomized wall placement
Heuristic: Manhattan distance
Benchmarking: repeated tests on randomly generated mazes
Path display: simplified paths that retain turning points
Limitations
This is an early prototype, and several aspects need further work:

Verify that every returned path is valid.
Check whether each algorithm finds a shortest path when one exists.
Test disconnected mazes and different maze structures.
Measure performance across multiple maze sizes and configurations.
Report path length separately from the number of simplified waypoints.
Improve memory management and benchmark reliability.
Previous benchmark results only describe the tested environment and conditions. They should not be interpreted as a general performance guarantee.

AI Assistance
The original algorithm ideas were explored by me, while AI tools assisted with translating those ideas into C++ implementations.

The current source code may not perfectly represent the original ideas or guarantee their intended properties. Understanding, verifying, and improving the implementations is part of the project's future development.

Future Plans
Review and document each algorithm.
Build correctness tests and edge-case tests.
Establish a fair benchmark against classic A*.
Improve implementation quality and memory management.
Revisit the original ideas and redesign the algorithms as my understanding of algorithms and C++ develops.
Project Status
Early experimental prototype — not production-ready.

This repository is a record of an early exploration into pathfinding, performance testing, and algorithm design. It is preserved for learning and future development rather than presented as a finished or proven optimization.

An idea worth revisiting, testing, and improving.
