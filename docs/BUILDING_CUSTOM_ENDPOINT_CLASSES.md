#  Building Custom Endpoint Classes

Endpoint classes allow a specified Model class to be exposed via the REST API. Without an Endpoint class, a Model 
class will not be accessible via the REST API and can only be utilized locally within the package via PHP. Endpoint
classes are also responsible for following:

- Defining the URL path for the endpoint.
- Specifying which request methods are allowed.
- Adding additional documentation to the endpoint's OpenAPI definition.
- Defining an associated GraphQL query/mutation type.
- Generating the PHP file in the pfSense web root to expose the endpoint.

## Getting Started

Use the following class template to initialize your custom Endpoint class:
    
```php
<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * TODO: Add a description of your Endpoint class here.
 */
class MyCustomEndpoint extends Endpoint {
    public function __construct() {
        # TODO: Set endpoint attributes here
    
        parent::__construct();
    }
}
```

!!! Important
    Be sure to place this class file within `/usr/local/pkg/RESTAPI/Endpoints/` directory and name the file after your
    Endpoint class name with a `.inc` extension.
!!! Warning
    The parent constructor must be called at the end of the `__construct` method to initialize the Endpoint class.

## Define __construct() Method Properties

The `__construct` method is used to define the properties of the Endpoint class. These properties allow you to customize the
behavior and attributes of the endpoint. The following properties are available:

### url

The `url` property is used to define the URL path for the endpoint. This path will be used to automatically generate the
PHP file in the pfSense webroot after executing the `pfsense-restapi buildendpoints` command. The URL path should unique 
and begin with the `/api/v2/` prefix. This property is required.

Example:
    
```php
$this->url = '/api/v2/my/custom/endpoint';
```

!!! Important
    This URL should NOT end with a trailing slash `/`, and should not contain a file extension such `index.php`. 
    The REST API will automatically generate the `index.php` file for the endpoint at the specified URL path.

### model_name

The `model_name` property is used to define the Model class that the endpoint will expose. This must be the Model 
class's short name, not the fully qualified class name. This property is required.

!!! Important
    The Model class set in this property must be defined within the `/usr/local/pkg/RESTAPI/Models/` directory.

### many

The `many` property is used to define if the endpoint will return a single object or many objects. This property
defaults to `false`. When set to `true`, the endpoint will return an array of objects in the `data` section of the response.
When set to `false`, the endpoint will return a single object in the `data` section of the response.

For more information on the difference between many and non-many endpoints, refer to the [Endpoint Types](ENDPOINT_TYPES.md) documentation.

!!! Important
    The `many` property can only be set to `true` if the assigned `model_name` property is a Model class that is also `many` enabled.

### request_method_options

The `request_method_options` property is used to define which request methods are allowed for the endpoint. If a request
method is not defined in this property, the endpoint will return a `405 Method Not Allowed` response when accessed with
that method. This property is required.

Each supported request method corresponds with a the following Model class methods:

- A `GET` request to a `many` enabled Endpoint will call the Model's `read_all()` method.
- A `GET` request to a non-`many` enabled Endpoint will call the Model's `read()` method.
- A `POST` request to a non-`many` enabled Endpoint will call the Model's `create()` method.
- A `PATCH` request to a non-`many` enabled Endpoint will call the Model's `update()` method.
- A `DELETE` request to a non-`many` enabled Endpoint will call the Model's `delete()` method.
- A `PUT` request to a `many` enabled Endpoint will call the Model's `replace_all()` method.

!!! Note
    The OPTIONS request method is automatically supported by all endpoints and does not need to be defined in the
    `request_method_options` property. The OPTIONS request method is used to return the allowed request methods for the
    endpoint. A successful options request will return a `200 OK` response with the `Allow` and `access-control-allow-methods`
    response header set to the allowed request methods.

### tag

The `tag` property is used to define the [OpenAPI tag](https://swagger.io/docs/specification/grouping-operations-with-tags/)
for the endpoint. This is used to group related endpoints together in the API documentation. This property is optional 
and defaults to the first section of the URL after the `/api/v2/` prefix.

### requires_auth

The `requires_auth` property is used to specify whether the endpoint requires authentication and authorization. This property
defaults to `true`.

!!! Danger
    Setting the `requires_auth` property to `false` will expose the endpoint to the public without any authentication or
    authorization checks. Do not set this property to `false` unless you are certain that the endpoint should be public.

### auth_methods

The `auth_methods` property is used to define the authentication methods that are allowed for the endpoint. This should
be an array of [Auth class names](https://pfrest.org/php-docs/namespaces/restapi-auth.html) or leave empty to allow all.
This property defaults to an empty array.

!!! Notes
    - This property is only applicable if the `requires_auth` property is set to `true`.
    - Specified class names should be the short name of the Auth class, not the fully qualified class name.

### ignore_read_only

The `ignore_read_only` property is used to define if the endpoint should ignore the REST API's read-only mode and allow
non-`GET` requests even when read-only mode is enabled. This property defaults to `false`.

!!! Danger
    Setting the `ignore_read_only` property to `true` will allow the endpoint to be modified even when the REST API is
    in read-only mode. Do not set this property to `true` unless you are certain that the endpoint should be writable
    when the REST API is in read-only mode.

### ignore_interfaces

The `ignore_interfaces` property is used to define if the endpoint should ignore the REST API's allowed interfaces setting
and allow all interfaces to access the endpoint. This property defaults to `false`.

!!! Danger
    Setting the `ignore_interfaces` property to `true` will allow all interfaces to access the endpoint even if the
    REST API's allowed interfaces setting is set. Do not set this property to `true` unless you are certain that the
    endpoint should be accessible from all interfaces.

### ignore_enabled

The `ignore_enabled` property is used to define if the endpoint should ignore the REST API's enabled setting and allow
the endpoint to be accessed even when the REST API is disabled. This property defaults to `false`.

!!! Danger
    Setting the `ignore_enabled` property to `true` will allow the endpoint to be accessed even when the REST API is
    disabled. Do not set this property to `true` unless you are certain that the endpoint should be accessible when the
    REST API is disabled.

### ignore_acl

The `ignore_acl` property is used to define if the endpoint should ignore the REST API's ACL settings and allow all
users and IPs to access the endpoint at all times. This property defaults to `false`.

!!! Danger
    Setting the `ignore_acl` property to `true` will allow all users and IPs to access the endpoint at all times. Do not
    set this property to `true` unless you are certain that the endpoint should be accessible to all users and IPs.

### get_help_text

The `get_help_text` property is used to overwrite the default OpenAPI description for the endpoint's GET documentation.

### post_help_text

The `post_help_text` property is used to overwrite the default OpenAPI description for the endpoint's POST documentation.

### patch_help_text

The `patch_help_text` property is used to overwrite the default OpenAPI description for the endpoint's PATCH documentation.

### delete_help_text

The `delete_help_text` property is used to overwrite the default OpenAPI description for the endpoint's DELETE documentation.

### put_help_text

The `put_help_text` property is used to overwrite the default OpenAPI description for the endpoint's PUT documentation.

## Example

Below is an example of a fully implemented Endpoint class:

```php
<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * TODO: Add a description of your Endpoint class here.
 */
class MyCustomEndpoint extends Endpoint {
    public function __construct() {
        # Set required endpoint properties
        $this->url = '/api/v2/my/custom/endpoint';
        $this->model_name = 'MyCustomModel';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];
        $this->many = false;
        
        # Set optional endpoint properties
        $this->tag = 'My Custom Endpoints';
        $this->requires_auth = true;
        $this->auth_methods = ['BasicAuth', "JWTAuth", "KeyAuth"];
        $this->ignore_read_only = false;
        $this->ignore_interfaces = false;
        $this->ignore_enabled = false;
        $this->ignore_acl = false;
        $this->get_help_text = 'Example GET description.';
        $this->post_help_text = 'Example POST description.';
        $this->patch_help_text = 'Example PATCH description.';
        $this->delete_help_text = 'Example DELETE description.';
    }
}
```

## Build the Endpoint

The Endpoint class is simply a blueprint or instruction set for the REST API endpoint. A PHP file must be present in the
pfSense web root to actually expose the endpoint. The REST API's framework will automatically generate the PHP file for all
Endpoint class URLs by running the following command:

```shell
pfsense-restapi buildendpoints
```

After running the command, the PHP file for the Endpoint will be generated in the pfSense web root (`/usr/local/www/`)
and the endpoint will be accessible via the REST API.

## Generating Documentation

To regenerate the OpenAPI and GraphQL schemas for all Endpoint classes, run the following command:

```shell
pfsense-restapi buildschemas
```

## Examples

You can find examples of fully implemented Endpoint classes in the [PHP reference](https://pfrest.org/php-docs/namespaces/restapi-endpoints.html).
Select the Endpoint class you are interested in to view the class's PHPDoc documentation, and then click on the
`<>` symbol next to the class name to view the class's source code.