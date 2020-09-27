import pathlib
import subprocess
import os
import sys

# Variables
tests_dir = pathlib.Path(__file__).parent.absolute()
tests = [f for f in os.listdir(tests_dir) if f.startswith("test") and f.endswith(".py")]
success_count = 0
exit_code = 0

# Run each test within the tests directory. Tests must start with 'test' and end with '.py' to be included.
for f in tests:
    results = subprocess.call(["python3", tests_dir.joinpath(f)] + sys.argv[1:])
    # If a test fails, set exit code to 1
    if results != 0:
        exit_code = 1

sys.exit(exit_code)