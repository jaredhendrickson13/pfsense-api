# Endpoint Types

The REST API's endpoints can be categorized into two main types: singular and plural. Both these endpoint types have
different purposes and handle objects in different ways. Understanding the differences between these two types of
endpoints will help you determine which endpoint to use for your specific use case.

!!! Important
    This document provides details on the default behavior of singular and plural endpoints. However, the behavior of
    specific endpoints may vary. Always refer to the [API documentation](https://pfrest.org/api-docs/) for the specific
    endpoint you are working with to determine the supported methods and behavior. The endpoint documentation will also include
    which type of endpoint it is, and what methods are supported.

## Singular Endpoints

Singular endpoints are endpoints that interact with and return a single object. For example, a singular endpoint might 
return a single user or a single firewall rule. These endpoints will always return the `data` of the response as an 
associative array. Singular endpoints will often require an [object ID](./WORKING_WITH_OBJECT_IDS.md).

Some examples of singular endpoints are:

- [/api/v2/firewall/rule](https://pfrest.org/api-docs/#/FIREWALL/getFirewallRuleEndpoint)
- [/api/v2/user](https://pfrest.org/api-docs/#/USER/getUserEndpoint)
- [/api/v2/interface](https://pfrest.org/api-docs/#/INTERFACE/getNetworkInterfaceEndpoint)

### GET Requests to Singular Endpoints

A `GET` request to a singular endpoint will allow you to retrieve a single object by providing the object's ID (if required).
If you are looking to retrieve or search for multiple objects, you should use a [GET request to the applicable plural endpoint](#get-requests-to-plural-endpoints)
instead.

### POST Requests to Singular Endpoints

A `POST` request to a singular endpoint will allow you to create a new object, or initiate a specific action (depending on
the endpoint). This is useful when you need to create a new object, such as a new user, firewall rule, or interface.

### PATCH Requests to Singular Endpoints

A `PATCH` request to a singular endpoint will allow you make updates to an existing object by providing the object's ID (if required).
PATCH requests support partial updates, meaning you can update only the fields you need to change, rather than replacing the
entire object. This is useful when you need to update an object without providing all of the object's fields over again.

### DELETE Requests to Singular Endpoints

A `DELETE` request to a singular endpoint will allow you to delete an object by providing the object's ID. If you are
looking to delete multiple objects, or need to delete objects based on a field other than the ID, you should use a
[DELETE request to the applicable plural endpoint](#delete-requests-to-plural-endpoints) instead.


## Plural (Many) Endpoints

Plural endpoints (referred to as `many` endpoints internally) are endpoints that interact with and return multiple objects. 
For example, a plural endpoint might return a list of users or a list of firewall rules. These endpoints will always
return the `data` of the response as an array of associative arrays. 

Some examples of plural endpoints are:

- [/api/v2/firewall/rules](https://pfrest.org/api-docs/#/FIREWALL/getFirewallRulesEndpoint)
- [/api/v2/users](https://pfrest.org/api-docs/#/USER/getUsersEndpoint)
- [/api/v2/interfaces](https://pfrest.org/api-docs/#/INTERFACE/getNetworkInterfacesEndpoint)

### GET Requests to Plural Endpoints

A `GET` request to a plural endpoint will allow you to retrieve a list of all existing objects at once. GET requests to plural
endpoints also support [querying and filtering](QUERIES_AND_FILTERS.md) to help you narrow down the list of objects
returned by specifying criteria for the objects you want to retrieve. This includes support for 
[pagination](QUERIES_AND_FILTERS.md#pagination) which will allow you to limit the amount of objects returned in a single request.

### PUT Requests to Plural Endpoints

A `PUT` request to a plural endpoint will allow you to replace all existing objects with a new set. This is 
useful when you need to ensure that the current dataset is entirely replaced with a new set of objects, such as during 
system configuration updates, data synchronization, inventory management, or when deploying consistent configurations 
across multiple services. This also ensures that your applications do not need to manage the state of the objects
one by one, and can instead replace the entire dataset as a whole in a single request.

!!! Warning
    Any objects not included in the new set will be effectively overwritten. Ensure you exercise caution when using 
    this method to replace objects to avoid potential disruptions.

!!! Note
    - Not all plural endpoints support PUT requests. Please refer to the [documentation](https://pfrest.org/api-docs/) for
    the specific endpoint you are working with to determine if PUT requests are supported.
    - PUT requests do not support queries or pagination.
    - The [`apply` control parameter](COMMON_CONTROL_PARAMETERS.md#apply) has no effect on PUT requests to plural endpoints.
    Unless the endpoint always applies changes immediately, you must make a separate call to the applicable apply endpoint
    to apply the change.

### DELETE Requests to Plural Endpoints

A `DELETE` request to a plural endpoint will allow you to delete many objects at once using a [query](QUERIES_AND_FILTERS.md).
This is useful when you need to remove a large number of objects from the system, such as when decommissioning services,
cleaning up old data, or removing objects that are no longer needed. This is primarily used as a method of deleting
objects without requiring an ID.

At least one query parameter must be provided in the DELETE request. This is to ensure that you are
intentionally deleting objects, and to prevent accidental deletions of all objects in the system. If the desired action is
to delete all objects, you can set the `all` query parameter to `true`.

!!! Warning
    - This will delete all objects that match the query. Be sure to use caution when
    deleting objects in this way to avoid disruptions.
    - DELETE requests to plural endpoints are non-atomic. If an error occurs while deleting objects, objects that were 
    already been deleted before the error will remain deleted.

!!! Note
    - Not all plural endpoints support DELETE requests. Please refer to the [documentation](https://pfrest.org/api-docs/) for
    the specific endpoint you are working with to determine if PUT requests are supported.
    - The `all` query parameter is exclusive to DELETE requests to plural endpoints.
    - The [`apply` control parameter](COMMON_CONTROL_PARAMETERS.md#apply) has no effect on DELETE requests to plural endpoints.
    Unless the endpoint always applies changes immediately, you must make a separate call to the applicable apply endpoint
    to apply the change.
