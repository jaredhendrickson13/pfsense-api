import requests
import argparse
import json
import sys
from urllib3.exceptions import InsecureRequestWarning

# Variables
requests.packages.urllib3.disable_warnings(category=InsecureRequestWarning)

# Arguments
parser = argparse.ArgumentParser(description='Tests if pfSense API was successfully installed on a remote host')
parser.add_argument('--url', dest="url", type=str, required=True,help='URL of the remote pfSense host to check')
args = parser.parse_args()

# Request
req_url = args.url + "/api/v1/system/api/errors/"
req = requests.get(req_url, verify=False)
if req.status_code == 200:
    try:
        req.json()
        print("OK: Response is valid")
        sys.exit(0)
    except json.decoder.JSONDecodeError:
        print("ERROR: Expected JSON response, recieved " + str(req.content))
        sys.exit(1)
else:
    print("ERROR: Expected status code 200, received " + str(req.status_code))
    sys.exit(1)