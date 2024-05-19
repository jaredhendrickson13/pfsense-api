# Working with HATEOAS

HATEOAS (Hypermedia As The Engine Of Application State) is a constraint of the REST application architecture that
allows clients to navigate the API by following links provided by the server. The pfSense REST API package supports
HATEOAS driven development by providing links to related resources in the response data. This allows clients to
easily navigate the API and discover available actions and resources related to the current API response.

!!! Notes 
    - Links are based on [HAL (Hypertext Application Language)](https://stateless.group/hal_specification.html) along
      with some [custom link types](#link-types).
    - HATEOAS is an optional feature and is disabled by default. HATEOAS can be enabled in the
      pfSense webConfigurator under `System` -> `REST API` -> `Settings` by checking the `Enable HATEOAS` checkbox or
      via a `PATCH` request to the [/api/v2/system/restapi/settings](https://pfrest.org/api-docs/#/SYSTEM/RESTAPI%5CEndpoints%5CSystemRESTAPISettingsEndpoint-patch) 
      endpoint's `hateoas` field.
!!! Important
    Enabling HATEOAS can greatly increase the size of API responses as additional links are included in the response data;
    which may also impact performance on large datasets.

### Link types

Below are the different link types that can be returned by the API. These will be found nested under `_links` in the
API response.

!!! Note
    These `_links` can be found both in the root of the API response and nested under specific objects under the
    `data` section. When nested under an object in the `data` section, the links will be specific to that object.

#### next

Provides a link to the next set of data when [pagination](./QUERIES_AND_FILTERS.md#pagination) is used.

#### prev

Provides a link to the previous set of data when [pagination](./QUERIES_AND_FILTERS.md#pagination) is used.

#### self

Provides a link to read an object's own self.

#### update

Provides a link that can be used to update the current object.

#### delete

Provides a link that can be used to delete the current object.

#### pfsense:field:FIELD_NAME

Provides links to the object(s) that are related to the current value(s) of a specific field in the API response.

For example, a static route object could contain a link to the assigned parent gateway's object using the
`gateway` field. This link could be used to make a subsequent API call to obtain the exact parent gateway for the static
route.
