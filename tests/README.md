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
Tests print an OK or ERROR status to the console depending on the tests outcome. These scripts will also
set the command's exit code to 1 upon error or 0 upon success.

```
OK ------> [ POST https://172.16.209.139:443/api/v1/user/privilege ]: Response is valid
```

## Running All Tests
`tests/run_all_tests.py` is a script that will run through all tests within the `tests/` directory. In order for
unit tests to be included, they must start with `test_` and end with `.py`. 

### Example
```commandline
$ python3 tests/run_all_tests.py --host 172.16.209.139
OK ------> [ POST https://172.16.209.139:443/api/v1/user/privilege ]: Response is valid
OK ------> [ DELETE https://172.16.209.139:443/api/v1/user/privilege ]: Response is valid
OK ------> [ GET https://172.16.209.139:443/api/v1/system/dns ]: Response is valid
OK ------> [ PUT https://172.16.209.139:443/api/v1/system/dns ]: Response is valid
OK ------> [ PUT https://172.16.209.139:443/api/v1/system/dns ]: Response is valid
OK ------> [ POST https://172.16.209.139:443/api/v1/services/ntpd/stop ]: Response is valid
OK ------> [ POST https://172.16.209.139:443/api/v1/services/ntpd/start ]: Response is valid
OK ------> [ POST https://172.16.209.139:443/api/v1/services/unbound/start ]: Response is valid
OK ------> [ GET https://172.16.209.139:443/api/v1/status/carp ]: Response is valid
OK ------> [ PUT https://172.16.209.139:443/api/v1/status/carp ]: Response is valid
...
```

## Notes
- For information on writing API unit tests, check the contributors guide at `docs/CONTRIBUTING.md`