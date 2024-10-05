# Swagger and OpenAPI

## Swagger Documentation

After installation, an interactive Swagger documentation site is available on your pfSense instance under
'System' -> 'REST API' -> 'Documentation'. This site provides full documentation for all API endpoints and their
associated parameters, as well as provides a quick and interactive way to try out API calls. Alternatively, a non-interactive
version of the Swagger documentation is available publicly [here](https://pfrest.org/api-docs/).

!!! Note
    When you expand an endpoint for `GET` and `DELETE` requests, Swagger will display field options as query parameters.
    For `POST`, `PATCH` and `PUT` requests, Swagger will display field options as request body parameters under the
    'Schema' tab.

### Understanding Endpoint Details

Each endpoint in the Swagger documentation contains a 'Details' section that provides information about certain
characteristics of the endpoint. These characteristics include:

#### Endpoint type

Displays the [Endpoint type](ENDPOINT_TYPES.md) for the endpoint. This will tell you if the endpoint interacts with a 
single object or multiple objects at once.

#### Associated model

Displays the [Model class](https://pfrest.org/php-docs/namespaces/restapi-models.html) that the endpoint interacts with.
This will also tell you the name of the model's OpenAPI schema definition. For most users, this information is not
necessary to know, but it can be useful for developers who are looking to understand the underlying code.

#### Parent model

For endpoints that interact with objects that are children of other objects, this field will display the parent Model's
class name. When a parent model is defined, the endpoint will require the 
[parent object's ID](WORKING_WITH_OBJECT_IDS.md#object-parent_id-field) as a parameter.

#### Requires authentication

Indicates whether the endpoint requires authentication. If authentication is required, the endpoint will require one of
the [supported authentication methods](#supported-authentication-methods) to be used.

#### Supported authentication methods

Displays which [authentication methods](AUTHENTICATION_AND_AUTHORIZATION.md#authentication) are supported by the 
endpoint. In most cases, the endpoint will support all authentication method available to the REST API. However, some
endpoints may have restrictions on which authentication methods can be used. Those restrictions will be displayed here.

#### Allowed privileges

Displays which pfSense privileges allow access to the endpoint. In general, there will be a privilege unique to the
endpoint and HTTP method combination, as well as the `page-all` privilege which allows access to all endpoints. Any
additional privileges that allow access to the endpoint will also be displayed here.

!!! Note
    The authenticating user must have at least one of the privileges listed in the 'Allowed privileges' section to
    access the endpoint.

#### Required packages

For endpoints that interact with a pfSense add-on package, this field will display the name of the package that is
required to use the endpoint. If the package is not installed, the endpoint will not be available to use until it
has been installed.

!!! Note
    If multiple packages are listed, all packages must be installed to use the endpoint.

#### Applies immediately

Indicates whether any associated backend services will be restarted/reloaded immediately after the endpoint is called
to apply the changes that were written to config. If this field is set to 'Yes', the changes will be written to config
and applied immediately. If this field is 'No', the changes will be written to config only. If there is an 
associated service that needs to be restarted/reloaded, the changes will not take effect until the service is 
restarted/reloaded. This is typically done by calling the relevant service's `apply` endpoint.

!!! Note
    - This functionality is made to mirror the behavior of the pfSense webConfigurators 'Apply Changes' button. However,
    not all endpoints interact with services that require a restart/reload to apply changes. In these cases, the
    'Applies immediately' attribute may not be relevant.
    - For endpoints that do not immediately apply changes, you can override the default behavior of the endpoint to 
    immediately apply changes by setting the [`apply` control parameter to `true`](COMMON_CONTROL_PARAMETERS.md#apply).

#### Utilizes cache

Indicates whether the Model class associated with the endpoint utilizes a cache to populate data. This attribute will
display the name of the [Cache class used](https://pfrest.org/php-docs/namespaces/restapi-caches.html) by the Model class.
If a cache is utilized, data returned by the endpoint is fetched periodically in the background on a schedule. This means
the data returned by the endpoint may not always be up-to-date.

## OpenAPI Schema

The pfSense REST API package was designed around the OpenAPI specification and is built to automatically document its
components as OpenAPI schemas. The full OpenAPI schema is available at the `/api/v2/schema/openapi` endpoint. This 
schema can be used to quickly generate client libraries, documentation, and more.
