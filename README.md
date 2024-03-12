# pfSense REST API Package

[![Build](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/build.yml/badge.svg?branch=next_major)](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/build.yml)
[![Quality](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/quality.yml/badge.svg?branch=next_major)](https://github.com/jaredhendrickson13/pfsense-api/actions/workflows/quality.yml)

Welcome to the pfSense-pkg-RESTAPI repository! This package offers a robust and secure REST API for pfSense firewalls.
It is designed to streamline automation by transforming the PHP functions and processes used by pfSense's
webConfigurator into versatile API endpoints. This document aims to provide comprehensive details about installation,
configuration, authentication, and usage of the pfSense REST API.

> [!TIP]
> Detailed API endpoint documentation can be found in the [webConfigurator after installation](#webconfigurator-settings--documentation).

## Requirements

### Supported pfSense Versions

- pfSense CE 2.7.2 (amd64)

> [!TIP]
> Don't see your version listed? Check the releases page. Older versions of this package may support older versions of
> pfSense.

> [!WARNING]
> This package may work on other architectures such as arm64 and aarch64, but compatibility is untested and therefor not
> guaranteed.

## Installation

Install pfSense-pkg-RESTAPI:

```bash
pkg-static -C /dev/null add https://github.com/jaredhendrickson13/pfsense-api/releases/latest/download/pfSense-2.7-pkg-API.pkg
```

Uninstall pfSense-pkg-RESTAPI:

```bash
pfsense-restapi delete
```

Update pfSense-pkg-RESTAPI to the latest stable version:

```bash
pfsense-restapi update
```

Revert to a previous version of pfSense-pkg-RESTAPI (e.g., v1.1.7):

```bash
pfsense-restapi revert v1.1.7
```

> [!NOTE]
> You may need to customize the installation command to reference the package built for your pfSense version. Check
> the [releases page](https://github.com/jaredhendrickson13/pfsense-api/releases) for available versions.

> [!IMPORTANT]
> After updating pfSense, **you must reinstall this package** as pfSense removes unofficial packages during updates.

## webConfigurator Settings & Documentation

After installation, access the REST API user interface within the pfSense webConfigurator under 'System > REST API.'
The 'Settings' tab allows configuration of API interfaces, authentication modes, and more. The 'Documentation' tab
provides an embedded tool for viewing the full API documentation within the context of your pfSense instance.

> [!NOTE]
> Users need `page-all` or `page-system-restapi-settings` privileges to access the API page.

## Authentication & Authorization

### Authentication Methods

By default, the API uses the same credentials as the webConfigurator. Users can configure API authentication in '
System > REST API' within the pfSense webConfigurator. External authentication servers like LDAP or RADIUS are not
supported at this time. Available authentication methods are:

<details>
    <summary>Local Database (default)</summary>

Use basic authentication with the same credentials as the pfSense webConfigurator:

```bash
curl -u admin:pfsense https://pfsense.example.com/api/v2/firewall/rules
```

</details>

<details>
    <summary>JWT</summary>

Requires a bearer token to be included in the Authorization header of your request. These are time-based tokens that
will expire after the configured amount of time. To receive a JWT, you must make a POST request to the /api/v2/auth/jwt
endpoint. This endpoint will always require the use of the Local Database authentication type to receive the JWT. For
example:

1. Request a JWT
   ```bash
   curl -u admin:pfsense -X POST https://pfsense.example.com/api/v2/auth/jwt
   ```
2. Use the obtained JWT for API calls:
   ```bash
   curl -H "Authorization: Bearer xxxxx.xxxxxx.xxxxxx" -X GET https://pfsense.example.com/api/v2/diagnostics/arp_table
   ```

- To configure the JWT expiration time, navigate to 'System > REST API' within the webConfigurator and configure the JWT
  Expiration value. Alternatively, you can use the /api/v2/system/restapi/settings endpoint to update the `jwt_exp`
  value.

</details>

<details>
    <summary>Key</summary>

Uses standalone keys generated via API or webConfigurator. These are better suited to distribute to systems as they are
revocable and will only allow API authentication; not webConfigurator or SSH authentication (like the local database
credentials).

```bash
curl -H "Authorization: KeyAuth API_KEY_HERE" -X GET https://pfsense.example.com/api/v2/diagnostics/arp_table
```

- To generate API keys, navigate to 'System > REST API' and select the 'Keys' tab. Alternatively, you can generate new
  API keys via API at /api/v2/auth/key

</details>

### Authorization:

The API provides specific privileges for each API endpoint and its associated HTTP methods to provide granular privilege
options. These privileges can be assigned like any other pfSense privilege via webConfigurator or by API. Users with "
page-all" privileges are automatically granted access to all API endpoints and methods.

### Login Protection

All API requests are monitored by pfSense's Login Protection feature by default. This is an important security measure
to help thwart brute force attacks against API endpoints. Although strongly discouraged, this can be disabled under '
System > REST API'.

## Content Types

Specify the `Content-Type` header in requests to avoid undesired results. The recommended content type
is `application/json`, but other supported types include:

<details>
    <summary>application/json</summary>

- Parses the request body as a JSON formatted string:

  ```bash
  curl -u admin:pfsense -H "Content-Type: application/json" -d '{"name": "sshd", "action": "restart"}' -X POST https://pfsense.example.com/api/v2/status/service
  ```

</details>

<details>
    <summary>application/x-www-form-urlencoded</summary>

- Parses the request body as URL encoded parameters:

  ```bash
  curl -u admin:pfsense -H "Content-Type: application/x-www-form-urlencoded" -X POST https://pfsense.example.com/api/v2/status/service?name=sshd&action=restart
  ```

</details>

> [!IMPORTANT] > `application/x-www-form-urlencoded` is suitable for GET or DELETE requests with string, integer, or boolean
> data types.

## Queries

pfSense-pkg-RESTAPI includes an advanced query engine for specific data retrieval from API calls. For `GET` requests, users
can query the return data to retrieve desired information. Add query parameters to the payload to match criteria or use
query filters for more complex queries. Query filters may be specified after the field name separated by a
double-underscore.

<details>
    <summary>Query Filters</summary>

### Exact

Search for objects whose field value matches a given value exactly. This is assumed as the default query filter if no
query filter is specified.

- Name: `exact`
- Examples:
  - `https://pfsense.example.com/api/v2/examples?fieldname=example`
  - `https://pfsense.example.com/api/v2/examples?fieldname__exact=example`

### Starts With

Search for objects whose field value starts with a given substring.

- Name: `startswith`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__startswith=example`

### Ends With

Search for objects whose field value ends with a given substring.

- Name: `endswith`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__endswith=example`

### Contains

Search for objects whose field value contains a given substring.

- Name: `contains`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__contains=example`

### Less Than

Search for objects whose field value is less than a given integer.

- Name: `lt`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__lt=5`

### Less Than or Equal To

Search for objects whose field value is less than or equal to a given integer.

- Name: `lte`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__lte=5`

### Greater Than

Search for objects whose field value is greater than a given integer.

- Name: `gt`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__gt=5`

### Greater Than or Equal To

Search for objects whose field value is greater than or equal to a given integer.

- Name: `gte`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__gte=5`

### Has

Search for objects field value is an array that has a given value. This query filter is only supported on array-type
fields.

- Name: `gte`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__has=example`

</details>

### Pagination

Pagination comes built-in and is a fantastic tool to help manage the number of objects fetched from the
API at a time, giving you greater control over your application's performance during integration. To make use of
pagination, just toss in the `limit` query parameter to specify how many objects you want in one go, and add the `offset`
query parameter to specify where in the dataset you'd like to start fetching objects. Pagination is only performed
when one of these query parameters is provided. Make navigating through your data a breeze!

> [!NOTE]
> Data within the `_links` sections of the API response cannot be queried.

## HATEOAS

The REST API designed to effortlessly incorporate HATEOAS using HAL (Hypertext Application Language) standards,
complemented by a few custom enhancements. As you receive a response, you'll find handy links to related resources,
making navigation a breeze and granting swift access to additional, relevant information. This reduces the need for
applications to hardcode URLs for related items.

> [!NOTE]
> HATEOAS is disabled by default. HATEOAS can be enabled in the System > REST API > Settings webConfigurator page or via
> the /api/v2/system/restapi/settings API endpoint. While disabled, the `_links` field will not show up in the API
> response schemas.

> [!WARNING]
> Depending on the size of your pfSense instance's configuration, enabling HATEOAS can dramatically increase the size
> of API responses and may increase API response times.

### Link types

Below are the different link types that can be returned by the API. These will be found nested under `_links` in the
API response.

> [!NOTE]
> These `_links` can be found both in the root of the API response as well as nested under specific objects under the
> `data` section. When nested under an object in the `data` section, the links will be specific to that object.

#### next

Provides a link to the next set of data when [pagination](#pagination) is used.

#### prev

Provides a link to the previous set of data when [pagination](#pagination) is used.

#### self

Provides a link to read an object's own self.

#### update

Provides a link that can be used to update an object.

#### delete

Provides a link that can be used to delete an object.

#### pfsense:field:FIELD_NAME

Provides links to the object(s) that are related to the current value(s) of a specific field in the API response.

For example, a static route object could contain a link to the assigned parent gateway's object using the
`gateway` field. This link could be used to make a subsequent API call to obtain the exact parent gateway for the static
route.

#### pfsense:service:ACTION

Provides links to start, stop, and/or restart services associated with the API response.

## Limitations

Keep the following limitations in mind when using this API:

- pfSense's XML configuration was not designed for large scale concurrency. It may be necessary to delay API calls to
  prevent unexpected behavior, such as configuration overwrites.
