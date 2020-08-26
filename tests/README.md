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
python3 tests/test_api_v1_access_token.py --url https://172.16.209.129 
OK: Response is valid
```

```commandline
python3 tests/test_api_v1_access_token.py -h
usage: test_api_v1_access_token.py [-h] --url URL [--username USERNAME]
                                   [--password PASSWORD]

Tests if pfSense APIs Access Token endpoint is operating normally

optional arguments:
  -h, --help           show this help message and exit
  --url URL            URL of the remote pfSense host to check
  --username USERNAME  Username to authenticate as
  --password PASSWORD  Password to authenticate with

```

## Output
Tests typically print an OK or ERROR status to the console depending on the tests outcome. These scripts will also
set the command's exit code to 1 upon error or 0 upon success.