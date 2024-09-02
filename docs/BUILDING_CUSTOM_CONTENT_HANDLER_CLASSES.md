# Building Custom Content Handler Classes

ContentHandler classes are responsible for decoding (parsing) request content received from API clients and encoding 
(formatting) response content returned to API clients based on the MIME type specified in the `Content-Type` and `Accept` 
headers respectively. ContentHandlers follow this process:

1. An Endpoint receives an API request, the REST API package will attempt to find a ContentHandler that supports the MIME type provided in the `Content-Type` header.
2. The ContentHandler will decode the request content into a PHP array which will be passed to the appropriate Model class.
3. The Endpoint will call the associated Model class to process the request and convert the resulting Model object into it's representation form as a PHP array.
4. The Endpoint will attempt to find a ContentHandler that supports the MIME type provided in the `Accept` header.
4. The ContentHandler will encode the PHP array into the MIME type specified in the client's `Accept` header and return the response content to the API client.

This document will guide you through the process of building a custom ContentHandler class as well
as options for interacting with the ContentHandler:

## Getting Started

Use the following class template to initialize your custom ContentHandler class:

```php
<?php

namespace RESTAPI\ContentHandlers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\ContentHandler;

/**
 * TODO: Add a description of your ContentHandler class here.
 */
class MyCustomContentHandler extends ContentHandler {

}
```

!!! Important
    Be sure to place this class file within `/usr/local/pkg/RESTAPI/ContentHandlers/` directory and name the file after your
    ContentHandler class name with a `.inc` extension.

## Defining ContentHandler Properties

Below are the properties that you can define in your ContentHandler class to customize the behavior of the ContentHandler:

### mime_type

The `mime_type` property is used to define the MIME type that the ContentHandler supports. This property is required and
must be set to a valid MIME type string.

!!! Important
    The `mime_type` property must be unique from all other ContentHandler classes. If two ContentHandler classes have the same
    `mime_type` property, unexpected behavior may occur.

## Implementing the _decode() Method

The `_decode()` method is used to define how the ContentHandler decodes the raw request content returned by 
`get_content()` into a PHP array. If this method is not implemented, the ContentHandler will throw a 406 Not Acceptable error
a client submits a Content-Type header with this ContentHandler's MIME type:

```php
/**
 * Decode the raw request content into a PHP array.
 * @param string $content The raw request content.
 * @return array The decoded request content.
 */
protected function _decode(mixed $content): mixed {
    # TODO: Add your decoding logic here.
}
```

## Implementing the _encode() Method

The `_encode()` method is used to define how the ContentHandler encodes the PHP array response data into the MIME type 
associated with this ContentHandler. If this method is not implemented, the ContentHandler will throw a 406 Not Acceptable error
a client submits an Accept header with this ContentHandler's MIME type:

```php
/**
 * Encode the PHP array response data into the MIME type associated with this ContentHandler.
 * @param array $data The PHP array response data.
 * @return string The encoded response data.
 */
protected function _encode(mixed $data): mixed {
    # TODO: Add your encoding logic here.
}
```

!!! Important
    You do not need to worry about setting the HTTP response headers in the ContentHandler. The REST API package will handle
    setting the appropriate headers based on the ContentHandler's MIME type automatically.

## Examples

You can find examples of fully implemented ContentHandler classes in the [PHP reference](https://pfrest.org/php-docs/namespaces/restapi-contenthandlers.html).
Select the ContentHandler class you are interested in to view the class's PHPDoc documentation, and then click on the
`<>` symbol next to the class name to view the class's source code.