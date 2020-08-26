import requests
import argparse
import json
import sys
from urllib3.exceptions import InsecureRequestWarning

# Variables
requests.packages.urllib3.disable_warnings(category=InsecureRequestWarning)

# Arguments
parser = argparse.ArgumentParser(description='Tests if pfSense APIs Access Token endpoint is operating normally')
parser.add_argument('--url', dest="url", type=str, required=True,help='URL of the remote pfSense host to check')
parser.add_argument('--username', dest="username", type=str, default="admin", help='Username to authenticate as')
parser.add_argument('--password', dest="password", type=str, default="pfsense", help='Password to authenticate with')

args = parser.parse_args()

# Request
req_url = args.url + "/api/v1/access_token/"
req_payload = {"client-id": args.username, "client-token": args.password}
req = requests.request("POST", url=req_url, data=json.dumps(req_payload), verify=False)
if req.status_code == 200:
    try:
        req.json()
    except json.decoder.JSONDecodeError:
        print("ERROR: Expected JSON response, recieved " + str(req.content))
        sys.exit(1)

    if req.json()["return"] == 0:
        print("OK: Response is valid")
        sys.exit(0)
    else:
        print("ERROR: Expected return code of 0, received" + str(req.json()["return"]))
        sys.exit(1)
else:
    print("ERROR: Expected status code 200, received " + str(req.status_code))
    sys.exit(1)