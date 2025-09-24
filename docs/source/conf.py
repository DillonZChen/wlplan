import wlplan


# -- Project information -----------------------------------------------------

author = "Dillon Z. Chen"
copyright = "2025, Dillon Z. Chen"
project = "wlplan"
version = wlplan.__version__

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

add_module_names = False
language = "en"
master_doc = "index"
nbsphinx_execute = "never"
templates_path = ["_templates"]
html_static_path = ["_static"]
html_css_files = ["custom.css"]
source_suffix = [".rst", ".md"]

# -- Options for HTML output ----------------------------------------------

html_theme = "sphinx_rtd_theme"
html_logo = "_static/logo.png"
html_favicon = "_static/favicon.png"
