#!/bin/bash

# Exit on the first error
set -e

# Build with all threads
export MAKEFLAGS="-j$(nproc)"

# Install the package from sources
pip install . -v

# Make sure required tools are installed
pip install pybind11-stubgen

# Generate stubs
rm -rf _wlplan/*.pyi
pybind11-stubgen _wlplan -o .

# Generate documentation
pip install sphinx sphinx_rtd_theme
cd docs
rm -rf build/
make html
cd ..
