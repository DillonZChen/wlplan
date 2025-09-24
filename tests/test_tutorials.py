import pathlib

import nbformat
from nbconvert.preprocessors import ExecutePreprocessor
import pytest


TUTORIALS = [
    "1_introduction",
    "2_graphs",
    "3_gnns",
]


@pytest.mark.parametrize("tutorial", TUTORIALS)
def test_notebook_exec(tutorial: str):
    """Only tests that the notebook can be executed with no errors."""

    path_to_dir = pathlib.Path(__file__).parent.absolute()
    run_path = f"{path_to_dir}/../docs/source/tutorials"
    notebook = f"{run_path}/{tutorial}.ipynb"

    with open(notebook) as f:
        nb = nbformat.read(f, as_version=4)
        ep = ExecutePreprocessor(timeout=600, kernel_name="python3")
        try:
            resources = {"metadata": {"path": run_path}}
            assert_msg = f"Got empty notebook for {notebook}"
            assert ep.preprocess(nb, resources=resources) is not None, assert_msg
        except Exception:
            assert False, f"Failed executing {notebook}"


if __name__ == "__main__":
    test_notebook_exec()
