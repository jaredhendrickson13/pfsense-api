# GraphQL

The package also includes a fully-featured [GraphQL API](https://graphql.org/learn/). GraphQL is a popular alternative to 
traditional REST APIs and provides a more flexible and efficient way to interact with objects from the API. The GraphQL 
API is built on top of the same underlying components as the REST API and provides virtually the same functionality as
the REST API, with the added benefits of GraphQL.

!!! Tips
    If you are new to APIs in general, it is recommended to start with the REST API first. The REST API is more
    straightforward and easier to understand for beginners. Once you are comfortable with the REST API, you may
    benefit from the additional features and flexibility provided by the GraphQL API.

## Authentication and Authorization

The GraphQL API uses the same authentication methods and privileges as the REST API. For more information on 
authentication, please refer to the [Authentication and Authorization](AUTHENTICATION_AND_AUTHORIZATION.md) 
documentation.

!!! Important
    The user must at least have the `api-v2-graphql-post` privilege to access the GraphQL API. From there, the user
    must also have the relevant privileges to perform any desired operations within the GraphQL API.

## Content and Accept Types

The GraphQL API supports the same content and accept types as the REST API. However, it is highly recommended to use
`application/json` as both the content and accept types when making requests to the GraphQL API. For more information
on content and accept types, please refer to the [Content and Accept Types](CONTENT_AND_ACCEPT_TYPES.md) documentation.

## Differences from REST API

There are some key differences in behavior between the REST and GraphQL APIs. Below are some of the most notable differences:

- All GraphQL requests are made to the `/api/v2/graphql` endpoint.
- GraphQL requests are made using the `POST` HTTP method exclusively.
- Responses from the GraphQL API follow a different schema than the REST API. The GraphQL API will return a `data` object
  containing the result of the query or mutation, as well as an `errors` array containing any errors that occurred during
  the request.
- All GraphQL responses will return a 200 OK status code, regardless if the result contains errors. This is because GraphQL 
  responses may include both success data and error messages. Do not rely on the status code to determine the success or
  failure of the request, use the `errors` array in the response instead.
- Fields with pre-defined choices must use the GraphQL enum values. See the [Enums](#enums) section for more information.

## Schema and Documentation

The full schema and documentation for the GraphQL API can be found in the [GraphQL reference docs](https://pfrest.org/graphql-docs/).
It is also highly recommended to read through the [GraphQL documentation](https://graphql.org/learn/) to fully understand how
to interact with the GraphQL API.

## Why Use GraphQL?

GraphQL is a powerful tool for building APIs that allow clients to request only the data they need. This can lead to
faster and more efficient API interactions, as clients can avoid over-fetching or under-fetching data. GraphQL also
provides a more flexible way to query and mutate data, allowing clients to obtain all the data they need in a single
request instead of making multiple requests to different endpoints for various resources.

In summary, the primary benefits of the GraphQL API are:

1. **Flexibility**: Clients can specify exactly which field values they need, reducing the amount of data transferred over the network.
2. **Efficiency**: Clients can request multiple resources in a single query, reducing the number of round trips to the server.
3. **Versatility**: GraphQL can handle complex operations out of the box, which often requires custom logic on the client-side when using REST APIs.

However, GraphQL is not always the best choice for everyone. GraphQL is more complex and may require more effort to
implement than a traditional REST API. For simple apps, scripts and integrations where you only need to read or write 
a small number of resources, the REST API may be a more straightforward approach.

## Getting Started

To get started with the GraphQL API, you will need to make requests to the `/api/v2/graphql` endpoint using the `POST`
HTTP method. The body of the request should contain a JSON object with a `query` key and a GraphQL query string as the
value. The query string should look something like this:

```json
{
    "query": "query { operationName { some_field_i_need_data_from } }"
}
```

Take this scenario for example. Let's say you have the following firewall alias configured on your system:

```json
[
  {
    "id": 0,
    "name": "alias1",
    "type": "host",
    "address": [
      "127.0.0.1"
    ],
    "detail": ["localhost"]
  }
]
```

You'd like to read this firewall alias, but you really only need the `name` and `address` fields. With the REST API, you
could make a request to the `/api/v2/firewall/alias` endpoint, but you will always receive the full object. With GraphQL,
you can make a request that only obtains the `name` and `address` fields from the alias:

```
curl -s -k -u admin:pfsense -X POST -H "Content-Type: application/json" -d '{"query": "query { readFirewallAlias(id: 0) { name address } }"}' https://localhost/api/v2/graphql
```

```json
{"data":{"readFirewallAlias":{"name":"alias1","address":["127.0.0.1"]}}}
```

Note how the response only contains the `name` and `address` fields, even though the alias object has more fields. This
is the beauty of GraphQL - you get exactly the data you need, no more, no less.

## Enums

GraphQL uses enums to define a set of possible values for a field. In other words, these are simply a list of pre-defined
choices that a field can have. It is important to note that GraphQL adds an additional layer of abstraction when working
with enums. When querying or mutating objects that contain enum fields, you must use the enum values formatted
as `VAL_<value>`, where `VAL_` is a prefix and `<value>` is the actual value in uppercase. For example, the `type` field of
a firewall alias object may be returned as `VAL_HOST` instead of `host`. When you are querying or mutating objects with
enum fields, you must use these GraphQL enum values instead of the actual values.

!!! Important
    - Enum values will not contain any special characters that may be present in the actual value except for underscores.
    - When working with enum fields in GraphQL, do not wrap the enum value in quotes. They must be passed as raw values.
    Correct usage: `type: VAL_HOST`. Incorrect usage: `type: "VAL_HOST"`.

## Queries and Mutations

GraphQL queries are operations used to read data from the API, while mutations are operations used to create, update, 
or delete data. The GraphQL API supports a wide range of queries and mutations for interacting with the pfSense 
configuration. For a full list of available queries and mutations, please refer to 
the [GraphQL reference docs](https://pfrest.org/graphql-docs/).

!!! Important
    Queries and mutations are executed in the order they are defined in your request. If you need to perform multiple
    operations in a single request, you can define them in the same query or mutation block but you must ensure they are
    defined in the correct order to avoid conflicts.

### Operation Naming

For consistency, query and mutation operation names are based off the equivalent REST API endpoints URL, with the 
following modifications:

1. The `/api/v2/` prefix is not included in the operation name.
2. The operation names are camel-cased (e.g. readFirewallAlias)
3. The operation name will be prefixed with an operation type that directly corresponds with the REST API endpoint's supported HTTP methods:
   - `read` for `GET` requests to [singular endpoints](ENDPOINT_TYPES.md#singular-endpoints)
   - `query` for `GET` requests to [plural endpoints](ENDPOINT_TYPES.md#plural-many-endpoints)
   - `create` for `POST` requests to [singular endpoints](ENDPOINT_TYPES.md#singular-endpoints)
   - `update` for `PATCH` requests to [singular endpoints](ENDPOINT_TYPES.md#singular-endpoints)
   - `replaceAll` for `PUT` requests to [plural endpoints](ENDPOINT_TYPES.md#plural-many-endpoints)
   - `delete` for `DELETE` requests to [singular endpoints](ENDPOINT_TYPES.md#singular-endpoints)
   - `deleteAll` for `DELETE` requests to [plural endpoints](ENDPOINT_TYPES.md#plural-many-endpoints)
   - `deleteMany` for queried `DELETE` requests to [plural endpoints](ENDPOINT_TYPES.md#plural-many-endpoints)

Examples:

- `readFirewallAlias` is equivalent to `GET /api/v2/firewall/alias`
- `queryFirewallAliases` is equivalent to `GET /api/v2/firewall/aliases`
- `createFirewallAlias` is equivalent to `POST /api/v2/firewall/alias`
- `updateFirewallAlias` is equivalent to `PATCH /api/v2/firewall/alias`
- `replaceAllFirewallAliases` is equivalent to `PUT /api/v2/firewall/aliases`
- `deleteFirewallAlias` is equivalent to `DELETE /api/v2/firewall/alias`
- `deleteAllFirewallAliases` is equivalent to `DELETE /api/v2/firewall/aliases` with `all` query parameter set to `true`
- `deleteManyFirewallAliases` is equivalent to `DELETE /api/v2/firewall/aliases` with query parameters to filter the deletion

### Query Syntax

GraphQL queries follow a specific syntax that defines the structure of the request. Multiple queries can be defined in 
a single request to read objects of many different types at once. Below is a basic example of a GraphQL query:

```graphql
query {
    readFirewallAlias(id: 0) {
        name
        address
    }
}
```

In this query, we are asking the API to return the `name` and `address` fields for the alias with an `id` of `0`. The
response will contain only these requested fields.

### Mutation Syntax

Mutations are used to create, update, or delete data in the API. Multiple mutations can be defined in a single request
to perform multiple operations at once. Below is a basic example of a GraphQL mutation:

```graphql
mutation {
    createFirewallAlias(name: "alias2", type: "host", address: ["127.0.0.1"], detail: ["localhost"]) {
        id
        name
        address
    }
}
```

In this mutation, we are creating a new firewall alias. We specify the `name`, `type`, `address`, and `detail` fields for
the new alias and request the response only include the `id`, `name`, and `address` fields of the newly created alias.

### Variables

GraphQL queries and mutations can accept variables to parameterize the request. This allows you to reuse the same query
or mutation with different input values. Variables are defined in the query or mutation string and are passed as a separate
JSON object in the request body. Below is an example of a query with variables:

```json
{
    "query": "query ReadFirewallAlias($id: Int!) { readFirewallAlias(id: $id) { name address } }",
    "variables": {
        "id": 0
    }
}
```

In this query, we define a variable `$id` of type `Int!` (non-nullable integer) and use it in the `readFirewallAlias`
operation. The variable's value is assigned in the `variables` object in the request body. For more information on
variables, refer to the [GraphQL documentation](https://graphql.org/learn/queries/#variables).

## Common Use Cases

GraphQL is a powerful tool that can be used in a variety of ways to interact with the pfSense configuration. Below are
a few common use cases for the GraphQL API:

### Creating an alias and firewall rule in a single request

You can use a single GraphQL request to create a new alias and a new firewall rule that references that alias. 

```graphql
mutation {
    createFirewallAlias(
        name: "example_alias", 
        type: VAL_HOST, 
        address: ["example.com"], 
        detail: ["Example FQDN"]
    ) { id }
    createFirewallRule(
        interface: "lan", 
        type: VAL_PASS, 
        ipprotocol: VAL_INET, 
        protocol: VAL_TCP,
        source: "lan",
        destination: "example_alias",
        descr: "Allow traffic to hosts defined in the newly created alias"   
    ) { id interface type ipprotocol protocol source destination descr }
```

```json
{
  "data": {
    "createFirewallAlias": {
      "id": 1
    },
    "createFirewallRule": {
      "id": 4,
      "interface": [
        "lan"
      ],
      "type": "VAL_PASS",
      "ipprotocol": "VAL_INET",
      "protocol": "VAL_TCP",
      "source": "lan",
      "destination": "alias2",
      "descr": "Allow traffic to hosts defined in the newly created alias"
    }
  }
}
```
