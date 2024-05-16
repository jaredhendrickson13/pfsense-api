# Common Control Parameters

The API utilizes a set of common parameters to control certain behaviors of API calls. These parameters are available to
all endpoints and requests, but some endpoints may not support all parameters. Below are the available control
parameters you can use:

## async

- Type: Boolean
- Default: `true`
- Description: This parameter allows you to control the API's default behavior of apply changes in the background. Setting
  this parameter to `false` will force the API to wait for the changes to be made on the backend before responding. This
  parameter may not be supported by all endpoints.

!!! Tip
Instead of forcing the API to wait for changes to apply, it is recommended to leave this parameter set to `true`
and periodically check the status of the changes using a `GET` request to the applicable apply endpoint.
!!! Warning
Setting this parameter to `false` may cause the API to hang if the changes take a long time to apply. In severe cases,
this may result in your API request receiving a timeout. It is recommended to use this parameter with caution and
only when necessary.

## apply

- Type: Boolean
- Default: `false`
- Description: This parameter allows you to control the default API behavior of _not_ applying changes to the pfSense
  configuration immediately. Setting this parameter to `true` will force the API to apply the changes immediately. This
  is useful when you are making a small change and want to apply it immediately without needing to make a separate API
  call to the applicable apply endpoint. This may not be applicable to all endpoints as some always apply changes
  immediately.

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
