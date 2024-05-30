# Queries and Filters

## Queries

Queries can be used to filter the data that is returned from the API based on specific criteria. Queries are passed as
query parameters in the URL and are formatted as `key=value`. Multiple queries can be passed in a single request by
separating them with an ampersand `&`. There are a couple of requirements for using queries with the REST API:

- Queries are only available for `GET` requests to endpoints.
- Queries are only available on endpoints that return a list of items in the `data` section of the response.

## Query Filters

Below are the available query filters that can be used with the REST API. Each filter has a specific name and format that
must be followed. Filters can be used in combination with each other to create complex queries. To use a filter, simply
add the filter name to the end of the field name in the query parameter separated by two underscores `__`.

### Exact (exact)

Search for objects whose field value matches a given value exactly. This is assumed as the default query filter if no
query filter is specified.

- Name: `exact`
- Examples:
  - `https://pfsense.example.com/api/v2/examples?fieldname=example`
  - `https://pfsense.example.com/api/v2/examples?fieldname__exact=example`

### Starts With (startswith)

Search for objects whose field value starts with a given substring for non-array fields, or search for objects whose field
value array starts with a given value for array fields.

- Name: `startswith`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__startswith=example`

### Ends With (endswith)

Search for objects whose field value ends with a given substring for non-array fields, or search for objects whose field
value array ends with a given value for array fields.

- Name: `endswith`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__endswith=example`

### Contains (contains)

Search for objects whose field value contains a given substring for non-array fields, or search for objects whose field
value array contains a given value for array fields.

- Name: `contains`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__contains=example`

### Less Than (lt)

Search for objects whose field value is less than a given integer.

- Name: `lt`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__lt=5`

### Less Than or Equal To (lte)

Search for objects whose field value is less than or equal to a given integer.

- Name: `lte`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__lte=5`

### Greater Than (gt)

Search for objects whose field value is greater than a given integer.

- Name: `gt`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__gt=5`

### Greater Than or Equal To (gte)

Search for objects whose field value is greater than or equal to a given integer.

- Name: `gte`
- Example: `https://pfsense.example.com/api/v2/examples?fieldname__gte=5`

## Pagination

Pagination can be used to limit the number of items returned in a single request. Pagination is controlled by two query
parameters: `limit` and `offset`. The `limit` parameter specifies the number of items to return, and the `offset`
parameter specifies the starting index of the items to return. Pagination is useful when working with large datasets to
reduce the amount of data returned in a single request.

!!! Important
    By default, the REST API does not paginate responses. If you want to paginate the response, you must include the
    `limit` and `offset` query parameters in your request.
