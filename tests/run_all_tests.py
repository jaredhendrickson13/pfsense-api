# Copyright 2020 Jared Hendrickson
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
    try:
        results = subprocess.call(["python3", tests_dir.joinpath(f)] + sys.argv[1:])
    except KeyboardInterrupt:
        sys.exit(1)

    # If a test fails, set exit code to 1
    if results != 0:
        exit_code = 1

sys.exit(exit_code)