# Steady State Circuit Simulation

A C++ implementation of **Graph Theory-based nodal analysis** for automatic computation of node voltages, element voltages, and element currents in any linear circuit — using the element-node incidence matrix formulation.

Developed as part of **EEN-102 Project**, supervised by **Prof. Vinay Pant, Department of Electrical Engineering, IIT Roorkee**.

---

## What This Project Does

Solving a circuit by hand gets tedious fast — especially when you have more than 3 nodes or mixed voltage and current sources. This program automates that entire process using a well-established mathematical framework from Graph Theory.

You describe your circuit (how elements connect, their admittances, source values) and the program computes everything: node voltages, element voltages, and element currents — for any circuit topology.

---

## Mathematical Foundation

The program implements the standard nodal admittance formulation:

| Matrix | Formula | Meaning |
|--------|---------|---------|
| `[Yn]` | `[A][ge][A]ᵀ` | Nodal admittance matrix |
| `[Isn]` | `[A][ge][Vse] - [A][ise]` | Equivalent nodal current source |
| `[Vn]` | `[Yn]⁻¹ [Isn]` | Node voltage solution |
| `[Ve]` | `[A]ᵀ [Vn]` | Element voltages |
| `[ie]` | `[ge](Ve - Vse) + ise` | Element currents |

Where:
- `[A]` — Element-node incidence matrix (reduced, ground node removed)
- `[ge]` — Diagonal admittance matrix of elements
- `[Vse]` — Element voltage source vector
- `[ise]` — Element current source vector

---

## Code Structure (Modular)

The code is organised into **7 independent modules**, each handling one step of the pipeline:

```
circuit_simulation.cpp
│
├── inputTopology()        → reads element start/end nodes
├── inputGeMatrix()        → reads admittance diagonal (ge)
├── inputVse()             → reads voltage source values
├── inputIse()             → reads current source values
│
├── buildIncidenceMatrix() → constructs [A], [A_matrix], [A_transpose]
├── computeYn()            → computes [Yn] = [A][ge][A']
├── computeIsn()           → computes [Isn] = [A][ge][Vse] - [A][ise]
│
├── invertMatrix()         → Gauss-Jordan matrix inversion of [Yn]
│
├── computeVn()            → solves [Vn] = [Yn]⁻¹[Isn]
├── computeVe()            → computes [Ve] = [A'][Vn]
└── computeIe()            → computes [ie] = [ge](Ve-Vse) + ise
```

---

## How to Compile and Run

### Requirements
- Any C++ compiler (g++, clang++, MSVC)
- No external libraries needed — standard C++ only

### Compile
```bash
g++ -o circuit_sim circuit_simulation.cpp
```

### Run
```bash
./circuit_sim
```

---

## Input Format

The program prompts you step by step. Here is what each input means:

```
Number of elements  → total branches/components in circuit
Number of nodes     → total nodes INCLUDING ground (node 0)

For each element:
  Starting node     → positive terminal node number
  Ending node       → negative terminal node number

GE matrix           → admittance (1/R) of each element (diagonal values only)
VSE values          → voltage source in series with each element (0 if none)
ISE values          → current source in parallel with each element (0 if none)
```

> **Node numbering:** Ground node = 0. Other nodes start from 1.

---

## Sample Test Cases

### Test Case 1 — 3-element, 3-node circuit

```
Number of elements : 3
Number of nodes    : 3

Element connections (Start Node >> End Node):
1 0
1 2
2 0

GE matrix (admittance diagonal):
4 5 6

VSE values:
0 -13 0

ISE values:
-75 0 13
```

**Expected Output:**
```
Yn matrix:
9  -5
-5  11

Isn: 10, 52

Vn (node voltages):  V1 = 5V,  V2 = 7V
Ve (element voltages): 5, -2, 7
ie (element currents): -55, 55, 55 (approximately)
```

---

### Test Case 2 — 4-element, 3-node circuit

```
Number of elements : 4
Number of nodes    : 3

Element connections (Start Node >> End Node):
1 0
1 0
1 2
2 0

GE matrix (admittance diagonal):
0.2 0.1 0.1 0.05

VSE values:
20 0 0 0

ISE values:
0 0 0 -4
```

**Expected Output:**
```
Yn matrix:
0.4  -0.1
-0.1  0.15

Isn: 4, 4

Vn (node voltages):  V1 = 20V,  V2 = 40V
Ve (element voltages): 20, 20, -20, 40
ie (element currents): 0, 2, -2, -2
```

---

## Limitations

- Supports **resistive circuits** with independent voltage and current sources
- Does not handle **dependent sources**, **capacitors**, or **inductors** (steady-state DC only)
- Matrix size limited to **50 elements / 50 nodes** (adjustable via array size constants)
- Assumes circuit is **connected** and **non-singular** (Yn must be invertible)

---

## Project Structure

```
steady-state-circuit-simulation/
│
├── circuit_simulation.cpp    # Main modular C++ source code
├── report/
│   └── 21115013_AMIT_GOGIA_P1.pdf   # Original project report
└── README.md
```

---

## Author

**Amit Gogia** | 21115013 | B.Tech Electrical Engineering, IIT Roorkee
Supervised by **Prof. Vinay Pant** | EEN-102 | Department of Electrical Engineering
