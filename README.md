---
# Introduction
pfSense API is a fast, safe, full-fledged HTTP API. This works by leveraging the same PHP functions and processes used 
by pfSense's webConfigurator into API endpoints to create, read, update and delete pfSense configurations. 
All API endpoints enforce input validation to prevent invalid configurations from being made. Configurations made via 
API are properly written to the master XML configuration and the correct backend configurations are made preventing the 
need for a reboot. All this results in the fastest, safest, and easiest way to automate pfSense!

# Installation
To install pfSense API, simply run the following command from the pfSense shell:<br>
`pkg add https://github.com/jaredhendrickson13/pfsense-api/releases/download/v0.0.2/pfSense-2-4-pkg-API-0.0_2.txz`<br>

To uninstall, run the following command:<br>
`pkg delete pfSense-pkg-API`<br>

_Note: if you do not have shell access to pfSense, you can still install via the webConfigurator by navigating to 
'Diagnostics > Command Prompt' and enter the commands there_

# Requirements
- pfSense 2.4.4 or later is supported
- pfSense API requires a local user account in pfSense. The same permissions required to make configurations in the 
webConfigurator are required to make calls to the API endpoints
- While not an enforced requirement, it is STRONGLY recommended that you configure pfSense to use HTTPS instead of HTTP.
 This ensures that login credentials and/or API tokens remain secure in-transit

# Authentication
By default, pfSense API uses the same credentials as the webConfigurator. This behavior allows you to configure pfSense 
from the API out of the box, and user passwords may be changed from the API to immediately add additional security if 
needed. Alternatively, you can configure pfSense API to create secure API client IDs and tokens for API users. To 
generate, or delete API keys you can navigate to `System > API` in the UI after installation, and change the 
authentication mode to `API Token`.

To authenticate you need to use `client-id` and `client-token` must be specified in your payload to authenticate the API
 call. Your client-id and client-token will differ depending on which auth mode you have set in the General > API 
 settings page within the UI:

**Local database** is the default setting. This will allow you to authenticate using the same credentials as an existing
 UI user. 
Your `client-id` will be your UI username, and the `client-token` will be your UI password. 

For example, `https://localhost/api/v1/users/?client-id=admin&client-token=pfsense`

**Base64** will also allow you to use an existing UI user. The `client-id` _and_ `client-token` parameters must be Base64 
encoded in your payload. 
For example, `https://localhost/api/v1/users/?client-id=YWRtaW4K&client-token=cGZzZW5zZQo`

**API Token** will allow you to generate unique API tokens that will remain static. 

You can create an API token by setting the auth mode to API Token within the General > API settings page in the UI. 
You will need to save the changes, then a button to generate an API token will appear. 
You should get a dialogue containing the client-token (which will only be displayed once) and your client-id can be 
found in the API tokens table at the bottom of the page. 

For example, `https://localhost/api/v1/users/?client-id=61646d696e&client-token=4ef51f460b78cc1ff0d6667b447b3031`

# Response Codes
`200 (OK)` : API call succeeded<br>
`400 (Bad Request)` : An error was found within your requested parameters<br>
`401 (Unauthorized)` : API client has not completed authentiation or authorization successfully<br>
`403 (Forbidden)` : The API endpoint has refused your call. Commonly due to your access settings found in 
`System > API`<br>
`404 (Not found)` : Either the API endpoint or requested data was not found<br>
`500 (Server error)` : The API endpoint encountered an unexpected error processing your API request<br>

# Error Codes
A full list of error codes can be found by navigating to /api/v1/system/api/errors/ after installation. This will return
 JSON data containing each error code and their corresponding error message. No authentication is required to view the 
 error code library. This also makes API integration with third-party software easy as the API error codes and messages 
 are always just an HTTP call away!

# Rate limit
There is no limit to API calls at this time
