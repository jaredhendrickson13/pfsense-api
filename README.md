
# pfSense REST API Documentation

---

# Introduction
pfSense API is a fast, safe, REST API package for pfSense firewalls. This works by leveraging the same PHP functions and processes used 
by pfSense's webConfigurator into API endpoints to create, read, update and delete pfSense configurations. All API 
endpoints enforce input validation to prevent invalid configurations from being made. Configurations made via API are 
properly written to the master XML configuration and the correct backend configurations are made preventing the need for
 a reboot. All this results in the fastest, safest, and easiest way to automate pfSense!


# Requirements
- pfSense 2.4.4 or later is supported
- pfSense API requires a local user account in pfSense. The same permissions required to make configurations in the 
webConfigurator are required to make calls to the API endpoints
- While not an enforced requirement, it is **strongly** recommended that you configure pfSense to use HTTPS instead of HTTP. This ensures that login credentials and/or API tokens remain secure in-transit


# Installation
To install pfSense API, simply run the following command from the pfSense shell:<br>
```
pkg add https://github.com/jaredhendrickson13/pfsense-api/releases/latest/download/pfSense-2.4-pkg-API.txz && /etc/rc.restart_webgui
```

To uninstall pfSense API, run the following command:<br>
```
pfsense-api delete
```

To update pfSense API to the latest stable version, run the following command:
```
pfsense-api update
```

To revert to a previous version of pfSense API (e.g. v1.0.2), run the following command:
```
pfsense-api revert v1.0.2
```

### Notes: 
- pfSense API is supported on the pfSense 2.5 developer snapshots. To install the 2.5 package, simply change the `2.4` in the install URL to `2.5`.
- In order for pfSense to apply some required web server changes, it is required to restart the webConfigurator after installing the package
- If you do not have shell access to pfSense, you can still install via the webConfigurator by navigating to 
'Diagnostics > Command Prompt' and enter the commands there
- When updating pfSense, **_you must reinstall pfSense API afterwards_**. Unfortunately, pfSense removes all existing packages and only reinstalls packages found within pfSense's package repositories. Since pfSense API is not an official package in pfSense's repositories, it does not get reinstalled automatically.
- The `pfsense-api` command line tool was introduced in v1.1.0. Refer to the corresponding documentation for earlier releases.


# UI Settings & Documentation
After installation, you will be able to access the API user interface pages within the pfSense webConfigurator. These will be found under System > API. The settings tab will allow you change various API settings such as enabled API interfaces, authentication modes, and more. Additionally, the documentation tab will give you access to an embedded documentation tool that makes it easy to view the full API documentation in context to your pfSense instance.

### Notes: 
- Users must hold the `page-all` or `page-system-api` privileges to access the API page within the webConfigurator


# Authentication & Authorization
By default, pfSense API uses the same credentials as the webConfigurator. This behavior allows you to configure pfSense 
from the API out of the box, and user passwords may be changed from the API to immediately add additional security if 
needed. After installation, you can navigate to System > API in the pfSense webConfigurator to configure API
authentication. Please note that external authentication servers like LDAP or RADIUS are not supported with any 
API authentication method at this time. 

To authenticate your API call, follow the instructions for your configured authentication mode:

<details>
    <summary>Local Database (default)</summary>

Uses the same credentials as the pfSense webConfigurator. To authenticate API calls, simply add a `client-id` value containing your username and a `client-token` value containing your password to your payload. For example `{"client-id": "admin", "client-token": "pfsense"}`

</details>

<details>
    <summary>JWT</summary>

Requires a bearer token to be included in the `Authorization` header of your request. To receive a bearer token, you may make a POST request to /api/v1/access_token/ and include a `client-id` value containing your pfSense username and a `client-token` value containing your pfSense password to your payload. For example `{"client-id": "admin", "client-token": "pfsense"}`. Once you have your bearer token, you can authenticate your API call by adding it to the request's authorization header. (e.g. `Authorization: Bearer xxxxxxxx.xxxxxxxxx.xxxxxxxx`)
</details>

<details>
    <summary>API Token</summary>

Uses standalone tokens generated via the UI. These are better suited to distribute to systems as they are revocable and will only allow API authentication; not UI or SSH authentication (like the local database credentials). To generate or revoke credentials, navigate to System > API within the UI and ensure the Authentication Mode is set to API token. Then you should have the options to configure API Token generation, generate new tokens, and revoke existing tokens. Once you have your API token, you may authenticate your API call by specifying your client-id and client-token within an `Authorization` header, these values must be seperated by a space. (e.g. `Authorization: client-id-here client-token-here`)

_Note: In previous versions of pfSense API, the client-id and client-token were provided via the request payload. This functionality is still supported but is not recommended. It will be removed in a future release._

</details>

### Authorization
pfSense API uses the same privileges as the pfSense webConfigurator. The required privileges for each endpoint are stated within the API documentation.


# Content Types
pfSense API can handle a few different content types. Please note, if a `Content-Type` header is not specified in your request, pfSense API will attempt to determine the
content type which may have undesired results. It is recommended you specify your preferred `Content-Type` on each request. While several content types may be enabled,
`application/json` is the recommended content type. Supported content types are:

<details>
    <summary>application/json</summary>

Parses the request body as a JSON formatted string. This is the recommended content type.

Example:

```
curl -s -H "Content-Type: application/json" -d '{"client-id": "admin", "client-token": "pfsense"}' -X GET https://pfsense.example.com/api/v1/system/arp
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "ip": "192.168.1.1",
      "mac": "00:0c:29:f6:be:d9",
      "interface": "em1",
      "status": "permanent",
      "linktype": "ethernet"
    },
    {
      "ip": "172.16.209.139",
      "mac": "00:0c:29:f6:be:cf",
      "interface": "em0",
      "status": "permanent",
      "linktype": "ethernet"
    }
  ]
}
```

</details>

<details>
    <summary>application/x-www-form-urlencoded</summary>

Parses the request body as URL encoded parameters.

Example:

```
curl -s -H "Content-Type: application/x-www-form-urlencoded" -X GET "https://pfsense.example.com/api/v1/system/arp?client-id=admin&client-token=pfsense"
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "ip": "192.168.1.1",
      "mac": "00:0c:29:f6:be:d9",
      "interface": "em1",
      "status": "permanent",
      "linktype": "ethernet"
    },
    {
      "ip": "172.16.209.139",
      "mac": "00:0c:29:f6:be:cf",
      "interface": "em0",
      "status": "permanent",
      "linktype": "ethernet"
    }
  ]
}
```

</details>

# Response Codes
`200 (OK)` : API call succeeded<br>
`400 (Bad Request)` : An error was found within your requested parameters<br>
`401 (Unauthorized)` : API client has not completed authentication or authorization successfully<br>
`403 (Forbidden)` : The API endpoint has refused your call. Commonly due to your access settings found in System > API<br>
`404 (Not found)` : Either the API endpoint or requested data was not found<br>
`500 (Server error)` : The API endpoint encountered an unexpected error processing your API request<br>


# Error Codes
A full list of error codes can be found by navigating to /api/v1/system/api/error after installation. This will return
 JSON data containing each error code and their corresponding error message. No authentication is required to view the 
 error code library. This also makes API integration with third-party software easy as the API error codes and messages 
 are always just an HTTP call away!


# Queries
pfSense API contains an advanced query engine to make it easy to query specific data from API calls. For endpoints supporting `GET` requests, you may query the return data to only return data you are looking for. To query data, you may add the data you are looking for to your payload. You may specify as many query parameters as you need. In order to match the query, each parameter must match exactly, or utilize a query filter to set criteria. If no matches were found, the endpoint will return an empty array in the data field. 
<details>
    <summary>Targetting Objects</summary>
    
You may find yourself only needing to read objects with specific values set. For example, say an API endpoint normally returns this response without a query:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you want the endpoint to only return the objects that have their `type` value set to `type1` you could add `{"type": "type1"}` to your payload. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}
    ]
}
```

Additionally, if you need to target values that are nested within an array, you can add `{"extra__tag": 100}` to recursively target the `tag` value within the `extra` array. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}}
    ]
}
```

</details>

<details>
    <summary>Query Filters</summary>
    
Query filters allow you to apply logic to the objects you target. This makes it easy to target data that meets specific criteria:

### Starts With
    
The `startswith` filter allows you to target objects whose values start with a specific substring. This will work on both string and integer data types. Below is an example response without any queries:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you wanted to target objects whose names started with `Other`, you could use the payload `{"name__startswith": "Other"}`. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}}
    ]
}
```

### Ends With
    
The `endswith` filter allows you to target objects whose values end with a specific substring. This will work on both string and integer data types. Below is an example response without any queries:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you wanted to target objects whose names ended with `er Test`, you could use the payload `{"name__endswith" "er Test"}`. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}    
    ]
}
```

### Contains
    
The `contains` filter allows you to target objects whose values contain a specific substring. This will work on both string and integer data types. Below is an example response without any queries:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you wanted to target objects whose names contain `ther`, you could use the payload `{"name__contains": "ther"}`. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}    
    ]
}
```

### Less Than
    
The `lt` filter allows you to target objects whose values are less than a specific number. This will work on both numeric strings and integer data types. Below is an example response without any queries:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you wanted to target objects whose tag is less than `100`, you could use the payload `{"extra__tag__lt": 100}`. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}  
    ]
}
```

### Less Than or Equal To
    
The `lte` filter allows you to target objects whose values are less than or equal to a specific number. This will work on both numeric strings and integer data types. Below is an example response without any queries:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you wanted to target objects whose tag is less than or equal to `100`, you could use the payload `{"extra__tag__lte": 100}`. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}},
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}}
    ]
}
```

### Greater Than
    
The `gt` filter allows you to target objects whose values are greater than a specific number. This will work on both numeric strings and integer data types. Below is an example response without any queries:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you wanted to target objects whose tag is greater than `100`, you could use the payload `{"extra__tag__gt": 100}`. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}  
    ]
}
```

### Greater Than or Equal To
    
The `lte` filter allows you to target objects whose values are greater than or equal to a specific number. This will work on both numeric strings and integer data types. Below is an example response without any queries:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 0, "name": "Test", "type": "type1", "extra": {"tag": 0}}, 
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}

    ]
}
```

If you wanted to target objects whose tag is greater than or equal to `100`, you could use the payload `{"extra__tag__gte": 100}`. This returns:

```json
{
    "status":"ok",
    "code":200,
    "return":0,
    "message":"Success",
    "data": [
        {"id": 1, "name": "Other Test", "type": "type2", "extra": {"tag": 100}},
        {"id": 2, "name": "Another Test", "type": "type1", "extra": {"tag": 200}}
    ]
}
```

</details>

### Requirements for queries:
- API call must be successful and return `0` in the `return` field.
- Endpoints must return an array of objects in the data field (e.g. `[{"id": 0, "name": "Test"}, {"id": 1, "name": "Other Test"}]`).
- At least two objects must be present within the data field to support queries.

### Notes:
- For those using the Local database or API token authentication types, `client-id` and `client-token` are excluded from queries by default
- Both recursive queries and query filters are seperated by double underscores (`__`)


# Rate limit
There is no limit to API calls at this time but is important to note that pfSense's XML configuration was not designed for quick simultaneous writes like a traditional database. It may be necessary to delay API calls in sequence to prevent unexpected behavior. Alternatively, you may limit the API to read-only mode to only allow endpoints with read (GET) access within the webConfigurator's System > API page.


# Endpoints

## Indices

* [ACCESS_TOKEN](#access_token)

  * [Request Access Token](#1-request-access-token)

* [FIREWALL](#firewall)

  * [ALIAS](#1-alias)
  * [APPLY](#2-apply)
  * [NAT](#3-nat)
  * [RULE](#4-rule)
  * [STATES](#5-states)
  * [VIRTUAL_IP](#6-virtual_ip)

* [INTERFACE](#interface)

  * [APPLY](#1-apply)
  * [Create Interfaces](#2-create-interfaces)
  * [Delete Interfaces](#3-delete-interfaces)
  * [Read Interfaces](#4-read-interfaces)
  * [Update Interfaces](#5-update-interfaces)
  * [VLAN](#6-vlan)

* [ROUTING](#routing)

  * [APPLY](#1-apply-1)
  * [GATEWAY](#2-gateway)
  * [STATIC_ROUTE](#3-static_route)

* [SERVICES](#services)

  * [DDNS](#1-ddns)
  * [DHCPD](#2-dhcpd)
  * [DPINGER](#3-dpinger)
  * [NTPD](#4-ntpd)
  * [Read Services](#5-read-services)
  * [Restart All Services](#6-restart-all-services)
  * [SSHD](#7-sshd)
  * [SYSLOGD](#8-syslogd)
  * [Start All Services](#9-start-all-services)
  * [Stop All Services](#10-stop-all-services)
  * [UNBOUND](#11-unbound)

* [STATUS](#status)

  * [CARP](#1-carp)
  * [GATEWAY](#2-gateway-1)
  * [INTERFACE](#3-interface)
  * [LOG](#4-log)
  * [SYSTEM](#5-system)

* [SYSTEM](#system)

  * [API](#1-api)
  * [ARP](#2-arp)
  * [CERTIFICATE](#3-certificate)
  * [CONFIG](#4-config)
  * [DNS](#5-dns)
  * [HALT](#6-halt)
  * [HOSTNAME](#7-hostname)
  * [REBOOT](#8-reboot)
  * [TABLE](#9-table)
  * [TUNABLE](#10-tunable)
  * [VERSION](#11-version)

* [USER](#user)

  * [AUTH_SERVER](#1-auth_server)
  * [Create Users](#2-create-users)
  * [Delete Users](#3-delete-users)
  * [GROUP](#4-group)
  * [PRIVILEGE](#5-privilege)
  * [Read Users](#6-read-users)
  * [Update Users](#7-update-users)


--------


## ACCESS_TOKEN
API endpoints used to receive access tokens used to authenticate API requests. Only applicable when the API authentication mode is set to JWT.



### 1. Request Access Token


Receive a temporary access token using your pfSense local database credentials.


***Endpoint:***

```bash
Method: POST
Type: RAW
URL: https://{{$hostname}}/api/v1/access_token
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| client-id | string | Specify the client's pfSense username |
| client-token | string | Specify the client's pfSense password |



***Body:***

```js        
{
	"client-id": "admin", 
	"client-token": "pfsense"
}
```



## FIREWALL
API endpoints that create, read, update and delete firewall services.



### 1. ALIAS



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 2. APPLY



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 3. NAT



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 4. RULE



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 5. STATES



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 6. VIRTUAL_IP



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



## INTERFACE
API endpoints that create, read, update and delete network interfaces.



### 1. APPLY



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 2. Create Interfaces


Add a new interface.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-interfaces-assignnetworkports`]


***Endpoint:***

```bash
Method: POST
Type: RAW
URL: https://{{$hostname}}/api/v1/interface
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| if | string | Specify the physical interface to configure |
| descr | string | Set a descriptive name for the new interface |
| enable | boolean | Enable the interface upon creation. Do not specify this value to leave disabled. |
| spoofmac | string | Set a custom MAC address to the interface (optional) |
| mtu | string or integer | Set a custom MTU for this interface (optional) |
| mss | string or integer | Set a custom MSS for the this interface (optional) |
| media | string | Set a custom speed/duplex setting for this interface (optional) |
| type | string | Set the interface IPv4  configuration type (optional) |
| type6 | string | Set the interface IPv6  configuration type (optional) |
| ipaddr | string | Set the interface's static IPv4 address (required if `type` is set to `staticv4`) |
| subnet | string or integer | Set the interface's static IPv4 address's subnet bitmask (required if `type` is set to `staticv4`) |
| gateway | string | Set the interface network's upstream gateway. This is only necessary on WAN/UPLINK interfaces (optional) |
| ipaddrv6 | string | Set the interface's static IPv6 address (required if `type6` is set to `staticv6`) |
| subnetv6 | string or integer | Set the interface's static IPv6 address's subnet bitmask (required if `type6` is set to `staticv6`) |
| gatewayv6 | string | Set the interface network's upstream IPv6 gateway. This is only necessary on WAN/UPLINK interfaces (optional) |
| ipv6usev4iface | boolean | Allow IPv6 to use IPv4 uplink connection (optional) |
| dhcphostname | string | Set the IPv4 DHCP hostname. Only available when `type` is set to `dhcp` (optional) |
| dhcprejectfrom | string or array | Set the IPv4 DHCP rejected servers. You may pass values in as array or as comma seperated string. Only available when `type` is set to `dhcp` (optional) |
| alias-address | string | Set the IPv4 DHCP address alias. The value in this field is used as a fixed alias IPv4 address by the DHCP client (optional) |
| alias-subnet | string or integer | Set the IPv4 DHCP address aliases subnet (optional) |
| adv_dhcp_pt_timeout | string or integer | Set the IPv4 DHCP protocol timeout interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_retry | string or integer | Set the IPv4 DHCP protocol retry interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_select_timeout | string or integer | Set the IPv4 DHCP protocol select timeout interval. Must be numeric value greater than 0 (optional) |
| adv_dhcp_pt_reboot | string or integer | Set the IPv4 DHCP protocol reboot interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_backoff_cutoff | string or integer | Set the IPv4 DHCP protocol backoff cutoff interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_initial_interval | string or integer | Set the IPv4 DHCP protocol initial interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_config_advanced | boolean | Enable the IPv4 DHCP advanced configuration options. This enables the DHCP options listed below (optional) |
| adv_dhcp_send_options | string | Set a custom IPv4 `send` option (optional) |
| adv_dhcp_request_options | string | Set a custom IPv4 `request` option (optional) |
| adv_dhcp_request_options | string | Set a custom IPv4 `required` option (optional) |
| adv_dhcp_option_modifiers | string | Set a custom IPv4 optional modifier (optional) |
| adv_dhcp_config_file_override | boolean | Enable local DHCP configuration file override (optional) |
| adv_dhcp_config_file_override_file | string | Set the custom DHCP configuration file's absolute path. This file must exist beforehand (optional) |
| dhcpvlanenable | boolean | Enable DHCP VLAN prioritization (optional) |
| dhcpcvpt | string or integer | Set the DHCP VLAN priority. Must be a numeric value between 1 and 7 (optional) |
| gateway-6rd | string | Set the 6RD interface IPv4 gateway address. Only required when `type6` is set to `6rd` |
| prefix-6rd | string | Set the 6RD IPv6 prefix assigned by the ISP. Only required when `type6` is set to `6rd` |
| prefix-6rd-v4plen | integer or string | Set the 6RD IPv4 prefix length. This is typically assigned by the ISP. Only available when `type6` is set to `6rd` |
| track6-interface | string | Set the Track6 dynamic IPv6 interface. This must be a dynamically configured IPv6 interface. You may specify either the interface's descriptive name, the pfSense ID (wan, lan, optx), or the physical interface id (e.g. igb0). Only required with `type6` is set to `track6` |
| track6-prefix-id-hex | string or integer | Set the IPv6 prefix ID. The value in this field is the (Delegated) IPv6 prefix ID. This determines the configurable network ID based on the dynamic IPv6 connection. The default value is 0. Only available when `type6` is set to `track6` |
| apply | boolean | Specify whether or not you would like this interface to be applied immediately, or simply written to the configuration to be applied later. Typically, if you are creating multiple interfaces at once it Is best to set this to false and apply the changes afterwards using the `/api/v1/interface/apply` endpoint. Otherwise, If you are only creating a single interface, you may set this true to apply it immediately. Defaults to false. (optional) |



***Body:***

```js        
{
	"if": "em1.3",
	"descr": "asdf",
	"enable": "",
	"type": "staticv4",
	"ipaddr": "10.250.0.1",
	"subnet": "24",
	"blockbogons": true
}
```



### 3. Delete Interfaces


Delete an existing interface. __Note: interface deletions will be applied immediately, there is no need to apply interface changes afterwards__<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-interfaces-assignnetworkports`]


***Endpoint:***

```bash
Method: DELETE
Type: RAW
URL: https://{{$hostname}}/api/v1/interface
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| if | string | Specify the interface to delete. You may specify either the interface's descriptive name, the pfSense ID (wan, lan, optx), or the physical interface id (e.g. igb0) |



***Body:***

```js        
{
	"if": "em1.3"
}
```



### 4. Read Interfaces


Read interface assignements and configuration.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-interfaces-assignnetworkports`]


***Endpoint:***

```bash
Method: GET
Type: RAW
URL: https://{{$hostname}}/api/v1/interface
```



***Body:***

```js        
{
}
```



### 5. Update Interfaces


Update an existing interface.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-interfaces-assignnetworkports`]


***Endpoint:***

```bash
Method: PUT
Type: RAW
URL: https://{{$hostname}}/api/v1/interface
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| id | string | Specify the Interface to update. You may specify either the interface's descriptive name, the pfSense ID (wan, lan, optx), or the physical interface id (e.g. igb0) |
| if | string | Update the physical interface configured (optional) |
| descr | string | Update the descriptive name of the interface (optional) |
| enable | boolean | Enable or disable the Interface (optional) |
| spoofmac | string | Update the MAC address of the interface (optional) |
| mtu | string or integer | Update the MTU for this interface (optional) |
| mss | string or integer | Update the MSS for the this interface (optional) |
| media | string | Update the speed/duplex setting for this interface (optional) |
| type | string | Update the interface IPv4  configuration type (optional) |
| type6 | string | Update the interface IPv6  configuration type (optional) |
| ipaddr | string | Update the interface's static IPv4 address (required if `type` is set to `staticv4`) |
| subnet | string or integer | Update the interface's static IPv4 address's subnet bitmask (required if `type` is set to `staticv4`) |
| gateway | string | Update the interface network's upstream gateway. This is only necessary on WAN/UPLINK interfaces (optional) |
| ipaddrv6 | string | Update the interface's static IPv6 address (required if `type6` is set to `staticv6`) |
| subnetv6 | string or integer | Update the interface's static IPv6 address's subnet bitmask (required if `type6` is set to `staticv6`) |
| gatewayv6 | string | Update the interface network's upstream IPv6 gateway. This is only necessary on WAN/UPLINK interfaces (optional) |
| ipv6usev4iface | boolean | Enable or disable IPv6 over IPv4 uplink connection (optional) |
| dhcphostname | string | Update the IPv4 DHCP hostname. Only available when `type` is set to `dhcp` (optional) |
| dhcprejectfrom | string or array | Update the IPv4 DHCP rejected servers. You may pass values in as array or as comma seperated string. Only available when `type` is set to `dhcp` (optional) |
| alias-address | string | Update the IPv4 DHCP address alias. The value in this field is used as a fixed alias IPv4 address by the DHCP client (optional) |
| alias-subnet | string or integer | Update the IPv4 DHCP address aliases subnet (optional) |
| adv_dhcp_pt_timeout | string or integer | Update the IPv4 DHCP protocol timeout interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_retry | string or integer | Update the IPv4 DHCP protocol retry interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_select_timeout | string or integer | Update the IPv4 DHCP protocol select timeout interval. Must be numeric value greater than 0 (optional) |
| adv_dhcp_pt_reboot | string or integer | Update the IPv4 DHCP protocol reboot interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_backoff_cutoff | string or integer | Update the IPv4 DHCP protocol backoff cutoff interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_pt_initial_interval | string or integer | Update the IPv4 DHCP protocol initial interval. Must be numeric value greater than 1 (optional) |
| adv_dhcp_config_advanced | boolean | Enable or disable the IPv4 DHCP advanced configuration options. This enables the DHCP options listed below (optional) |
| adv_dhcp_send_options | string | Update the IPv4 `send` option (optional) |
| adv_dhcp_request_options | string | Update the IPv4 `request` option (optional) |
| adv_dhcp_request_options | string | Update the IPv4 `required` option (optional) |
| adv_dhcp_option_modifiers | string | Update the IPv4 optionamodifier (optional) |
| adv_dhcp_config_file_override | boolean | Enable or disable local DHCP configuration file override (optional) |
| adv_dhcp_config_file_override_file | string | Update the custom DHCP configuration file's absolute path. This file must exist beforehand (optional) |
| dhcpvlanenable | boolean | Enable or disable DHCP VLAN prioritization (optional) |
| dhcpcvpt | string or integer | Update the DHCP VLAN priority. Must be a numeric value between 1 and 7 (optional) |
| gateway-6rd | string | Update the 6RD interface IPv4 gateway address. Only required when `type6` is set to `6rd` |
| prefix-6rd | string | Update the 6RD IPv6 prefix assigned by the ISP. Only required when `type6` is set to `6rd` |
| prefix-6rd-v4plen | integer or string | Update the 6RD IPv4 prefix length. This is typically assigned by the ISP. Only available when `type6` is set to `6rd` |
| track6-interface | string | Update the Track6 dynamic IPv6 interface. This must be a dynamically configured IPv6 interface. You may specify either the interface's descriptive name, the pfSense ID (wan, lan, optx), or the physical interface id (e.g. igb0). Only required with `type6` is set to `track6` |
| track6-prefix-id-hex | string or integer | Update the IPv6 prefix ID. The value in this field is the (Delegated) IPv6 prefix ID. This determines the configurable network ID based on the dynamic IPv6 connection. The default value is 0. Only available when `type6` is set to `track6` |
| apply | boolean | Specify whether or not you would like this interface update to be applied immediately, or simply written to the configuration to be applied later. Typically, if you are updating multiple interfaces at once it Is best to set this to false and apply the changes afterwards using the `/api/v1/interface/apply` endpoint. Otherwise, If you are only updating a single interface, you may set this true to apply it immediately. Defaults to false. (optional) |



***Body:***

```js        
{
	"if": "em1.3",
	"descr": "asdf",
	"enable": "",
	"type": "staticv4",
	"ipaddr": "10.250.0.1",
	"subnet": "24",
	"blockbogons": true
}
```



### 6. VLAN



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



## ROUTING
API endpoints that create, read, update and delete network routing configuration.



### 1. APPLY



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 2. GATEWAY



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 3. STATIC_ROUTE



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



## SERVICES
API endpoints that create, read, update, and delete system services.



### 1. DDNS



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 2. DHCPD



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 3. DPINGER



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 4. NTPD



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 5. Read Services


Read available services.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-status-services`]


***Endpoint:***

```bash
Method: GET
Type: RAW
URL: https://{{$hostname}}/api/v1/services
```



***Body:***

```js        
{
    
}
```



### 6. Restart All Services


Restart all available services.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-status-services`]


***Endpoint:***

```bash
Method: POST
Type: RAW
URL: https://{{$hostname}}/api/v1/services/restart
```



***Body:***

```js        
{
    
}
```



### 7. SSHD



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 8. SYSLOGD



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 9. Start All Services


Start all available services.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-status-services`]


***Endpoint:***

```bash
Method: POST
Type: RAW
URL: https://{{$hostname}}/api/v1/services/start
```



***Body:***

```js        
{
    
}
```



### 10. Stop All Services


Stop all available services.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-status-services`]


***Endpoint:***

```bash
Method: POST
Type: RAW
URL: https://{{$hostname}}/api/v1/services/stop
```



***Body:***

```js        
{
    
}
```



### 11. UNBOUND



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



## STATUS
API endpoints that read and update system statuses.



### 1. CARP



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 2. GATEWAY



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 3. INTERFACE



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 4. LOG



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 5. SYSTEM



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



## SYSTEM
API calls that create, read, update and delete system configuration.



### 1. API



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 2. ARP



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 3. CERTIFICATE



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 4. CONFIG



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 5. DNS



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 6. HALT



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 7. HOSTNAME



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 8. REBOOT



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 9. TABLE



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 10. TUNABLE



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 11. VERSION



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



## USER
API endpoints that create, read, update and delete pfSense users and authentication management.



### 1. AUTH_SERVER



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 2. Create Users


Add a new pfSense user to the local database.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-system-usermanager`]


***Endpoint:***

```bash
Method: POST
Type: RAW
URL: https://{{$hostname}}/api/v1/user
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| username | string | Username to assign new user |
| password | string | Password to assign new user |
| descr | string | Descriptive name to assign new user (optional) |
| authorizedkeys | string | Base64 encoded authorized SSH keys to assign new user (optional) |
| ipsecpsk | string | IPsec pre-shared key to assign new user (optional) |
| disabled | boolean | Disable user account upon creation. Do not include to leave enabled. (optional) |
| expires | string | Expiration date for user account in  MM/DD/YYYY format (optional) |



***Body:***

```js        
{
	"disabled": true,
	"username": "new_user",
	"password": "changeme",
	"descr": "NEW USER",
	"authorizedkeys": "test auth key",
	"ipsecpsk": "test psk",
	"expires": "11/22/2020"
}
```



### 3. Delete Users


Delete an existing pfSense user from the local database.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-system-usermanager`]


***Endpoint:***

```bash
Method: DELETE
Type: RAW
URL: https://{{$hostname}}/api/v1/user
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| username | string | Username to to delete |



***Body:***

```js        
{
	"username": "new_user"
}
```



### 4. GROUP



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 5. PRIVILEGE



***Endpoint:***

```bash
Method: 
Type: 
URL: 
```



### 6. Read Users


Reads users from the local user database.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-system-usermanager`]


***Endpoint:***

```bash
Method: GET
Type: RAW
URL: https://{{$hostname}}/api/v1/user
```



***Body:***

```js        
{
    
}
```



### 7. Update Users


Update an existing user.<br><br>

_Requires at least one of the following privileges:_ [`page-all`, `page-system-usermanager`]


***Endpoint:***

```bash
Method: PUT
Type: RAW
URL: https://{{$hostname}}/api/v1/user
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| username | string | Username to modify |
| password | string | Change user password (optional) |
| descr | string | Descriptive name to assign the user (optional) |
| authorizedkeys | string | Base64 encoded authorized keys to add to user (optional) |
| ipsecpsk | string | IPsec pre-shared key to assign to user (optional) |
| disabled | boolean | Disable user account  (optional) |
| expires | string | Expiration date for user account in  MM/DD/YYYY format (optional) |



***Body:***

```js        
{
	"disabled": true,
	"username": "new_user",
	"password": "changeme",
	"descr": "NEW USER",
	"authorizedkeys": "test auth key",
	"ipsecpsk": "test psk",
	"expires": "11/22/2020"
}
```



---
[Back to top](#pfsense-rest-api-documentation)
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2021-01-31 12:08:59 by [docgen](https://github.com/thedevsaddam/docgen)
