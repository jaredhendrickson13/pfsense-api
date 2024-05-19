# Authentication and Authorization

## Authentication

There are three authentication methods available for the pfSense REST API out of the box. The allowed authentication methods
can be configured in the pfSense webConfigurator under `System` -> `REST API` -> `Settings` -> `Authentication Methods`
or via a `PATCH` request to the [/api/v2/system/restapi/settings](https://pfrest.org/api-docs/#/SYSTEM/RESTAPI%5CEndpoints%5CSystemRESTAPISettingsEndpoint-patch) 
endpoint's `auth_methods` field. In addition to these methods, custom authentication methods can be added by extending 
the `Auth` class and implementing your own authentication logic.

!!! Note
    Multiple authentication methods can be enabled at the same time. If multiple methods are
    enabled, the first method that successfully authenticates the user will be used.

### Basic Authentication (Local database)

Basic authentication is the default and simplest form of authentication and is supported by most HTTP clients. This
method uses the same username and password as your pfSense webConfigurator login and requires the client to send an
`Authorization` header with the value `Basic <base64 encoded username:password>`. The REST API will decode the base64
encoded string and attempt to authenticate the user with the provided credentials. Below are two examples of using
Basic authentication with `curl`:

Full syntax:

```bash
curl -H "Authorization: Basic YWRtaW46cGZzZW5zZQo=" https://pfsense.example.com/api/v2/firewall/rules
```

Short syntax:

```bash
curl -u admin:pfsense https://pfsense.example.com/api/v2/firewall/rules
```

!!! Note
    Currently, the REST API does not support Basic authentication with LDAP or RADIUS authentication server backends.
    Only the local database is supported.

### API Key Authentication

API key authentication is a more secure form of authentication that requires the client to send an `X-API-Key` header
containing a valid API key. These keys are better suited to distribute to systems as they cannot allow webConfigurator
or SSH authentication (like local database credentials can). API keys can be generated in the pfSense webConfigurator
under `System` -> `REST API` -> `Keys` or by `POST` request to [/api/v2/auth/key](https://pfrest.org/api-docs/#/AUTH/RESTAPI%5CEndpoints%5CAuthKeyEndpoint-post). 
Below is an example of API Key authentication using `curl`:

```bash
curl -H "X-API-Key: xxxxxxxxxxxxxxxxxxxxxxx" https://pfsense.example.com/api/v2/firewall/rules
```

!!! Important
    API keys utilize the privileges assigned to the user that generated the key.
!!! Warning
    API keys are sensitive information and should be treated as such. Do not share your API key with anyone you do not
    trust. If you believe your API key has been compromised, you can revoke it in the pfSense webConfigurator or by
    making a DELETE request to the [/api/v2/auth/key](https://pfrest.org/api-docs/#/AUTH/RESTAPI%5CEndpoints%5CAuthKeyEndpoint-delete) endpoint.

### JSON Web Token (JWT) Authentication

JSON Web Token (JWT) authentication is a stateless, secure authentication method that uses a signed token to authenticate
users. This method requires the client to send a `Bearer` token in the `Authorization` header. The token is signed with
a secret key that only the REST API and the client know. JWT tokens can be generated using the 
[/api/v2/auth/jwt](https://pfrest.org/api-docs/#/AUTH/RESTAPI%5CEndpoints%5CAuthJWTEndpoint-post) endpoint
and require the client to authenticate as a local database user. Below is an example of receiving a JWT token using
`curl`:

```bash
curl -X POST -H "Content-Type: application/json" -u admin:pfsense https://pfsense.example.com/api/v2/auth/jwt
{"code":200,"status":"ok","response_id":"SUCCESS","message":"","data":{"token":"xxxxxxxxxxxxxxxxxxx"}}
```

And an example of using the JWT token to authenticate a request using `curl`

```bash
curl -H "Authorization: Bearer xxxxxxxxxxxxxxxxxxxxxxx" https://pfsense.example.com/api/v2/firewall/rules
```

!!! Important
    JWTs are only valid for a limited time (default 1 hour) and must be regenerated after they expire. The expiration
    interval can be configured in the REST API settings within the pfSense webConfigurator or via a `PATCH` request to
    the [/api/v2/system/restapi/settings](https://pfrest.org/api-docs/#/SYSTEM/RESTAPI%5CEndpoints%5CSystemRESTAPISettingsEndpoint-patch)
    endpoint's `jwt_exp` field.

### Custom Authentication

For advanced users, the REST API's framework allows for custom authentication methods to be added using PHP. To add a custom
authentication method, extend the `\RESTAPI\Core\Auth` class and implement the `_authenticate` method. This method simply
needs to return a boolean value indicating if the user is authenticated or not. Below is an example of a custom Auth class:

```php
<?php

namespace RESTAPI\Auth;

require_once "RESTAPI/autoloader.inc";

use RESTAPI\Core\Auth;

class MyCustomAuth extends Auth
{
  # Defines a human-readable name for your custom auth method
  public ?string $verbose_name = "My Custom Auth";

  # Tells OpenAPI how to use your custom auth method in the Swagger documentation. In this example, we are telling
  # OpenAPI to check for an authentication secret in the HTTP header named 'custom-header-name'
  public array $security_scheme = [
    "type" => "apiKey",
    "in" => "header",
    "name" => "custom-header-name",
  ];

  public function _authenticate(): bool
  {
    # Add your custom authentication logic here
    if ($my_custom_auth_logic) {
      return true;
    }

    # Return false if authentication fails
    return false;
  }
}
```

Once your custom Auth class is created, simply place it at `/usr/local/pkg/RESTAPI/Auth/MyCustomAuth.inc` and it will
automatically become an available authentication method in the REST API settings.

!!! Note
    Rename `MyCustomAuth` to any name you like, but make sure the class name matches the file name.

## Authorization

The REST API uses the same privilege system as the pfSense webConfigurator to determine what actions a user can perform.
However, a unique privilege is generated for each API endpoint and it's associated HTTP methods. These privileges can
be assigned to users the same way as any other privilege in pfSense.

!!! Note
    The REST API will respect the `WebCfg - All pages` and `User - Config: Deny Config Write` privileges. Users with
    the `WebCfg - All pages` privilege will have full access to the REST API. Users with the `User - Config: Deny
    Config Write` privilege will be allowed to make API calls but attempts to modify the configuration will be denied.
