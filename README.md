# pfSense-API

-----------

[![OpenAPI](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/openapi.yml/badge.svg)](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/openapi.yml)
[![PHPlint](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/phplint.yml/badge.svg)](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/phplint.yml)
[![Pylint](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/pylint.yml/badge.svg)](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/pylint.yml)

# Introduction

pfSense API is a fast, safe, REST API package for pfSense firewalls. This works by leveraging the same PHP functions and
processes used by pfSense's webConfigurator into API endpoints to create, read, update and delete pfSense
configurations. All API endpoints enforce input validation to prevent invalid configurations from being made.
Configurations made via API are properly written to the master XML configuration and the correct backend configurations
are made preventing the need for a reboot. All this results in the fastest, safest, and easiest way to automate pfSense!

# Requirements

<details>
    <summary>Supported pfSense Versions</summary>

  - pfSense Plus 22.05 (AMD64)
  - pfSense Plus 22.01 (AMD64)
  - pfSense CE 2.6.0 (AMD64)
  - pfSense CE 2.5.2 (AMD64)
  - pfSense CE 2.5.1 (AMD64)
  - pfSense CE 2.5.0 (AMD64)
  ---
  
</details>

- pfSense API requires a local user account in pfSense. The same permissions required to make configurations in the
  webConfigurator are required to make calls to the API endpoints.
- While not an enforced requirement, it is **strongly** recommended that you configure pfSense to use HTTPS instead of
  HTTP. This ensures that login credentials and/or API tokens remain secure in-transit.

# Installation

To install pfSense API, simply run the following command from the pfSense shell:<br>

```
pkg add https://github.com/jaredhendrickson13/pfsense-api/releases/latest/download/pfSense-2.6-pkg-API.txz && /etc/rc.restart_webgui
```

<br>

To uninstall pfSense API, run the following command:<br>

```
pfsense-api delete
```

<br>

To update pfSense API to the latest stable version, run the following command:<br>

```
pfsense-api update
```

<br>

To revert to a previous version of pfSense API (e.g. v1.1.7), run the following command:<br>

```
pfsense-api revert v1.1.7
```

<br>

### Notes:

- While not always necessary, it's recommended to change the installation command to reference the package built for your
  version of pfSense. You can check the [releases page](https://github.com/jaredhendrickson13/pfsense-api/releases)
  for available versions.
- In order for pfSense to apply some required web server changes, it is required to restart the webConfigurator after
  installing the package.
- If you do not have shell access to pfSense, you can still install via the webConfigurator by navigating to '
  Diagnostics > Command Prompt' and enter the commands there.
- When updating pfSense, **_you must reinstall pfSense API afterwards_**. Unfortunately, pfSense removes all existing
  packages and only re-installs packages found within pfSense's package repositories. Since pfSense API is not an
  official package in pfSense's repositories, it does not get reinstalled automatically.
- The `pfsense-api` command line tool was introduced in v1.1.0. Refer to the corresponding documentation for earlier
  releases.

# webConfigurator Settings & Documentation

After installation, you will be able to access the API user interface pages within the pfSense webConfigurator. These
will be found under System > API. The settings tab will allow you change various API settings such as enabled API
interfaces, authentication modes, and more. Additionally, the documentation tab will give you access to an embedded
documentation tool that makes it easy to view the full API documentation in context to your pfSense instance.

### Notes:

- Users must hold the `page-all` or `page-system-api` privileges to access the API page within the webConfigurator.

# Authentication & Authorization

By default, pfSense API uses the same credentials as the webConfigurator. This behavior allows you to configure pfSense
from the API out of the box, and user passwords may be changed from the API to immediately add additional security if
needed. After installation, you can navigate to System > API in the pfSense webConfigurator to configure API
authentication. Please note that external authentication servers like LDAP or RADIUS are not supported with any API
authentication method at this time. To authenticate your API call, follow the instructions for your configured
authentication mode:

<details>
    <summary>Local Database (default)</summary>

Uses the same credentials as the pfSense webConfigurator. To authenticate API calls, pass in your username and password
using basic authentication. For example:<br><br>
`curl -u admin:pfsense https://pfsense.example.com/api/v1/firewall/rule`
<br><br>
_Note: in previous releases, local database authentication used the `client-id` and `client-token` fields in your
request body to authenticate. This functionality still exists but is not recommended. It will be removed in a future
release._
</details>

<details>
    <summary>JWT</summary>

Requires a bearer token to be included in the `Authorization` header of your request. These are time-based tokens that
will expire after the configured amount of time. To configure the JWT expiration, navigate to System > API within the
webConfigurator and ensure the the Authentication Mode is set to JWT. Then you should have the option to configure the
JWT Expiration value. Alternatively, you can use the /api/v1/system/api endpoint to update the `jwt_exp` value. To
receive a JWT, you must make a POST request to the /api/v1/access_token endpoint. This endpoint will always require the
use of the Local Database authentication type to receive the JWT. <br><br>For example:<br>

```
curl -u admin:pfsense -X POST https://pfsense.example.com/api/v1/access_token
```

<br><br>
Once you have your JWT, you can authenticate your API calls by adding it to the request's authorization
header. For example:<br>

```
curl -H "Authorization: Bearer xxxxx.xxxxxx.xxxxxx" -X GET https://pfsense.example.com/api/v1/system/arp
```

</details>

<details>
    <summary>API Token</summary>

Uses standalone tokens generated via the webConfigurator. These are better suited to distribute to systems as they are
revocable and will only allow API authentication; not webConfigurator or SSH authentication (like the local database
credentials). To generate or revoke credentials, navigate to System > API within the webConfigurator and ensure the
Authentication Mode is set to API token. Then you should have the options to configure API Token generation, generate
new tokens, and revoke existing tokens. After generating a new API token, the actual token will display at the top of
the page on the success banner. This token will only be displayed once so ensure it is stored somewhere safe.<br><br>

Once you have your API token, you may authenticate your API call by specifying your client-id and client-token within
an `Authorization` header, these values must be separated by a space. For example:<br>

```
curl -H "Authorization: CLIENT_ID_HERE CLIENT_TOKEN_HERE" -X GET https://pfsense.example.com/api/v1/system/arp
```

<br><br>_Note: In previous versions of pfSense API, the client-id and client-token were provided via the request payload. This
functionality is still supported but is not recommended. It will be removed in a future release._
</details>

### Authorization

pfSense API uses the same privileges as the pfSense webConfigurator. The required privileges for each endpoint are
stated within the API documentation.

# Content Types

pfSense API can handle a few different content types. Please note, if a `Content-Type` header is not specified in your
request, pfSense API will attempt to determine the content type which may have undesired results. It is recommended you
specify your preferred `Content-Type` on each request.<br><br>
While several content types may be enabled, `application/json` is the recommended content type. Supported content types
are:

<details>
    <summary>application/json</summary>

Parses the request body as a JSON formatted string. This is the recommended content type.<br><br>
Example:<br><br>

```
curl -u admin:pfsense -H "Content-Type: application/json" -d '{"service": "sshd"}' -X POST https://pfsense.example.com/api/v1/services/restart
```

</details>

<details>
    <summary>application/x-www-form-urlencoded</summary>

Parses the request body as URL encoded parameters.<br><br>
Example:<br><br>

``` 
curl -u admin:pfsense -H "Content-Type: application/x-www-form-urlencoded" -X DELETE "https://pfsense.example.com/api/v1/firewall/alias?id=EXAMPLE_ALIAS"
```

<br><br>_Note: this content type only has the ability to pass values of string, integer, or boolean data types. Complex data
types like arrays and objects cannot be parsed by the API when using this content type. It is recommended to only
use this content type when making GET or DELETE requests._

</details>

# Queries

pfSense API contains an advanced query engine to make it easy to query specific data from API calls. For endpoints
supporting `GET` requests, you may query the return data to only return data you are looking for. To query data, you may
add the data you are looking for to your payload. You may specify as many query parameters as you need. In order to
match the query, each parameter must match exactly, or utilize a query filter to set criteria. If no matches were found,
the endpoint will return an empty array in the data field.
<details>
    <summary>Targeting Objects</summary>

You may find yourself only needing to read objects with specific values set. For example, say an API endpoint normally
returns this response without a query:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

If you want the endpoint to only return the objects that have their `type` value set to `type1` you could
add `{"type": "type1"}` to your payload. This returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

Additionally, if you need to target values that are nested within an array, you can add `{"extra__tag": 100}` to
recursively target the `tag` value within the `extra` array. Note the double underscore separating the parent and child
keys. This returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    }
  ]
}

```

</details>

<details>
    <summary>Query Filters</summary>

Query filters allow you to apply logic to the objects you target. This makes it easy to target data that meets specific
criteria:<br><br>

### Starts With

The `startswith` filter allows you to target objects whose values start with a specific substring. This will work on
both string and integer data types. Below is an example response without any queries:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

If you wanted to target objects whose names started with `Other`, you could use the
payload `{"name__startswith": "Other"}`. This returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    }
  ]
}

```

### Ends With

The `endswith` filter allows you to target objects whose values end with a specific substring. This will work on both
string and integer data types. Below is an example response without any queries:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

If you wanted to target objects whose names ended with `er Test`, you could use the
payload `{"name__endswith" "er Test"}`. This returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

### Contains

The `contains` filter allows you to target objects whose values contain a specific substring. This will work on both
string and integer data types. Below is an example response without any queries:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

If you wanted to target objects whose names contain `ther`, you could use the payload `{"name__contains": "ther"}`. This
returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

### Less Than

The `lt` filter allows you to target objects whose values are less than a specific number. This will work on both
numeric strings and integer data types. Below is an example response without any queries:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
} 
```

If you wanted to target objects whose tag is less than `100`, you could use the payload `{"extra__tag__lt": 100}`. This
returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    }
  ]
}

```

### Less Than or Equal To

The `lte` filter allows you to target objects whose values are less than or equal to a specific number. This will work
on both numeric strings and integer data types. Below is an example response without any queries:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

If you wanted to target objects whose tag is less than or equal to `100`, you could use the
payload `{"extra__tag__lte": 100}`. This returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    }
  ]
}

```

### Greater Than 

The `gt` filter allows you to target objects whose values are greater than a specific number. This will work on both numeric strings and integer data types. Below is an example response without any queries:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

If you wanted to target objects whose tag is greater than `100`, you could use the payload `{"extra__tag__gt": 100}`.
This returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

### Greater Than or Equal To

The `lte` filter allows you to target objects whose values are greater than or equal to a specific number. This will
work on both numeric strings and integer data types. Below is an example response without any queries:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 0,
      "name": "Test",
      "type": "type1",
      "extra": {
        "tag": 0
      }
    },
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

If you wanted to target objects whose tag is greater than or equal to `100`, you could use the
payload `{"extra__tag__gte": 100}`. This returns:<br><br>

```json
{
  "status": "ok",
  "code": 200,
  "return": 0,
  "message": "Success",
  "data": [
    {
      "id": 1,
      "name": "Other Test",
      "type": "type2",
      "extra": {
        "tag": 100
      }
    },
    {
      "id": 2,
      "name": "Another Test",
      "type": "type1",
      "extra": {
        "tag": 200
      }
    }
  ]
}

```

</details>

<details>
  <summary>Obtaining Object IDs</summary>

You may notice some API endpoints require an object `id` to update or delete objects. These IDs are not stored values,
rather pfSense uses the object's array index value to locate and identify objects. Unless specified otherwise, API
endpoints will use the same array index value (as returned in the `data` response field) to locate objects when updating
or deleting. Some important items to note about these IDs:<br><br>

- pfSense starts arrays with an index of 0. If you use a loop counter to determine the ID of a specific object, you must
  start that counter at 0.
- These IDs are dynamic. For example, if you have 3 static route objects stored (IDs `0`, `1`, and `2`) and you _delete_
  the object with ID `1`, pfSense will resort the array so the object with ID `2` will now have an ID of `1`.
- API queries will retain the object's ID in the response. In the event that the `data` response field is no longer a
  sequential array due to the query, the `data` field will be represented as an associative array with the array items`
  key being the objects ID.

</details>

### Requirements for queries:

- API call must be a successful GET request and return `0` in the `return` field.
- Endpoints must return an array of objects in the data field (
  e.g. `[{"id": 0, "name": "Test"}, {"id": 1, "name": "Other Test"}]`).
- At least two objects must be present within the data field to support queries.

# Limitations

There are a few key limitations to keep in mind while using this API:<br><br>

- pfSense's XML configuration was not designed for quick simultaneous writes like a traditional database. It may be
  necessary to delay API calls in sequence to prevent unexpected behavior such as configuration overwrites.
- By design, values stored in pfSense's XML configuration can only be parsed as strings, arrays or objects. This means
  that even though request data requires data to be of a certain type, it will not necessarily be stored as that type.
  Data read from the API may be represented differently than the format it was requested as.
