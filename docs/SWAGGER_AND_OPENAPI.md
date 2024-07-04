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

## OpenAPI Schema

The pfSense REST API package was designed around the OpenAPI specification and is built to automatically document its
components as OpenAPI schemas. The full OpenAPI schema is available at the `/api/v2/schema` endpoint. This schema can be
used to quickly generate client libraries, documentation, and more.
