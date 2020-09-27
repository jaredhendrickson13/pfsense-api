Tests
=====
This directory holds our unit tests. Any script or file that aides the testing of this projects functionality should be 
placed here. Ideally, each supported API call should have some sort of unit test behind it to ensure functionality does
not regress between releases.

## Overview
Unit tests are written using Python3. These are standalone scripts that can be run from any command line with Python3
installed. These scripts are using Python3's argparse package which includes an embedded help page by adding the `-h`
argument to your commands

## Example Usage
```commandline
$ python3 tests/test_api_v1_access_token.py --host https://172.16.209.129 
OK [POST /api/v1/access_token]: Response is valid
```

```commandline
$ python3 tests/test_api_v1_system_api.py --help
usage: test_api_v1_system_api.py [-h] --host HOST [--port {1-65535}] [--scheme {http,https}] [--username USERNAME] [--password PASSWORD] [--timeout TIMEOUT] [--verbose]

Check pfSense API's '/api/v1/system/api' endpoint for correct functionality.

optional arguments:
  -h, --help            show this help message and exit
  --host HOST           The host to connect to
  --port {1-65535}      The port to use when connecting
  --scheme {http,https}
                        The URL scheme to use when connecting
  --username USERNAME   Username to authenticate as.
  --password PASSWORD   Password to authenticate with
  --timeout TIMEOUT     Connection timeout limit in seconds
  --verbose             Display verbose output

```

## Output
Tests typically print an OK or ERROR status to the console depending on the tests outcome. These scripts will also
set the command's exit code to 1 upon error or 0 upon success.

## Notes
- For information on writing API unit tests, check the contributors guide at `docs/CONTRIBUTING.md`