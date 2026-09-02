[![banner 2025](banner2025.png)](2025/)

### Highlights (OCaml)

#### Arithmetic and Counting

- **Day 2, Part 2**
  - **Repeated-block number counting:** [Prime factors](2025/q2/q2b.ml#L6) identify the possible repetition structures, while [decimal masks](2025/q2/q2b.ml#L17) act as multiplicative repetition generators and [bounded arithmetic sums](2025/q2/q2b.ml#L41) count their values without enumerating the ranges.

#### Graph and State Algorithms

- **Day 8, Part 1**
  - **Kruskal-style component clustering:** The solve [constructs and sorts every weighted 3D edge](2025/q8/q8a.ml#L22), then [merges hashmap-backed component sets](2025/q8/q8a.ml#L60) until the requested number of components remains.

- **Day 8, Part 2**
  - **Kruskal-style full-connectivity search:** The same [component-merging process](2025/q8/q8b.ml#L60) continues until one component remains, allowing the [final accepted edge](2025/q8/q8b.ml#L64) to provide the result without constructing a separate spanning-tree object.

- **Day 10, Part 1**
  - **BFS over Set-XOR states:** Each operation applies a [symmetric-difference toggle](2025/q10/q10a.ml#L1), while a [visited-state hashmap and breadth-first search](2025/q10/q10a.ml#L37) find the minimum number of operations.

- **Day 10, Part 2**
  - **Parity decomposition with memoized halving:** Operations are [grouped by their parity effects](2025/q10/q10b.ml#L36), allowing the remaining values to be [recursively halved and memoized](2025/q10/q10b.ml#L100).

- **Day 11, Part 1**
  - **Lanternfish-style path counting:** The solve [propagates counts between graph nodes](2025/q11/q11a.ml#L26) instead of constructing each individual path.

- **Day 11, Part 2**
  - **State-aware path counting:** [Count vectors are propagated through the graph](2025/q11/q11b.ml#L27) while tracking whether paths have visited either, both, or neither of two required nodes.

#### Spatial Algorithms

- **Day 9, Part 2**
  - **Coordinate compression:** [Large coordinates are mapped](2025/q9/q9b.ml#L3) into a compact grid containing only the boundaries relevant to the search.
  - **Scanline rasterization:** [Horizontal and vertical intersections are converted into merged intervals](2025/q9/q9b.ml#L82) to fill the polygon interior.
  - **Summed-area table:** A [2D prefix sum](2025/q9/q9b.ml#L145) supports constant-time rectangle containment checks.

- **Day 12**
  - **Area-bound polygon packing:** [Occupied-area and bounding-box limits](2025/q12/q12.ml#L37) classify the input-specific packing cases without enumerating every rotation, reflection, and placement.

---

[![banner 2024](banner2024.png)](2024/)

### Highlights (C)

#### Data Structures and Systems

- **Day 11, Part 2**
  - **64-bit FNV open-addressing hashmap:** The implementation combines [64-bit FNV hashing](2024/q11/q11b.c#L83), [linear probing](2024/q11/q11b.c#L95), and [load-factor resizing](2024/q11/q11b.c#L122).
  - **Lanternfish-style frequency propagation:** The solve [folds counts directly into successor values](2024/q11/q11b.c#L181) instead of instantiating the exponentially growing transition graph.

- **Day 11 pthread Experiment**
  - **Parallel expansion:** [Thread-local caches and output buffers](2024/q11/q11b.threads.c#L249) allow branches to expand independently before [batched thread joins](2024/q11/q11b.threads.c#L395) combine their results.
  - **Shared pointer reallocation:** A [stable shared-pointer holder](2024/q11/q11b.threads.c#L225) and [safe reallocation helper](2024/q11/q11b.threads.c#L238) allow the underlying allocation to move while its current address remains accessible.

- **Day 16, Part 2**
  - **Binary min-heap:** The [custom priority queue](2024/q16/q16b.c#L29) implements the heap operations covered in [Stanford CS161 Lecture 4](https://web.stanford.edu/class/archive/cs/cs161/cs161.1168/lecture4.pdf).
  - **Separate-chaining state hashmap:** [Byte-buffer keys and `void *` values](2024/q16/q16b.c#L144) store position-and-direction search states.
  - **Direction-aware Dijkstra:** The search [runs Dijkstra over position-and-facing states](2024/q16/q16b.c#L428), retains [every equal-cost predecessor](2024/q16/q16b.c#L468), and walks that graph backward to find every position on an optimal route.

- **Day 17, Part 2**
  - **3-bit virtual machine:** [Function-pointer tables](2024/q17/q17b.c#L66) dispatch operands and opcodes, while [execution tracing](2024/q17/q17b.c#L142) records the changing machine state.
  - **Reverse base-8 search:** The input register is [reconstructed three bits at a time](2024/q17/q17b.c#L272), pruning prefixes that cannot reproduce the required output.

- **Day 18, Part 2**
  - **Generic key-value hashmap:** The separate-chaining map accepts [caller-provided key conversion and cleanup behavior](2024/q18/q18b.c#L125), allowing it to support different key and value types.
  - **Binary search with A\* reachability:** An [A* search](2024/q18/q18b.c#L363) acts as the reachability check inside a [binary search](2024/q18/q18b.c#L491) for the first input prefix that makes the destination unreachable.

- **Day 23, Part 2**
  - **Generic separate-chaining hashmap:** The implementation supports [arbitrary pointer-backed keys and `void *` values](2024/q23/q23bbk.c#L9) through [key conversion and cleanup callbacks](2024/q23/q23bbk.c#L213).
  - **DAG-adapted Bron–Kerbosch search:** A [fixed edge ordering](2024/q23/q23bbk.c#L389) gives every clique one increasing traversal, while an [explicit backtracking stack and candidate-set intersections](2024/q23/q23bbk.c#L423) find the maximum clique. The approach follows the clique-search material from [UCI ICS 163](https://ics.uci.edu/~eppstein/163/lecture6b.pdf).

#### Graph and State Algorithms

- **Day 6, Part 2**
  - **Candidate-pruned segment loop detection:** Only [positions from the original route](2024/q6/q6b.c#L49) are tested, and each simulation detects loops through repeated [directed segment states](2024/q6/q6b.c#L94).

- **Day 15, Part 2**
  - **Dependency-closure movement:** A [queue discovers every coupled object](2024/q15/q15b.c#L129) required by a movement, rejects the complete dependency set on collision, and [commits successful updates in reverse order](2024/q15/q15b.c#L170).

- **Day 19, Part 2**
  - **Trie search with memoized suffix counting:** A [trie shares common prefixes](2024/q19/q19b.c#L49), while [suffix memoization](2024/q19/q19b.c#L157) folds together the number of valid completions from every matching branch.

- **Day 20, Part 2**
  - **Ordered-path indexing:** The solve [maps each position to its distance along the path](2024/q20/q20b.c#L274), then [compares path distance with Manhattan distance](2024/q20/q20b.c#L328) instead of running a new path search for every candidate.

- **Day 21, Part 2**
  - **Lanternfish-style instruction expansion:** [Unique instruction fragments and their frequencies](2024/q21/q21b.c#L425) are repeatedly [folded through the transformation graph](2024/q21/q21b.c#L463) instead of constructing the exponentially growing command string.

#### Numerical Methods and Reverse Engineering

- **Day 13, Part 2**
  - **Linear-system solving:** [Dynamic matrix operations](2024/q13/q13b.c#L26) support implementations of [Gaussian elimination](2024/q13/q13b.c#L125) and [Gauss–Jordan elimination](2024/q13/q13b.c#L140) for solving the resulting systems of equations.

- **Day 14, Part 2**
  - **Chinese Remainder Theorem alignment:** The two periodic axes are [solved independently](2024/q14/q14b_crt.c#L109), then their residues are [combined with the Chinese Remainder Theorem](2024/q14/q14b_crt.c#L145).

- **Day 24, Part 2**
  - **Ripple-carry-adder structural reverse engineering:** The solve [encodes the expected full-adder relationships](2024/q24/q24b.c#L310), [constructs the circuit graph](2024/q24/q24b.c#L335), and [verifies the 45-bit carry chain](2024/q24/q24b.c#L420) to locate swapped outputs.
