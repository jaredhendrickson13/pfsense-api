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
- `$this->config` : Our pfSense configuration array. You may read current configuration values using this array or write
changes to the configuration by updating it's values. If you do make changes to the configuration, you must use call
`$this->write_config()` to apply them. 
- `$this->id` : A property to track the current instances configuration ID. This is primarily helpful for updating and 
deleting objects.
- `$this->validate_id` : A boolean to dictate whether the model object should require validation of the configuraiton ID.
This defaults to true, but can be useful for nested model object calls where you would like to validate a payload before
it's parent is created. It is entirely up to you to implement this property if desired.

#### Reading and Writing to pfSense's XML Configuration ####
Included in the API framework are properties and methods to read and write to pfSense's XML configuration. Please note
that other functions are likely required to configure pfSense's backend to use the new configuration. These properties
and methods are available anywhere within your API model:

- `$this->config` : Our pfSense XML configuration in a PHP array format. You may read the current configuration from 
this property or update/add new configuration by assigning the corresponding array key new values
- `$this->write_config()` : This method writes any changes made to $this->config to pfSense's XML configuration file. 
Any changes made to $this->config will not be applied until this method is executed. 

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
        $this->config["test"][] = $this->validated_data;          // Write a new 'test' item to our master config
        $this->write_config();       // Apply our configuration change
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


## Writing API Unit Tests ##
Unit tests are written using Python3. pfSense API includes a an unit_test_framework module in the `tests` directory to make
this process simple. Unit tests help to ensure each endpoint remains functional when changes have been made.

#### Getting Started ####
To get started creating a new API unit test, you first need to create a new Python3 file in `/tests`. This file must 
start with `test` and end with `.py` to be included in the `run_all_tests.py` script. Once you have created this file, 
you must create a new class that extends our APIUnitTest framework class:

```python
import unit_test_framework

class NewAPIUnitTest(unit_test_framework.APIUnitTest):
    # ...
```

#### Overriding Base Model Properties ####
The APIUnitTest class requires you to override a some properties to function correctly:

- `url` : A string specifying the URL this unit test will be testing<br>
- `get_payloads` : A list of dictionary formatted API payloads to use when testing GET requests. If this endpoint does not
support GET requests, you do not need to override this property. If this endpoint does support GET request, but does not
require any payload data to receive a valid response you must set this value to `[{}]`
- `post_payloads` : A list of dictionary formatted API payloads to use when testing POST requests. If this endpoint does 
not support POST requests, you do not need to override this property. 
- `put_payloads` : A list of dictionary formatted API payloads to use when testing PUT requests. If this endpoint does 
not support PUT requests, you do not need to override this property. 
- `delete_payloads` : A list of dictionary formatted API payloads to use when testing DELETE requests. If this endpoint does 
not support DELETE requests, you do not need to override this property. 
- `get_responses` : A list of previously executed GET requests in a dictionary format. Failing responses will not be 
included.
- `post_responses` : A list of previously executed POST requests in a dictionary format. Failing responses will not be 
included.
- `put_responses` : A list of previously executed PUT requests in a dictionary format. Failing responses will not be 
included.
- `delete_responses` : A list of previously executed DELETE requests in a dictionary format. Failing responses will not be 
included.

```python
import unit_test_framework

class NewAPIUnitTest(unit_test_framework.APIUnitTest):
    url = "/api/v1/your_endpoint"
    get_payloads = [{}]
    post_payloads = [
        {"some_parameter": "some value to create"},
        {"some_parameter": "some other value to create"}
    ]
    put_payloads = [
        {"some_parameter": "some value to update"},
        {"some_parameter": "some other value to update"}
    ]  
    delete_payloads = [
        {"some_parameter": "some value to delete"},
    ]    
```

#### Overriding Base Model Methods ####
There are methods that will assist you when you need to dynamically format API request data. These are typically used 
when you need to add payload data that is dependent on a previous API response. The following methods may 
overridden:

- `pre_get()` : Runs before the GET request is made.
- `post_get()` : Runs after the GET request is made.
- `pre_post()` : Runs before the POST request is made.
- `post_post()` : Runs after the POST request is made.
- `pre_put()` : Runs before the PUT request is made.
- `post_put()` : Runs after the PUT request is made.
- `pre_delete()` : Runs before the DELETE request is made.
- `post_delete()` : Runs after the DELETE request is made.

#### Running Unit Tests ####
Once you have written your unit test class, you must ensure you create the unit test object at the end of the file
you've created like so:

```python
import unit_test_framework

class NewAPIUnitTest(unit_test_framework.APIUnitTest):
    url = "/api/v1/your_endpoint"
    get_payloads = [{}]
    post_payloads = [
        {"some_parameter": "some value to create"},
        {"some_parameter": "some other value to create"}
    ]
    put_payloads = [
        {"some_parameter": "some value to update"},
        {"some_parameter": "some other value to update"}
    ]  
    delete_payloads = [
        {"some_parameter": "some value to delete"},
    ]    

NewAPIUnitTest()
```

Once this is done, you can run the unit test via command line by running:<br>
`python3 test/test_your_unit_test_framework.py --host <ENTER PFSENSE IP/HOSTNAME HERE>`

Or you may run all the unit tests by running:<br>
`python3 test/run_all_tests.py`

Unit tests will check API responses for the following:
- Ability to connect to API endpoint
- API responses properly return data in a JSON format
- API payloads return a 200 OK response
- CRUD success. POST requests are always run first, then GET requests to check that the creation was successful, then
PUT requests attempt to update the created object, then finally DELETE requests attempt to destroy the object. 

## Making the pfSense API package ##
The package Makefile and pkg-plist files are auto generated by `tools/make_package.py`. This simply pulls the files and
directories needed to build our package from the `files` directory. For more information on building the package, refer
to `tools/README.md`

## Creating or Updating Documentation ##
Documentation is written and maintained using Postman. The JSON export of the Postman collection can be found in 
`docs/documentation.json`. Both the README.md and embedded documentation are generated using this JSON file. To update
or add documentation, you can either import this collection to Postman, make your changes in Postman and then export the
updated collection as JSON, or you may edit the JSON file directly if you are familiar with the structure. To generate
the README.md and embedded documentation pages, you may use the `tools/make_documentation.py` script. For more info on
running the script, refer to `tools/README.md`
 
Questions
---------
There are some complex components to this project. Please feel free to reach out with any questions, 
issues, or insight you have when creating API endpoints. Time permitting I am happy to help any way I can. 