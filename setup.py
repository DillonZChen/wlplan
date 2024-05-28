# Available at setup time due to pyproject.toml
from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import find_packages, setup

__version__ = "0.2.1"

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

files = [
    glob("src/*.cpp"),
    glob("src/**/*.cpp"),
]

ext_modules = [
    Pybind11Extension(
        "_wlplan",
        sorted([f for file_group in files for f in file_group]),
        # Example: passing in the version to the compiled code
        define_macros=[("VERSION_INFO", __version__)],
    ),
]

setup(
    name="wlplan",
    version=__version__,
    author="Dillon Z. Chen",
    author_email="dillon.chen1@gmail.com",
    description="WLPlan: Relational Features for PDDL Planning",
    packages=find_packages(),
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    project_urls={"GitHub": "https://github.com/DillonZChen/wlplan"},
    license="MIT License",
    python_requires=">=3.10",
    install_requires=[
        "networkx>=3.0",
        "numpy>=1.26",
    ],
)
