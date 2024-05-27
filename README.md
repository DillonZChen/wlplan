WLPlan
======

WLPlan is a package for generating embeddings of PDDL planning problems for machine learning tasks.

## Installation
The Python interface for WLPlan can be installed simply by

    pip install wlplan

### Installing from source

To install from source, clone the repository and then run

    pip install .

To test whether your installation was successful, install the test dependencies and run the tests by running the following:

    pip install -r test-requirements.txt
    pytest

## Documentation
To generate the documentation locally for this package with Sphinx, clone the repository and then run the following commands:

    cd docs/
    pip install sphinx sphinx_rtd_theme
    make html

The full documentation can only be generated if the installation was successful. It can be viewed by opening `docs/_build/html/index.html` in your browser.

## References
For information about the technical details of the underlying algorithm, read the paper [here](https://dillonzchen.github.io/publications/chen-trevizan-thiebaux-icaps2024.pdf). The corresponding bib entry is

    @inproceedings{chen-trevizan-thiebaux-icaps2024,
        author       = {Dillon Z. Chen and
                        Felipe W. Trevizan and 
                        Sylvie Thi{\'{e}}baux},
        title        = {Return to Tradition: Learning Reliable Heuristics with Classical Machine Learning},
        booktitle    = {ICAPS},
        year         = {2024},
    }
