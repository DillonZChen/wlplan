Installation
============

.. raw:: html

    <p class="flex-wrapper"><img src="https://raw.githubusercontent.com/DillonZChen/wlplan/refs/heads/main/favicon.png" style="height:2em; display:inline; padding-right:0.1em;">Plan is supported for `Python 3.10` and above</p>

Python Interface
----------------

WLPlan can be installed for Python simply by::

    pip install wlplan

C++ Interface
-------------

WLPlan can be installed for your C++ project downloading the source code from `GitHub <https://github.com/DillonZChen/wlplan>`__, followed by running in the root::

    ./cmake_build.py <path/to/installation>

and adding the following to the root CMakeLists.txt file of your project::

    list(APPEND CMAKE_PREFIX_PATH "<path/to/installation>")
    find_package(wlplan)
    ...
    target_link_libraries(<your_project> PRIVATE wlplan)
