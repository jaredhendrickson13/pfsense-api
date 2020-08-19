How to Contribute
=================
Thank you for your interest in contributing to this project! Community support is essential to keep this package secure,
efficient, and useful. Your contributions are very much appreciated. To ensure this project can achieve and retain a 
production ready package, please follow the guidelines detailed in this document.

Requirements
------------
If you plan on contributing, please follow these basic requirements
- Do not introduce known vulnerabilities. Please do your due diligence before integrating anything that may be insecure.
- Be respectful. Open source contributions are a learning experience for all developers regardless of skill level. 
 Please be kind and helpful towards other contributors.
- Write ethical code. Please do not steal code and claim it as your own. 

Testing
-------
It is preferred that any API endpoints or core features of this package include unit tests at the time they are written.
This will help speed up pull requests, and assist in future regression testing. For example, if you write a new API
endpoint please include a unit test that will thoroughly test the extent of that endpoint. Python3 is the preferred
language for unit tests. Seeing as changes will primarily be to API endpoints, please consider Python3's `requests` 
module.

Proposing Changes
-----------------
A pull request is required for any proposed change. It is preferred that you fork the main project, make your changes,
then submit a pull request to merge your changes to the main project's current development branch. Once merged, your
changes will be made available upon the next release.

Coding Conventions
------------------
Make an attempt to match the format of existing code. The basic conventions are:
- Comments are recommended as long as they are brief, meaningful, and short
- Variables should be defined using snake case (e.g. snake_case)
- Functions should be defined using snake case (e.g. snake_case())
- Constants should be defined using upper snake case (e.g. SNAKE_CASE)
- Classes should defined using pascal case (e.g. PascalCase)
- Lines should not contain more than 128 characters<br>
_Note: suggestions to coding conventions are welcome, refactoring as things change is necessary to support maintainable 
code_

Writing new API Endpoints
---------------------
Most contributions to this project will be in the form of integrating new API endpoints. API endpoints are comprised of
a few different components. It is strongly recommended that you familiarize yourself with pfSense's PHP shell before
diving into creating endpoints. To get started writing your own endpoints, please follow the steps below:

### Things to Know ###
 - The API is based on REST principals. Unfortunately, pfSense does not allow any custom changes to the NGINX 
 configuration so alternate request methods like `PUT` and `DELETE` do not appear to be possible. To accommodate this, 
 the requested action must be defined in the endpoint path. 
    - Create actions must be a `POST` request to an endpoint ending in `/add/` 
     (e.g. `https://localhost/api/v1/firewall/rules/add/`) 
    - Read actions must be a `GET` request to a base endpoint (e.g. `https://localhost/api/v1/firewall/rules/`)
    - Update actions must be a `POST` request to an endpoint ending in `/update/` 
     (e.g. `https://localhost/api/v1/firewall/rules/update/`)
    - Delete actions must be a `POST` request to an endpoint ending in `/delete/`
     (e.g. `https://localhost/api/v1/firewall/rules/delete/`)

### Writing the API caller ### 
At the core of the API endpoint is the API caller. This is a function that validates client request data, writes changes
to the pfSense configuration file, makes any corresponding system changes, and returns the requested data as an array.

1. Most API endpoints are designed to allow programmatic changes to configuration that is usually made in the pfSense
webConfigurator. To get a basic understanding on what your API call will need to do, look at the PHP code of the 
corresponding webConfigurator page (found within `/usr/local/www/` of your pfSense installation). You should be able to 
get a good idea of what input validation is needed, and what core pfSense functions you will need to call to achieve 
these changes. You can also use existing API call functions (found in `/files/etc/inc/apicalls.inc` within this repo) as
a reference!

2. Write your function in `/files/etc/inc/apicalls.inc`. The function name should match the URL filepath without the 
version. For example, for an API endpoint at URL `/api/v1/firewall/rules/delete/`, the function would be named
`api_firewall_rules_delete`. Please also place this function next to any related functions in `apicall.inc`. For example
if you write a new API call function for `api_firewall_rules_update`, place it next to any existing functions for the
API firewall calls.

3. Ensure API callers always return the data of the action that was performed. For example, if you are writing an 
`update` endpoint, ensure the API callers returns the updated data. Or if you are writing a `delete` endpoint, ensure
the API caller always returns the deleted data. 

4. Ensure any validation or API errors encountered when the API caller function is run returns a corresponding error
from the `/files/etc/inc/apiresp.inc` file. You can read more about writing API error responses below.

Here is an example structure of an API caller function:
```
function api_caller_function() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-some-other-webconfigurator-permission");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Get our HTTP method

    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // ADD YOUR INPUT VALIDATION, CONFIG WRITES, AND SYSTEM CONFIGURATION
            // Return our response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => []);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;    // Returns default unauthorized response
    }
}
```

### Writing API responses ###
The API uses a centralized API response array (found in `/file/etc/inc/apiresp.inc` of this repo). Each response
corresponds with a unique code that can be used to get the response message, status, etc. This is particularly helpful
when API response messages need to be changed as it is always in one central location. 

To create a new API response:

1. Pick a numeric code that is not already in use in the `$err_lib` array within the `api_error_lib()` function of 
`/files/etc/inc/apiresp.inc`. Try to use a code that is within the reserved range of the API endpoint you are creating.
2. Add a new array item to the `$err_lib` array within the `api_error_lib()`. The associated ID should be the numeric
code you picked, and the value should be the descriptive message to return from the API. Some examples are:
```
$err_lib = array(
     // 0-999 reserved for system API level responses
     0 => "Success",
     1 => "Process encountered unexpected error",
     2 => "Invalid HTTP method",
     3 => "Authentication failed"
)
```
3. To get the response message, ensure your API caller function has `$err_lib` declared globally 
(e.g. `global $err_lib;`), then you can pull the message corresponding with your code as such `$err_lib[<CODE>]`. Each
API caller should return a response error similar to: 
```
array(
    "status" => "unauthorized",    # Sets the descriptive HTTP response message (unauthorized, not found, ok, etc.)
    "code" => 401,    # Sets the HTTP response code. This will be the response code PHP returns to the client.
    "return" => 3,    # Set the unique API response code from `apiresp.inc`
    "message" => $err_lib[3],    # Pull the message corresponding with this unique response code from `apiresp.inc`
    "data" => []    # Set the data to return to the client in an array format
);`
```

### Writing API endpoint listeners ###
Each API caller must have an API endpoint listener within the web path to listen for requests and execute the API caller 
function. These can be found in `/files/usr/local/www/api/v1/`. To create a new endpoint:

1. Create a new directory in `/files/usr/local/www/api/v1/` that corresponds with the endpoint you are creating. For 
example, if you are creating a new endpoint that deletes a firewall rule, you would create the directory 
`/files/usr/local/www/api/v1/firewall/rules/delete/`.
2. Create an index.php file within that directory and add the following code:
```
<?php
# IMPORTS
require_once("apicalls.inc");

# RUN API CALL
$resp = your_api_caller_function();    # <--- BE SURE TO CHANGE THIS TO YOUR API CALL FUNCTION
http_response_code($resp["code"]);
echo json_encode($resp) . PHP_EOL;
exit();
```
This expects the API caller function to return a associative array. This will then set the HTTP response code based on 
the "code" value of your functions returned array. The returned array with then be serialized to JSON and returned to 
the client.

### Writing tool functions ###
Often times you will need to create functions to condense redundant tasks. You can place any necessary tool functions in
`/files/etc/inc/api.inc`. 

### Adding endpoint to the package
After you have written your API endpoint and have tested it's functionality, you must specify your endpoint files in
the package makefile. Otherwise, it will not be included in the package in the next release. 

1. Add the following lines to the `Makefile` located in this repo. **Be sure to change the file paths to match the files
you have created**:
```
${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/status/carp/modify
${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/status/carp/modify/index.php \
    ${STAGEDIR}${PREFIX}/www/api/v1/status/carp/modify
```
2. Add the following lines to the `pkg-plist` file located in this repo. Be sure to change the file paths to match the
files you have created:
    - For each directory created, add: `@dir /usr/local/www/api/v1/status/carp/modify`
    - For each index.php file created, add `/usr/local/www/api/v1/status/carp/modify/index.php`
    
Questions
---------
There are some complex components to this project. Please feel free to reach out with any questions, 
issues, or insight you have when creating API endpoints. Time permitting I am happy to help any way I can. 