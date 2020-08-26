---
# Introduction
pfSense API is a fast, safe, full-fledged HTTP API. This works by leveraging the same PHP functions and processes used
by pfSense's webConfigurator into API endpoints to create, read, update and delete pfSense configurations. All API
endpoints enforce input validation to prevent invalid configurations from being made. Configurations made via API are
properly written to the master XML configuration and the correct backend configurations are made preventing the need for
 a reboot. All this results in the fastest, safest, and easiest way to automate pfSense!

# Installation
To install pfSense API, simply run the following command from the pfSense shell:<br>
`pkg add https://github.com/jaredhendrickson13/pfsense-api/releases/download/v0.0.3/pfSense-2-4-pkg-API-0.0_3.txz`<br>

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
needed. After installation, you can navigate to System > API in the pfSense webConfigurator to configure API
authentication.

To authenticate your API call, follow the instructions for your configured authentication mode:

- Local Database (default) : Uses the same credentials as the pfSense webConfigurator. To authenticate API calls, simply
add a `client-id` value containing your username and a `client-token` value containing your password to your payload. For
example `{"client-id": "admin", "client-token": "pfsense"}`

- JWT : Requires a bearer token to be included in the `Authorization` header of your request. To receive a bearer token,
you may make a POST request to /api/v1/access_token/ and include a `client-id` value containing your pfSense username
and a `client-token` value containing your pfSense password to your payload. For example
`{"client-id": "admin", "client-token": "pfsense"}`. Once you have your bearer token, you can authenticate your API
call by adding it to the request's authorization header. (e.g. `Authorization: Bearer xxxxxxxx.xxxxxxxxx.xxxxxxxx`)

- API Token : Uses standalone tokens generated via the UI. These are better suited to distribute to systems as they are
revocable and will only allow API authentication and not UI or SSH authentication (like the local database credentials).
To generate or revoke credentials, navigate to System > API within the UI and ensure the Authentication Mode is set to
API token. Then you should have the options to configure API Token generation, generate new tokens, and revoke existing
tokens. Once you have your API token, you may authenticate your API call by adding a `client-id` value containing your
API token client ID and a `client-token` value containing your API token client token to your payload.
(e.g. `{"client-id": "cccdj-311s", "client-token": "42jkjl-k234jlk1b38123kj3kjl-ffwzzuilaei"}`

# Response Codes
`200 (OK)` : API call succeeded<br>
`400 (Bad Request)` : An error was found within your requested parameters<br>
`401 (Unauthorized)` : API client has not completed authentication or authorization successfully<br>
`403 (Forbidden)` : The API endpoint has refused your call. Commonly due to your access settings found in `System > API`<br>
`404 (Not found)` : Either the API endpoint or requested data was not found<br>
`500 (Server error)` : The API endpoint encountered an unexpected error processing your API request<br>

# Error Codes
A full list of error codes can be found by navigating to /api/v1/system/api/errors/ after installation. This will return
 JSON data containing each error code and their corresponding error message. No authentication is required to view the
 error code library. This also makes API integration with third-party software easy as the API error codes and messages
 are always just an HTTP call away!

# Rate limit
There is no limit to API calls at this time. Alternatively, you enable the API in read-only mode to only allow endpoints
with read access within System > API.
