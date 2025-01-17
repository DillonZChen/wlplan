import os


def test_not_debug():
    CUR_DIR = os.path.dirname(os.path.abspath(__file__))
    ROOT_DIR = os.path.normpath(os.path.join(CUR_DIR, ".."))

    SETUP_SCRIPT = os.path.join(ROOT_DIR, "setup.py")
    with open(SETUP_SCRIPT, "r") as f:
        setup_script = f.read()

    found_debug = False
    for line in setup_script.split("\n"):
        toks = line.split("#")
        if len(toks) > 0 and "-DDEBUGMODE" in toks[0]:
            found_debug = True
            break

    assert not found_debug, "Found -DDEBUGMODE in setup.py"
