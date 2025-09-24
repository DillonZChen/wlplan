# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html


# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
import os
import sys


sys.path.insert(0, os.path.abspath(".."))

# -- Project information -----------------------------------------------------

project = "wlplan"
copyright = "2025, Dillon Z. Chen"
author = "Dillon Z. Chen"

# -- General configuration ---------------------------------------------------

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autosummary",
    "sphinx.ext.napoleon",
    "sphinx_rtd_theme",
    "myst_parser",
    "nbsphinx",
]

templates_path = ["_templates"]
html_static_path = ['_static']
html_css_files = ['custom.css']
source_suffix = ['.rst', '.md']
master_doc = "index"
language = "en"
add_module_names = False
nbsphinx_execute = 'never'

# -- Options for HTML output ----------------------------------------------

html_theme = "sphinx_rtd_theme"
html_logo = "https://raw.githubusercontent.com/DillonZChen/wlplan/refs/heads/main/wlplan.png"
html_favicon = "https://raw.githubusercontent.com/DillonZChen/wlplan/refs/heads/main/favicon.png"
htmlhelp_basename = "wlplandoc"
