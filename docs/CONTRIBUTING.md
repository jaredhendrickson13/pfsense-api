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
It is preferred that any API endpoints or core features of this package include E2E tests at the time they are written.
This will help speed up pull requests, and assist in future regression testing. For example, if you write a new API
endpoint please include a E2E test that will thoroughly test the extent of that endpoint. Python3 is the preferred
language for E2E tests. Seeing as changes will primarily be to API endpoints, please consider Python3's `requests` 
module.

Proposing Changes
-----------------
A pull request is required for any proposed change. It is preferred that you fork the main project, make your changes,
then submit a pull request to merge your changes to the main project's current development branch. Once merged, your
changes will be made available upon the next release.

Coding Conventions
------------------
Make an attempt to match the format of existing code. The basic conventions are:
- Comments are recommended as long as they are brief, meaningful, and non-intrusive
- Variables should be defined using snake case (e.g. snake_case)
- Functions should be defined using snake case (e.g. snake_case())
- Constants should be defined using upper snake case (e.g. SNAKE_CASE)
- Classes should defined using pascal case (e.g. PascalCase)
- Lines should not contain more than 128 characters<br>
_Note: suggestions to coding conventions are welcome, refactoring as things change is necessary to support maintainable 
code_

Creating new API functionality
---------------------
Most contributions to this project will be in the form of integrating new API endpoints. API endpoints are comprised of
a few different components. It is strongly recommended that you familiarize yourself with pfSense's PHP shell before
diving into creating endpoints. To get started writing your own endpoints, please follow the steps below:

## Writing the API model ## 
At the core of the API endpoint is the API model. This is a class that validates client request data, writes changes
to the pfSense configuration file, and makes any corresponding system changes. pfSense API is distributed with a 
custom micro-framework to accommodate developers wanting to contribute or create their own API models and endpoints.

#### Getting Started ####
To get started creating a new API model, you first need to create a new PHP file in `/files/etc/inc/api/models` and
create a new class that extends our APIModel framework class:

```php
<?php
require_once("api/framework/APIModel.inc");

class NewAPIModel extends APIModel {
    
}
```

#### Constructing the API Model ####
In order to use the APIModel framework, you must add a `__construct()` method to your new API model class and 
initialize the APIModel class as such. Additionally, you may specify any model attribute overrides within this 
method:

```php
<?php
require_once("api/framework/APIModel.inc");

class NewAPIModel extends APIModel {
    # Create our construct method
    public function __construct() {
        parent::__construct();    // Initializes our APIModel class

        # Add your API model properties here (see Overriding Base Model Properties
        $this->privileges = ["page-all", "page-diagnostics-arptable"];
        $this->requires_auth = false;
    }
}
```

#### Overriding Base Model Properties ####
There are several class properties that you can customize to fit the needs of your API Model. If a model attribute is
not specified, the default values are assumed:
 
- `$this->privileges` : Allows you to set what pfSense permissions are required for clients to access this model. This
utilizes the same permissions as the pfSense webConfigurator. Specify the privileges internal config value in array
format. Defaults to `["page-all"]` which requires client to have the 'WebCfg - All Pages' permission. A list of pfSense
privileges can be found here: https://github.com/pfsense/pfsense/blob/master/src/etc/inc/priv.defs.inc

- `$this->packages` : Allows you to specify any add-on packages required for the model to operate. This must use the
full pfSense package name (including `pfSense-pkg-`). All packages must be present on the system in order for the API
model to operate. If any packages are missing, the API will return a 500 error. Defaults to `[]`.

- `$this->package_includes` : Allows you to specify additional PHP files to include for add-on packages. These files
will only attempt to be included if ALL packages specified in `$this->packages` are present. Defaults to `[]`.

- `$this->requires_auth` : Specify whether authentication and authorization is required for the API model. If set to 
`false` clients will not have to authenticate or have privilege to access. Defaults to `true`.

- `$this->set_auth_mode` : Allows you to explicitly specify the API authentication mode. For example, if you are 
writing a model that tests user's local database credentials and do not want the model to assume the API's configured
auth mode you would specify `$this->set_auth_mode = "local";` to always force local authentication. Defaults to the 
API's configured auth mode in the /api/ webConfigurator page.

- `$this->set_read_mode` : Allows the read-only API setting to be bypassed for this model. If you set this value to 
`true` the model will be allowed to use POST, PUT or DELETE methods even when the API is in read-only mode. There is 
rarely a use case for this. Do not override this property unless absolutely needed. 

- `$this->change_note` : Sets the description of the action that occurred via API. This value will be shown in the 
change logs found at Diagnostics > Backup & Restore > Config History. This defaults to "Made unknown change via API". 
This is only necessary if your API model writes changes to the configuration.


#### Other Base Model Properties ####
There are other properties inherited from APIModel that are not intended (and shouldn't be) overridden by your
custom API model:

- `$this->client` : An APIAuth object that contains information about the client (for more see Accessing Client Data)
- `$this->initial_data` : The request data as it was when the object was created
- `$this->validated_data` : An array for validators to use to populate data that has been validated
- `$this->errors` : An array to populate any errors encountered. Should be an array of APIResponse values.
- `$this->id` : A property to track the current instances configuration ID. This is primarily helpful for updating and 
deleting objects.
- `$this->validate_id` : A boolean to dictate whether the model object should require validation of the configuration ID.
This defaults to true, but can be useful for nested model object calls where you would like to validate a payload before
it's parent is created. It is entirely up to you to implement this property if desired.
- `$this->retain_read_mode` : A boolean to dictate whether this model should respect the API's read only setting if
set. If set to `false`, the model will be considered a read only model and will be allowed to answer requests when the
API is in read only mode, even if the request is not a GET request. Defaults to `true`.
- `$this->ignore_ifs` : A boolean to dictate whether or not this model should respect the allowed interfaces API
setting. If set to `true`, the model will be allowed to answer API requests regardless of the interface the request was
received on. Defaults to `false`.
- `$this->ignore_enabled` : A boolean to dictate whether or not this model should respect the API's enabled setting. If
set to true, this model will be allowed to answer API requests even if the API is disabled. Defaults to `false`.

#### Reading and Writing to pfSense's XML Configuration ####
Included in the API framework are properties and methods to read and write to pfSense's XML configuration. Please note
that other functions are likely required to configure pfSense's backend to use the new configuration. These properties
and methods are available anywhere within your API model:

- `$this->get_next_id()` : This method returns the next array ID of a specific configuration path. This is necessary
for most API creation tasks as the next ID must be specified in the `$this->set_config()` call in order to actually
write the configuration correctly. In the case that target configuration path is not an array, 0 will be returned to
initialize the array. For example, if you wanted to get the next available firewall rule ID, you could call:
`$this->get_next_id("filter/rule")` which would return the integer of the next available array index for that
configuration area.
- `$this->get_config()` : This method allows you to pull the configuration of a specific configuration area by path and
optionally allows you to return a default value if no configuration was found at this path. For example, if you wanted to 
pull all firewall rules from the configuration and default to an empty array if none were found, you could call:
`$this->get_config("filter/rule", [])`. This method is simply a wrapper for the pfSense built-in `config_get_path()`
function and works exactly the same way. For more information:
https://docs.netgate.com/pfsense/en/latest/development/php-config-arrays.html#reading-configuration-values
- `$this->set_config()` : This method allows you to set the configuration of a specific configuration area by path and
optionally allows you to specify a default if the configuration could not be set. For example, if you wanted to change
the system hostname in the configuration or default the return value to `false` if the action fails, you could call:
`$this->set_config("system/hostname", "new-hostname-value, false)`. After calling this function, the 
`$this->write_config()` method must be called to actually write the changes to the configuration file. 
This method is simply a wrapper for the pfSense built-in `config_set_path()` function and works exactly the same way. 
For more information: 
https://docs.netgate.com/pfsense/en/latest/development/php-config-arrays.html#writing-configuration-values
- `$this->del_config()` : This method deletes the configuration at a specific configuration area by path and returns
the deleted configuration. After calling this function, the `$this->write_config()` method must be called to actually 
write the changes to the configuration file. This method is simply a wrapper for the built-in pfSense `config_del_path`
function and works exactly the same way. For more information:
https://docs.netgate.com/pfsense/en/latest/development/php-config-arrays.html#deleting-configuration-values
- `$this->write_config()` : This method writes any changes made to the config to pfSense's XML configuration file. 
Any changes made will not be applied until this method is executed. This method is a wrapper for the 
pfSense built-in `write_config()` function, that also adds additional functionality like config logging, and a 
configuration lock system built specifically for the API.
- `$this->is_config_enabled()` : This method checks if a specific key exists at a specific configuration path. pfSense
identifies boolean files by whether a specific key (usually `enable`) exists or not. This method simply checks for the
existence of that key and returns `true` if it exists or `false` if it doesn't. This works similarly to PHP's built-in
`isset()` function but is safe for array traversal. For example, if you wanted to check if the SSH service is enabled
in configuration, you could call `$this->is_config_enabled("system/ssh", "enable")` to check if the `enable` exists
in the `system` > `ssh` configuration array. This method is simply a wrapper for the pfSense built-in 
`config_path_enabled()` function and works exactly the same way. For more information:
https://docs.netgate.com/pfsense/en/latest/development/php-config-arrays.html#testing-if-settings-are-enabled

#### Overriding API Model Validation ####
By default, API models do not provide any sort of validation. You are responsible for overriding the class method to 
validate the request. To validate requests, you must override the `validate_payload()` method to check specific field(s)
in the `$this->initial_data` property and either add them to our `$this->validated_data` property if they are valid, or 
appends an error response to the `$this->errors` property.

For example:
```php
<?php
require_once("api/framework/APIModel.inc");

class NewAPIModel extends APIModel {
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-arptable"];
        $this->requires_auth = false;
    }

    # Overrides our APIModel validate_payload method to validate data within our initial_data property
    public function validate_payload() {
        # Check if we have a test value in our payload, if so add the value to validated_data array
        if (array_key_exists("test", $this->initial_data)) {
            $this->validated_data["test"] = $this->initial_data["test"];
        } 
        # Otherwise, append a new error response to our error list (see Writing API responses for more info)
        else {
            $this->errors[] = APIResponse\get(1);
        }      
    }
}
```

#### Writing API Model Action ####
By default, the API model will return a 'Your API request was valid but no actions were specified for this endpoint' 
error after validating input. This is because we need to tell it what to do when our request is valid! We do this by 
overriding the APIModel's `action()` method. This method should simply be a set of tasks to perform upon a 
successful API call. If you are writing an endpoint to perform a change that is usually performed via the pfSense 
webConfigurator, it may be helpful to look at the PHP code for the webConfigurator page. This method must return an
APIResponse item.

As a basic example, if I wanted to add our validated input to our pfSense configuration:

```php
class NewAPIModel extends APIModel {
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-arptable"];
        $this->requires_auth = false;
        
        # CALL YOUR VALIDATOR METHODS HERE. This must be an array of function calls!
        $this->validators = [
            $this->validate_payload()
        ];   
    }

    public function validate_payload() {
        if (array_key_exists("test", $this->initial_data)) {
            $this->validated_data["test"] = $this->initial_data["test"];
        } 
        else {
            $this->errors[] = APIResponse\get(1);
        }      
    }
    
    # Tell our API model what to do after successfully validating the client's request
    public function action(){
        $this->id = $this->get_next_id("test/item");    // Get the next available array index for the test item
        $this->set_config("test/item/{$this->id}");    // Set a new 'test' item in the config
        $this->write_config();       // Apply the configuration change
        return APIResponse\get(0, $this->validated_data);   // Return a success response containing our added data
    }
}
```

#### Accessing Client Data ####
If for any reason you need to access client data from within your API model class, you can access the `$this->client`
property. This is an APIAuth object that contains details about the client:

- `$this->client->username` : Our client's corresponding pfSense username
- `$this->client->ip_address` : The IP address of our client
- `$this->client->is_authenticated` : Whether the client successfully authenticated. Keep in mind the APIModel will
handle all authentication and authorization for you, so this is likely unneeded.
- `$this->client->is_authorized` : Whether the client was authorized. Keep in mind the APIModel will
handle all authentication and authorization for you, so this is likely unneeded.
- `$this->client->privs` : An array of privileges this user has
- `$this->client->auth_mode` : The authentication mode that was used to authenticate the user

## Writing API endpoints ####
API endpoints are classes that map API models to various HTTP methods and also specify the URL where the endpoint 
will be available. API endpoints are dynamically generated by the frameworks `/usr/local/share/pfSense-pkg-API/manage.php` script.

#### Getting Started ####
To get started creating a new API endpoint, you first need to create a new PHP file in `/files/etc/inc/api/endpoints` 
and create a new class that extends our APIEndpoint framework class:

```php
<?php
require_once("api/framework/APIEndpoint.inc");

class NewAPIEndpoint extends APIEndpoint {
    
}
```

#### Constructing the API Endpoint ####
In order to use the APIEndpoint framework, you must add a `__construct()` method to your new API endpoint class and 
initialize the APIEndpoint class as such. Additionally, you may specify any model attribute overrides within this 
method:

```php
<?php
require_once("api/framework/APIEndpoint.inc");

class NewAPIEndpoint extends APIEndpoint {
    # Create our construct method
    public function __construct() {
        parent::__construct();

        # Add your API endpoint properties here (see Overriding Base Model Properties
        $this->url = "/api/v1/firewall/your_new_endpoint";
    }
}

```
#### Overriding Base Model Properties ####
There are class properties that you can customize to fit the needs of your API endpoint. If a model attribute is
not specified, the default values are assumed:
 
- `$this->url` : Specify the URL where this endpoint will be built. For example, `/api/v1/firewall/your_new_endpoint`.
Please note that this URL should start with a `/` but not end with one. This will recursively create any directory 
within the URL path and overwrite any existing index.php file at this location. Defaults to `null` which will throw an
error when building endpoints.
- `$this-query_excludes` : Specify parameters to exclude from queries on GET requests. This is typically only necessary
if your GET request requires parameters to locate data.

#### Overriding Base Model Methods ####
There are class methods that you will need to override to map API models to specific HTTP methods. If any of these
methods are not overridden, a "method not supported" response will be returned. Please note that all classes within
`/etc/inc/api/models/` are imported by the APIEndpoint base class so you do not need to import your API models again.
Each overridden method should return the return data of the API model's `call()` method.

```php
<?php
require_once("api/framework/APIEndpoint.inc");

class NewAPIEndpoint extends APIEndpoint {
    # Create our construct method
    public function __construct() {
        parent::__construct();

        # Add your API endpoint properties here (see Overriding Base Model Properties
        $this->url = "/api/v1/firewall/your_new_endpoint";
    }

    # Override the get() method to map the model that should be used upon a GET request
    protected function get() {
        return (new NewAPIModelRead())->call();
    }

    # Override the post() method to map the model that should be used upon a POST request
    protected function post() {
        return (new NewAPIModelCreate())->call();
    }
    
    # Override the put() method to map the model that should be used upon a PUT request
    protected function put() {
        return (new NewAPIModelUpdate())->call();
    }
    
    # Override the delete() method to map the model that should be used upon a DELETE request
    protected function delete() {
        return (new NewAPIModelDelete())->call();
    }
}
```
#### Building the API endpoint ####
API endpoint classes are merely blueprints to build the actual endpoint within pfSense's web root (`/usr/local/www/`).
These endpoints are automatically built when the package is installed. If you need to manually build endpoints to test,
you may run `php -f /usr/local/share/pfSense-pkg-API/manage.php buildendpoints`. This will output a list of endpoint classes that
were built and the file path they were built at. If there was a problem building an endpoint, an error message will 
be returned and the script will exit on a non-zero return code.

## Writing API responses ##
The API uses a centralized API response array (found in `/files/etc/inc/api/framework/APIResponse.inc` of this repo). 
Each response corresponds with a unique ID that can be used to get the API response message, status, etc. This is 
particularly helpful when API response messages need to be changed as it is always in one central location. To add a 
new API response, you may add a new array item to the `$responses` variable within the `get()` function of 
`/files/etc/inc/api/framework/APIResponse.inc`. Each response within the array should be formatted as an associative
array with the `status`, `code`, `return`, and `message` keys. 

For example, if I needed to write a new error response for API endpoint I could add:

```
620 => [
    "status" => "bad request",     # Use this field to describe the HTTP response (not found, bad request, ok, etc.)
    "code" => 400,                 # Use this field to set the HTTP response code that will be returned to the client
    "return" => $id,               # This should always return the API response ID, in this case 620.
    "message" => "Error found!"    # Set a descriptive response message
]
```

After this response item is added to to the `$responses` variable within the `get()` function of 
`/files/etc/inc/api/framework/APIResponse.inc`, you can get the response within your API model like this:

`$this->errors[] = APIResponse\get(620);`

Optionally, you can add data to the response as a parameter of the `get()` function like this:

`$this->errors[] = APIResponse\get(620, $some_data);`

## Writing tool functions ##
Often times you will need to create functions to condense redundant tasks. You can place any necessary tool functions in
`/files/etc/inc/api/framework/APITools.inc`. You may then access the tool function from your API model like this:

`$some_variable = APITools\your_custom_tool_function();`

As a general rule, functions should be kept within the API model they relate closest to. For example, a function that
checks for the existence of an alias by name should be kept in the APIFirewallAlias* models, even if multiple models
will use the function. Tool functions should only be used in one of the following situations:
- Multiple models, endpoints or tools use the function and the function does not directly relate to any of the existing
API models, or it directly relates to multiple models.
- Use of the function causes an `include` or `require` loop.


## Writing API E2E Tests ##
E2E tests are written using Python3. pfSense API includes a an e2e_test_framework module in the `tests` directory to make
this process simple. E2E tests help to ensure each endpoint remains functional when changes have been made.

#### Getting Started ####
To get started creating a new API E2E test, you first need to create a new Python3 file in `/tests`. This file must 
start with `test` and end with `.py` to be included in the `run_all_tests.py` script. Once you have created this file, 
you must create a new class that extends our APIE2ETest framework class:

```python
import e2e_test_framework

class NewAPIE2ETest(e2e_test_framework.APIE2ETest):
    # ...
```

#### Overriding Base Model Properties ####
The APIE2ETest class requires you to override a some properties to function correctly:

- `url` : A string specifying the URL this E2E test will be testing
- `time_delay` : An integer specifying how many seconds should be waited between requests. Defaults to `1`.
- `get_tests` : A list of dictionary formatted test parameters for GET requests. If this endpoint does not support 
GET requests, you do not need to override this property. If this endpoint does support GET request, but does not require
any payload data to receive a valid response you must set this value to `[{}]`. Each dictionary can contain:
    - `name` : set a descriptive name to be printed alongside test results (defaults to `unnamed test`)
    - `req_data` : a nested dictionary that contains the request payload to use when running the test (defaults to `{}`)
    - `resp_data` : a nested dictionary that contains the expected API response data.
    - `resp_data_empty` : a boolean indicating whether this test should allow the response data to be empty.    - `return` : an integer that specifies the tests expected API return code (defaults to `0`)
    - `resp_time` : a float that specifies the tests maximum response time expected from the API endpoint
    - `delay` : an integer that specifies how many seconds a test should wait until starting
    - `pause` : an integer that specifies how many seconds a test should wait after the test has finished    - `username` : the client's username or client-id to authenticate with. Defaults to `username` argument value.
    - `password` : the client's password or client-token to authenticate with. Defaults to `password` argument value.
    - `auth_mode` : hard set the authentication mode for this test. Defaults to `auth_mode` argument value.
    - `pre_test_callable` : the name of a function callable to run before this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `post_test_callable` : the name of a function callable to run after this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `req_data_callable` : the name of a function callable to run to dynamically generate the request payload. This
      must be a function that returns a dictionary of request payload items. This can be used to apply request payload
      items that may by dynamic such as IDs. If this test also has a `req_data` specified, this callable will simply 
      be merged into the test's `req_data`. 
    
- `post_tests` : A list of dictionary formatted test parameters for POST requests. If this endpoint does not support 
POST requests, you do not need to override this property. If this endpoint does support POST request, but does not require
any payload data to receive a valid response you must set this value to `[{}]`. Each dictionary can contain:
    - `name` : set a descriptive name to be printed alongside test results (defaults to `unnamed test`)
    - `req_data` : a nested dictionary that contains the request payload to use when running the test (defaults to `{}`)
    - `resp_data` : a nested dictionary that contains the expected API response data.
    - `resp_data_empty` : a boolean indicating whether this test should allow the response data to be empty.    - `return` : an integer that specifies the tests expected API return code (defaults to `0`)
    - `resp_time` : a float that specifies the tests maximum response time expected from the API endpoint
    - `delay` : an integer that specifies how many seconds a test should wait until starting
    - `pause` : an integer that specifies how many seconds a test should wait after the test has finished    - `username` : the client's username or client-id to authenticate with. Defaults to `username` argument value.
    - `password` : the client's password or client-token to authenticate with. Defaults to `password` argument value.
    - `auth_mode` : hard set the authentication mode for this test. Defaults to `auth_mode` argument value.
    - `pre_test_callable` : the name of a function callable to run before this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `post_test_callable` : the name of a function callable to run after this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `req_data_callable` : the name of a function callable to run to dynamically generate the request payload. This
      must be a function that returns a dictionary of request payload items. This can be used to apply request payload
      items that may by dynamic such as IDs. If this test also has a `req_data` specified, this callable will simply 
      be merged into the test's `req_data`. 
  
- `put_tests` : A list of dictionary formatted test parameters for PUT requests. If this endpoint does not support 
PUT requests, you do not need to override this property. If this endpoint does support PUT request, but does not require
any payload data to receive a valid response you must set this value to `[{}]`. Each dictionary can contain:
    - `name` : set a descriptive name to be printed alongside test results (defaults to `unnamed test`)
    - `req_data` : a nested dictionary that contains the request payload to use when running the test (defaults to `{}`)
    - `resp_data` : a nested dictionary that contains the expected API response data.
    - `resp_data_empty` : a boolean indicating whether this test should allow the response data to be empty.    - `return` : an integer that specifies the tests expected API return code (defaults to `0`)
    - `resp_time` : a float that specifies the tests maximum response time expected from the API endpoint
    - `delay` : an integer that specifies how many seconds a test should wait until starting
    - `pause` : an integer that specifies how many seconds a test should wait after the test has finished    - `username` : the client's username or client-id to authenticate with. Defaults to `username` argument value.
    - `password` : the client's password or client-token to authenticate with. Defaults to `password` argument value.
    - `auth_mode` : hard set the authentication mode for this test. Defaults to `auth_mode` argument value.
    - `pre_test_callable` : the name of a function callable to run before this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `post_test_callable` : the name of a function callable to run after this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `req_data_callable` : the name of a function callable to run to dynamically generate the request payload. This
      must be a function that returns a dictionary of request payload items. This can be used to apply request payload
      items that may by dynamic such as IDs. If this test also has a `req_data` specified, this callable will simply 
      be merged into the test's `req_data`. 

- `delete_tests` : A list of dictionary formatted test parameters for DELETE requests. If this endpoint does not support 
DELETE requests, you do not need to override this property. If this endpoint does support DELETE request, but does not require
any payload data to receive a valid response you must set this value to `[{}]`. Each dictionary can contain:
    - `name` : set a descriptive name to be printed alongside test results (defaults to `unnamed test`)
    - `req_data` : a nested dictionary that contains the request payload to use when running the test (defaults to `{}`)
    - `resp_data` : a nested dictionary that contains the expected API response data.
    - `resp_data_empty` : a boolean indicating whether this test should allow the response data to be empty.
    - `status` : an integer that specifies the tests expected HTTPS status code (defaults to `200`) 
    - `return` : an integer that specifies the tests expected API return code (defaults to `0`)
    - `resp_time` : a float that specifies the tests maximum response time expected from the API endpoint
    - `delay` : an integer that specifies how many seconds a test should wait until starting
    - `pause` : an integer that specifies how many seconds a test should wait after the test has finished
    - `username` : the client's username or client-id to authenticate with. Defaults to `username` argument value.
    - `password` : the client's password or client-token to authenticate with. Defaults to `password` argument value.
    - `auth_mode` : hard set the authentication mode for this test. Defaults to `auth_mode` argument value.
    - `pre_test_callable` : the name of a function callable to run before this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `post_test_callable` : the name of a function callable to run after this test is run. This callable can initiate
      a test failure by raising any exception during the function call.
    - `req_data_callable` : the name of a function callable to run to dynamically generate the request payload. This
      must be a function that returns a dictionary of request payload items. This can be used to apply request payload
      items that may by dynamic such as IDs. If this test also has a `req_data` specified, this callable will simply 
      be merged into the test's `req_data`. 

- `get_responses` : A list of previously executed GET requests in a dictionary format. Failing responses will not be 
included.

- `post_responses` : A list of previously executed POST requests in a dictionary format. Failing responses will not be 
included.

- `put_responses` : A list of previously executed PUT requests in a dictionary format. Failing responses will not be 
included.

- `delete_responses` : A list of previously executed DELETE requests in a dictionary format. Failing responses will not be 
included.

- `get_privileges` : A list of pfSense privileges required to authorize GET API calls to this test's endpoint. Any 
privilege specified here will automatically be tested by the E2E test framework. 

- `post_privileges` : A list of pfSense privileges required to authorize POST API calls to this test's endpoint. Any
privilege specified here will automatically be tested by the E2E test framework.

- `put_privileges` : A list of pfSense privileges required to authorize PUT API calls to this test's endpoint. Any
privilege specified here will automatically be tested by the E2E test framework.

- `delete_privileges` : A list of pfSense privileges required to authorize DELETE API calls to this test's endpoint. Any
privilege specified here will automatically be tested by the E2E test framework.

#### Other Base Model Properties
The APIE2ETest class also contains a few properties that are not intended to be overridden:

- `uid` : a unique ID that can be used for req_data fields that require a unique value

#### Overriding Base Model Methods ####
There are methods that will assist you when you need to dynamically format API request data. These are typically used 
when you need to add request data that is dependent on a previous API response. The following methods may 
overridden:

- `pre_get()` : Runs before the GET request is made.
- `post_get()` : Runs after the GET request is made.
- `pre_post()` : Runs before the POST request is made.
- `post_post()` : Runs after the POST request is made.
- `pre_put()` : Runs before the PUT request is made.
- `post_put()` : Runs after the PUT request is made.
- `pre_delete()` : Runs before the DELETE request is made.
- `post_delete()` : Runs after the DELETE request is made.

#### Other Base Model Models
There are a few methods available to tests through the testing framework, these methods are intended for use in 
conjunction with the `pre_test_callable`, `post_test_callable`, and `payload_callable` test parameters:

- `pfsense_shell()` : Runs a shell command on the targeted pfSense instance using the /api/v1/diagnostics/command_prompt
endpoint. This function will return stdout and/or stderr of the executed command. This can be used to verify certain
conditions, files, or configurations exist on the pfSense backend via CLI.
  - Example usage: `self.pfsense_shell("ifconfig")`

#### Best Practices
When writing E2E tests, it is best to follow these guidelines to prevent unexpected test failures:

- Any configuration added by a test should be fully removed by the end of the test, this will prevent issues with
other tests that expect the configuration to be a blank slate. Each test should be able to run repeatedly without 
failure, both individually and using `run_all_tests.py`.
- For tests that utilize `pre_test_callable` or `post_test_callable` to verify the changes made are expected, it is 
strongly recommended you make use of constants for any values that are being set and evaluated. This ensures the 
value only needs to be updated in one place to update the context of the test, or change test values.
- When possible, you may want to consider integrating randomized values where applicable. For example, if a field
accepts a numeric value that must be within a set range, you may want tests to use a random value for this field to
better simulate variance of client options. This both ensures validation constraints work, as well as point out 
problematic values that would otherwise be missed by a static field value.
- As much as possible, tests should not bypass any functionality available to an API endpoint. One exception to this
rule is if the API endpoint inherently severs access to the target pfSense instance (e.g. system reboot). In these 
situations, you can add `_action_bypass` to the test's `req_data` to bypass the `action()` method of the API model.

#### Running E2E Tests ####
Once you have written your E2E test class, you must ensure you create the E2E test object at the end of the file
you've created like so:

```python
import e2e_test_framework

class NewAPIE2ETest(e2e_test_framework.APIE2ETest):
    url = "/api/v1/your_endpoint"
    
    get_privileges = ["page-all", "page-some-other-priv"]
    post_privileges = ["page-all", "page-some-other-priv"]
    put_privileges = ["page-all", "page-some-other-priv"]
    delete_privileges = ["page-all", "page-some-other-priv"]

    get_requests = [{}]
    post_requests = [
        {   
            "req_data": {
                "some_parameter": "some value to create"
            },
            "status": 400,
            "return": 1,
            "resp_time": 0.5
        },
        {   
            "req_data": {
                "some_other parameter": "some other value to create"
            },
            "status": 200,
            "return": 0,
            "resp_time": 0.6
        },    ]
    put_requests = [
        {   
            "req_data": {
                "some_parameter": "some value to update"
            },
            "status": 400,
            "return": 1,
            "resp_time": 0.5
        },
        {   
            "req_data": {
                "some_other parameter": "some other value to update"
            },
            "status": 200,
            "return": 0,
            "resp_time": 0.6
        },  
    ]  
    delete_requests = [
        {   
            "req_data": {
                "some_parameter": "some value to delete"
            },
            "status": 200,
            "return": 0,
            "resp_time": 0.5
        },
    ]     

NewAPIE2ETest()
```

Once this is done, you can run the E2E test via command line by running:<br>
`python3 test/test_your_e2e_test_framework.py --host <ENTER PFSENSE IP/HOSTNAME HERE>`

Or you may run all the E2E tests by running:<br>
`python3 test/run_all_tests.py`

E2E tests will check API responses for the following:
- Ability to connect to API endpoint
- API responses properly return data in a JSON format
- API responses include the correct HTTP status code
- API responses include the expected API return code
- API responses are received within an acceptable time frame
- API response data is expected
- CRUD success. POST requests are always run first, then GET requests to check that the creation was successful, then
PUT requests attempt to update the created object, then finally DELETE requests attempt to destroy the object.

## Making the pfSense API package ##
The package Makefile and pkg-plist files are auto generated by `tools/make_package.py`. This simply pulls the files and
directories needed to build our package from the `files` directory. For more information on building the package, refer
to `tools/README.md`

## Creating or Updating Documentation ##
Documentation is written using Swagger/OpenAPIv3. The OpenAPI configuration can be found at 
`pfSense-pkg-API/files/usr/local/www/api/documentation/openapi.yml`. Whenever new functionality is added or updated, 
the OpenAPI configuration must also be updated to reflect the changes. The OpenAPI configuration 
must be in OpenAPIv3 spec at all times. 
 
Questions
---------
There are some complex components to this project. Please feel free to reach out with any questions, 
issues, or insight you have when creating API endpoints. Time permitting I am happy to help any way I can. 
