# Content and Accept Types

The REST API has been designed to support multiple content and accept types to allow for flexibility in the data being sent and received. This document outlines the content and accept types supported by the REST API.

## Content Types

Content types are used to specify the format of the data being sent in your request. You must specify the content type in the `Content-Type` header of your request. The REST API supports the following content types:

### application/json
- MIME Type: `application/json`
- Description: Use this content type to send JSON data in the body of your request. The data should be formatted as a JSON object or array.
- Example: ```{"key": "value"}```

### application/x-www-form-urlencoded
- MIME Type: `application/x-www-form-urlencoded`
- Description: Use this content type to send form data in the URL of the request. The data is sent as key-value pairs separated by an ampersand `&`.
- Example: ```https://pfsense.example.com?key=value&key2=value2```

!!! Warning
    The `application/x-www-form-urlencoded` content-type is not fully suitable for requests other than `GET` and `DELETE` requests. It is recommended to use `application/json` for all other request types.

## Accept Types

Accept types are used to specify the format of the data you would like to receive in the response. You can specify the accept type in the `Accept` header of your request, or the response will default to `application/json`. The REST API supports the following accept types:

### application/json
- MIME Type: `application/json`
- Description: Use this accept type to receive JSON data in the response. The data will be formatted as a JSON object or array.
- Example: ```{"key": "value"}```