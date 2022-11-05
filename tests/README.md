Tests
=====
This directory holds our E2E tests. Any script or file that aids the testing of this project's functionality should be 
placed here. Ideally, each supported API call should have some sort of E2E test behind it to ensure functionality does
not regress between releases.

## Overview
E2E tests are written using Python3. These are standalone scripts that can be run from any command line with Python3
installed. These scripts are using Python3's argparse package which includes an embedded help page by adding the `-h`
argument to your commands

## Example Usage
```commandline
$ python3 tests/test_api_v1_access_token.py --host https://172.16.209.129 
OK [POST /api/v1/access_token]: Response is valid
```

```commandline
$ python3 tests/test_api_*.py --help
usage: test_api_*.py  [-h] --host HOST [--port {1-65535}] [--scheme {http,https}] [--auth_mode {local,token,jwt}] [--username USERNAME] [--password PASSWORD] [--timeout TIMEOUT] [--delay DELAY] [--verbose]

options:
  -h, --help            show this help message and exit
  --host HOST           The host to connect to
  --port {1-65535}      The port to use when connecting
  --scheme {http,https}
                        The URL scheme to use when connecting
  --auth_mode {local,token,jwt}
                        The API authentication mode to use.
  --username USERNAME   Username to authenticate as.
  --password PASSWORD   Password to authenticate with
  --timeout TIMEOUT     Connection timeout limit in seconds
  --delay DELAY         Delay between tests in seconds
  --verbose             Display verbose output


```

## Output
Tests print an OK, WARNING or ERROR status to the console depending on the tests outcome. These scripts will also
set the command's exit code to 1 upon error or 0 upon success or warning.

```
OK ------> [ POST https://172.16.209.139:443/api/v1/user/privilege ]: Response is valid
```

## Running All Tests
`tests/run_all_tests.py` is a script that will run through all tests within the `tests/` directory. In order for
E2E tests to be included, they must start with `test_` and end with `.py`. 

## Environment Requirements
E2E test are written to be executed against a fresh pfSense install. While precautions are taken to prevent dependency
on specific environment configurations, there are some environment requirements that must be met to run tests successfully:
- The pfSense-API package must be installed on the pfSense instance before running tests.
- Available interfaces on pfSense must be `em0`, `em1`, `em2`, and `em3`. Most virtualization software will use these 
by default for most virtual network adapters.
- pfSense must use the default DHCP enabled WAN interface. Testing routing and gateways requires the default DHCP dynamic
gateway to be present. Do not manually add any gateways or static routes before running tests.
- The default pfSense LAN interface must be up. In some virtualization software you will need to ensure the network adapter
is connected and the interface is up.
- pfSense must have public internet access and ability to resolve DNS. Some API calls make subsequent calls to upstream resources.
These tests will require public internet access and DNS resolution to work properly.

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
- For information on writing API E2E tests, check the contributors guide at `docs/CONTRIBUTING.md`
