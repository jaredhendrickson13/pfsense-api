# Copyright 2022 Jared Hendrickson
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Script that runs every test*.py test in the 'tests' directory."""
import pathlib
import subprocess
import os
import sys


def get_exit_code():
    """Runs all tests. Returns 0 if all tests pass, or returns 1 if at least 1 test failed."""
    tests_dir = pathlib.Path(__file__).parent.absolute()
    tests = sorted([f for f in os.listdir(tests_dir) if f.startswith("test") and f.endswith(".py")])
    success_count = 0
    fail_count = 0
    exit_code = 0

    # Run each test within the tests directory. Tests must start with 'test' and end with '.py' to be included.
    for test in tests:
        try:
            results = subprocess.call(["python3", tests_dir.joinpath(test)] + sys.argv[1:])
        except KeyboardInterrupt:
            sys.exit(1)

        # If a test fails, set exit code to 1
        if results != 0:
            fail_count += 1
            exit_code = 1
        else:
            success_count += 1

    # Print the number successful/failed tests
    print(
        f"------------------------------------------------------------------------"
        f"Ran all tests: {success_count}/{success_count + fail_count} tests passed."
    )
    return exit_code


sys.exit(get_exit_code())
