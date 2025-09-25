import os
from ast import main
from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup


def main():
    # Read version from wlplan/__version__.py file
    version_ns = {}
    with open("wlplan/__version__.py") as f:
        exec(f.read(), version_ns)
    __version__ = version_ns["__version__"]

    # Sort input source files if you glob sources to ensure bit-for-bit
    # reproducible builds (https://github.com/pybind/python_example/pull/53)
    source_files = sorted(
        [f for pattern in ["src/*.cpp", "src/**/*.cpp", "src/**/**/*.cpp"] for f in glob(pattern)]
    )

    ext_module = Pybind11Extension(
        "_wlplan",
        source_files,
        define_macros=[("WLPLAN_VERSION", __version__)],
    )
    if os.getenv(key="DEBUG", default="").upper() in ["ON", "1", "YES", "TRUE", "Y"]:
        ext_module._add_cflags(["-DDEBUGMODE", "-O0", "-g"])

    setup(
        name="wlplan",
        version=__version__,
        author="Dillon Z. Chen",
        author_email="dillon.chen1@gmail.com",
        description="WLPlan: Features for PDDL Planning",
        long_description=open("README.md").read(),
        long_description_content_type="text/markdown",
        packages=["wlplan", "_wlplan"],
        package_data={"_wlplan": ["py.typed", "*.pyi", "**/*.pyi"]},
        ext_modules=[ext_module],
        cmdclass={"build_ext": build_ext},
        project_urls={"GitHub": "https://github.com/DillonZChen/wlplan"},
        license="MIT License",
        classifiers=[
            "Intended Audience :: Developers",
            "Intended Audience :: Education",
            "Intended Audience :: Science/Research",
            "Topic :: Scientific/Engineering",
            "Topic :: Scientific/Engineering :: Mathematics",
            "Topic :: Scientific/Engineering :: Artificial Intelligence",
            "Topic :: Software Development",
            "Topic :: Software Development :: Libraries",
            "Topic :: Software Development :: Libraries :: Python Modules",
            "Programming Language :: C++",
            "Programming Language :: Python :: 3",
        ],
        keywords="wlplan, automated planning, machine learning",
        python_requires=">=3.10",
        install_requires=[
            "networkx",
            "pddl==0.4.1",
            "python-sat==1.8.dev14",
        ],
    )


if __name__ == "__main__":
    main()
