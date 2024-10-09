WLPlan
======

[![PyPI version](https://badge.fury.io/py/wlplan.svg)](https://pypi.org/project/wlplan/)
[![License](https://img.shields.io/pypi/l/wlplan)](LICENSE)

WLPlan is a package for generating embeddings of PDDL planning problems for machine learning tasks. It supports both classical and numeric planning problems.

<p align="center">
    <img src="image.png" alt="WLPlan" title="WLPlan"/>
</p>

## Installation
### Python Interface
The Python interface can be installed simply with

    pip install wlplan

The PyPI release only supports `python>=3.10`. Alternatively, you can also install the package from the source code.

### C++ Interface
The C++ interface can be installed in your project by running

    ./cmake_build.py <path/to/installation>

and adding the following to the root CMakeLists.txt file of your project

    list(APPEND CMAKE_PREFIX_PATH "<path/to/installation>")
    find_package(wlplan)
    ...
    target_link_libraries(<your_project> PRIVATE wlplan)

## Graph Representations
The graph representations of planning tasks implemented thus far are 
| Name                                   | WLPlan shorthand | Reference                                                                                                                         |
| -------------------------------------- | ---------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| Instance Learning Graph (ILG)          | `ilg`            | D. Chen, F. Trevizan, S. Thiébaux:  Return to Tradition: Learning Reliable Heuristics with Classical Machine Learning. ICAPS 2024 |
| Numeric Instance Learning Graph (νILG) | `nilg`           | D. Chen, S. Thiébaux:  Graph Learning for Numeric Planning. NeurIPS 2024                                                          |

## Feature Generators
The feature generators implemented thus far are
| Name                                    | WLPlan shorthand | Reference                                                                                                                         |
| --------------------------------------- | ---------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| Weisfeiler Leman (WL)/colour refinement | `wl`             | D. Chen, F. Trevizan, S. Thiébaux:  Return to Tradition: Learning Reliable Heuristics with Classical Machine Learning. ICAPS 2024 |
| 2-WL                                    | `kwl2`           | J. Cai, M. Fürer, N. Immerman: An optimal lower bound on the number of variables for graph identification. Combinatorica (1992)   |
| Local 2-WL (2-LWL)                      | `lwl2`           | C. Morris, K. Kersting, P. Mutzel: Glocalized Weisfeiler-Lehman Graph Kernels: Global-Local Feature Maps of Graphs. ICDM 2017     |
| continuous-categorical WL (ccWL)        | `ccwl`           | D. Chen, S. Thiébaux:  Graph Learning for Numeric Planning. NeurIPS 2024                                                          |
| individualised WL (iWL)                 | `iwl`            | coming soon                                                                                                                       |
| normalised iWL (niWL)                   | `niwl`           | coming soon                                                                                                                       |

## Usage
Examples for how to use the package include this self-contained [notebook](docs/examples/blocksworld.ipynb) and [test](tests/test_train_eval_blocks.py), as well as the [GOOSE](https://github.com/DillonZChen/goose) framework.
