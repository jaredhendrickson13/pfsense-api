# Common Control Parameters

The API utilizes a set of common parameters to control certain behaviors of API calls. These parameters are available to
all endpoints and requests, but some endpoints may not support all parameters. Below are the available control
parameters you can use:

## append

- Type: Boolean
- Default: `false`
- Description: This parameter allows you to control the behavior of updates to array fields. When set to `true`, any
  array values submitted in your request will be appended to the existing array values instead of replacing them. This
  is useful when you want to add additional values to an array field without needing to retrieve the existing values
  first.

!!! Warning
    If you set this parameter to `true`, it will apply to all array fields. You can't choose to append to some array 
    fields and replace others at the same time. To work around this, first make a request with the data for the fields 
    you want to append to. Then, make another request for the fields you want to replace.

!!! Notes
    - This parameter is only available for `PATCH` requests.
    - This parameter is only applicable to array fields.
    - If the new array values match the existing array values exactly, the API will not make any changes to that field.

## apply

- Type: Boolean
- Default: `false`
- Description: This parameter allows you to control the default API behavior of _not_ applying changes to the pfSense
  configuration immediately. Setting this parameter to `true` will force the API to apply the changes immediately. This
  is useful when you are making a small change and want to apply it immediately without needing to make a separate API
  call to the applicable apply endpoint. This may not be applicable to all endpoints as some always apply changes
  immediately.

!!! Tip
    The [Swagger documentation](SWAGGER_AND_OPENAPI.md#swagger-documentation) will indicate if an endpoint applies
    changes immediately or requires a separate apply call. If an endpoint applies changes immediately, this parameter
    will have no effect.

## async

- Type: Boolean
- Default: `true`
- Description: This parameter allows you to control the API's default behavior of applying changes in the background. Setting
  this parameter to `false` will force the API to wait for the changes to be made on the backend before responding. This
  parameter may not be supported by all endpoints.

!!! Tip
    Instead of forcing the API to wait for changes to apply, it is recommended to leave this parameter set to `true`
    and periodically check the status of the changes using a `GET` request to the applicable apply endpoint.
!!! Warning
    Setting this parameter to `false` may cause the API to hang if the changes take a long time to apply. In severe cases,
    this may result in your API request receiving a timeout. It is recommended to use this parameter with caution and
    only when necessary.

## placement

- Type: Integer
- Default: _Defaults to next available ID for new objects, or the current ID for existing objects._
- Description: This parameter allows you to control the placement of new and existing objects in the pfSense configuration.
  This effectively allows you to place or move objects to a specific location/ID in the configuration. This parameter is
  only available to POST and PATCH requests targeting objects that are stored in the pfSense configuration. Some endpoints
  force specific sorting attributes that may override this parameter.

!!! Warning
    Use caution when setting placement of objects which may be sensitive to order such as firewall rules. Placing the
    object in the wrong location may have unintended consequences such as blocking all traffic or allowing all traffic.

## remove

- Type: Boolean
- Default: `false`
- Description: This parameter allows you to control the behavior of array fields. When set to `true`, any array values
  submitted in your request will be removed from the existing array values. This is useful when you want to remove
  specific values from an array field without needing to retrieve the existing values first.

## reverse

- Type: Boolean
- Default: `false`
- Description: This parameter allows you to reverse the order of the objects returned in the data section of the API
  response. This enables you to paginate through objects in reverse order, which is particularly useful if you are
  looking for an object near the end of the list. Additionally, it is helpful for time-sorted objects, such as logs,
  where you may want to view the most recent entries first.

## sort_by

- Type: String or Array
- Default: _Defaults to the primary sort attribute for the endpoint, typically `null`._
- Description: This parameters allows you to select the fields to use to sort the objects related to the endpoint. The
  behavior of this parameter varies based on the request method and endpoint type. Refer to the 
  [Sorting](QUERIES_FILTERS_AND_SORTING.md#sorting) section for more information.

## sort_order

- Type: String
- Default: `SORT_ASC`
- Choices:
    - `SORT_ASC`
    - `SORT_DESC`
- Description: This parameter allows you to control the order in which the objects are sorted. The default value is
  `SORT_ASC` which sorts the objects in ascending order. Setting this parameter to `SORT_DESC` will sort the objects in
  descending order. The behavior of this parameter varies based on the request method and endpoint type. Refer to the
  [Sorting](QUERIES_FILTERS_AND_SORTING.md#sorting) section for more information.