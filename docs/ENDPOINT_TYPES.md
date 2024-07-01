# Endpoint Types

## Singular Endpoints

Singular endpoints are endpoints that interact with and return a single object. For example, a singular endpoint might 
return a single user or a single firewall rule. These endpoints will always return the `data` of the response as an 
associative array. With the exception of `POST` requests, singular endpoints will typically require an 
[object ID](./WORKING_WITH_OBJECT_IDS.md).

Some examples of singular endpoints are:

- [/api/v2/firewall/rule](https://pfrest.org/api-docs/#/FIREWALL/getFirewallRuleEndpoint)
- [/api/v2/user](https://pfrest.org/api-docs/#/USER/getUserEndpoint)
- [/api/v2/interface](https://pfrest.org/api-docs/#/INTERFACE/getNetworkInterfaceEndpoint)

## Plural (Many) Endpoints

Plural endpoints (referred to as `many` endpoints internally) are endpoints that interact with and return multiple objects. 
For example, a plural endpoint might return a list of users or a list of firewall rules. These endpoints will always
return the `data` of the response as an array of associative arrays. Plural endpoints can be used to read many objects at
once (`GET`), or replace all objects of a certain type (`PUT`). Plural endpoints support the use of 
[queries](./QUERIES_AND_FILTERS.md) ange [pagination](./QUERIES_AND_FILTERS.md#pagination).

Some examples of plural endpoints are:

- [/api/v2/firewall/rules](https://pfrest.org/api-docs/#/FIREWALL/getFirewallRulesEndpoint)
- [/api/v2/users](https://pfrest.org/api-docs/#/USER/getUsersEndpoint)
- [/api/v2/interfaces](https://pfrest.org/api-docs/#/INTERFACE/getNetworkInterfacesEndpoint)

!!! Note
    Only certain plural endpoints support `PUT` requests. Refer to the [Swagger documentation](./SWAGGER_AND_OPENAPI.md)
    for each endpoint to determine if `PUT` requests are supported.