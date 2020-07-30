# Example API Methods


## Base URL 

`https://pfsense-ip/api/v1`

# Endpoints

## Firewall

`/firewall/aliases/`
`/firewall/aliases/add/`
`/firewall/aliases/delete/`
`/firewall/aliases/modify/`


`/firewall/nat/`
`/firewall/nat/portforwards/`


`/firewall/rules/`
`/firewall/rules/add/`
`/firewall/rules/delete/`


`/firewall/states/`
`/firewall/states/size`


`/firewall/virtualips/`
`/firewall/virtualips/add/`
`/firewall/virtualips/delete/`

## Interfaces

`/interfaces/`
`/interfaces/add/`
`/interfaces/delete/`
`/interfaces/vlans/`
`/interfaces/vlans/add/`
`/interfaces/vlans/delete/`
`/interfaces/vlans/modify/`


## Routing

`routeing/gateways/`


## Services

## Status

## Users



# Usage examples

Code samples are shown in python.

```
import json
import requests
from requests.packages.urllib3.exceptions import InsecureRequestWarning
requests.packages.urllib3.disable_warnings(InsecureRequestWarning)

session = requests.Session()

base_url = 'https://192.168.1.1/api/v1'

client_id = 'admin'
client_token = '09123kjahsdyqwoi4y1w4o'

api_auth = {'client-id': client_id, 'client-token': client_token}

def request_api(endpoint='/', method='GET', verify=False, params={}, **kwargs):
    """
    :params endpoint: One of the endpoints defined above.
    :params method: HTTP method one of 'GET', 'POST', or 'DELETE'
    """
    
    # Automatically append our api_authentication
    params.update(kwargs.get('api_auth', api_auth))
    
    # Create a prepared request
    r = requests.Request(method.upper(), base_url + endpoint, params=params)  
    r = session.prepare_request(r)
    r = session.send(r, verify=verify)
    
    if r.status_code != 200:
        raise requests.HTTPError(r.reason)

    try:
        return r.json()
    except json.decoder.JSONDecodeError as e:
        # Return plain text version as we did not get a proper JSON response.
        return r.text
        
# Use the function above to repeatedly send API calls.

request_api('/firewall/aliases/')

```