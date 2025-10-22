:github_url: https://github.com/DillonZChen/wlplan

WLPlan Documentation
====================

.. raw:: html

    <p class="flex-wrapper"><img src="_static/favicon.png" style="height:2em; display:inline; padding-right:0.1em">Plan is a library for generating graph representations and feature embeddings of PDDL planning problems and states for machine learning tasks. WLPlan currently supports both classical and numeric planning problems.</p>

    <p>The main pipeline in WLPlan consists of (1) converting planning problems and states into graphs, and (2) synthesising feature embeddings by running a variant of the Weisfeiler-Leman (WL) algorithm on the resulting graph.</p>

    <p align="center">
        <img src="_static/pipeline.svg" alt="WLPlan" title="WLPlan"/>
    </p>

    <p>The entire pipeline of generating feature embeddings from PDDL planning problems can be performed in a matter of steps with WLPlan:</p>

.. code-block:: python

    feature_generator = init_feature_generator(domain)
    feature_generator.collect(dataset)
    X = feature_generator.embed(dataset)

.. toctree::
    :caption: Get Started
    :maxdepth: 1

    tutorials/installation
    tutorials/1_introduction.ipynb

.. toctree::
    :caption: Tutorials
    :maxdepth: 1

    tutorials/2_graphs.ipynb
    tutorials/3_gnns.ipynb

.. toctree::
    :caption: Package Reference
    :maxdepth: 1

    package/wlplan
    package/data
    package/feature_generator
    package/graph_generator
    package/planning
