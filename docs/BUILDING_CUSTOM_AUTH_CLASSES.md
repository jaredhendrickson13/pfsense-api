# Building Custom Authentication Classes

!!! Danger
    This feature is intended for advanced use cases only. Incorrectly implemented custom authentication classes can lead to
    significant security vulnerabilities.

For advanced users, the REST API's framework allows for custom authentication methods to be added using PHP. To add a custom
authentication method, extend the `\RESTAPI\Core\Auth` class and implement the `_authenticate` method. This method simply
needs to return a boolean value indicating if the user is authenticated or not. Below is an example of a custom Auth class:

```php
<?php

namespace RESTAPI\Auth;

require_once "RESTAPI/autoloader.inc";

use RESTAPI\Core\Auth;

class MyCustomAuth extends Auth
{
  # Defines a human-readable name for your custom auth method
  public ?string $verbose_name = "My Custom Auth";

  # Tells OpenAPI how to use your custom auth method in the Swagger documentation. In this example, we are telling
  # OpenAPI to check for an authentication secret in the HTTP header named 'custom-header-name'
  public array $security_scheme = [
    "type" => "apiKey",
    "in" => "header",
    "name" => "custom-header-name",
  ];

  public function _authenticate(): bool
  {
    # Add your custom authentication logic here
    if ($my_custom_auth_logic) {
      # Assign the username of the authenticated user to the $this->username property
      $this->username = "my_identified_user";
      
      return true;
    }

    # Return false if authentication fails
    return false;
  }
}
```

!!! Important
    Your `_authenticate` method must assign the `username` property of the `Auth` class to the authenticated user's username.
    The user must be an existing local user on the pfSense system. Otherwise, authentication or authorization will fail.

Once your custom Auth class is created, simply place it at `/usr/local/pkg/RESTAPI/Auth/MyCustomAuth.inc` and it will
automatically become an available authentication method in the REST API settings.

!!! Tip
    You can rename `MyCustomAuth` to any name you like, but make sure the class name matches the file name.

## Examples

You can find examples of fully implemented Auth classes in the [PHP reference](https://pfrest.org/php-docs/namespaces/restapi-auth.html).
Select the Auth class you are interested in to view the class's PHPDoc documentation, and then click on the
`<>` symbol next to the class name to view the class's source code.