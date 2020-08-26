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

Writing new API Endpoints
---------------------
Most contributions to this project will be in the form of integrating new API endpoints. API endpoints are comprised of
a few different components. It is strongly recommended that you familiarize yourself with pfSense's PHP shell before
diving into creating endpoints. To get started writing your own endpoints, please follow the steps below:

### Things to Know ###
 - The API is based on REST principals. Unfortunately, pfSense does not seem to allow any custom changes to the NGINX 
 configuration so alternate request methods like `PUT` and `DELETE` do not appear to be possible. To accommodate this, 
 the requested action must be defined in the endpoint path. 
    - Create actions must be a `POST` request to an endpoint ending in `/add/` 
     (e.g. `https://localhost/api/v1/firewall/rules/add/`) 
    - Read actions must be a `GET` request to a base endpoint (e.g. `https://localhost/api/v1/firewall/rules/`)
    - Update actions must be a `POST` request to an endpoint ending in `/update/` 
     (e.g. `https://localhost/api/v1/firewall/rules/update/`)
    - Delete actions must be a `POST` request to an endpoint ending in `/delete/`
     (e.g. `https://localhost/api/v1/firewall/rules/delete/`)

### Writing the API model ### 
At the core of the API endpoint is the API model. This is a class that validates client request data, writes changes
to the pfSense configuration file, and makes any corresponding system changes. pfSense API is distributed with a 
custom micro-framework to accommodate developers wanting to contribute or create their own API models and endpoints.

#### Getting Started ####
To get started creating a new API model, you first need to create a new PHP file in `/files/etc/inc/api/api_models` and
create a new class that extends our APIBaseModel framework class:

```php
<?php
require_once("api/framework/APIBaseModel.inc");

class NewAPIModel extends APIBaseModel {
    
}
```

#### Constructing the API Model ####
In order to use the APIBaseModel framework, you must add a `__construct()` method to your new API model class and 
initialize the APIBaseModel class as such. Additionally, you may specify any model attribute overrides within this 
method:

```php
<?php
require_once("api/framework/APIBaseModel.inc");

class NewAPIModel extends APIBaseModel {
    # Create our construct method
    public function __construct() {
        parent::__construct();    // Initializes our APIBaseModel class

        # Add your API model properties here (see Overriding Base Model Properties
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-diagnostics-arptable"];
        $this->requires_auth = false;
    }
}
```

#### Overriding Base Model Properties ####
There are several class properties that you can customize to fit the needs of your API Model. If a model attribute is
not specified, the default values are assumed:

- `$this->methods` : Allows you to set what HTTP methods are allowed in array format. Due limitations of pfSense's NGINX
 configuration, only GET and POST requests can be used. Defaults to `["GET", "POST"]`.
 
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

- `$this->change_note` : Sets the description of the action that occurred via API. This value will be shown in the 
change logs found at Diagnostics > Backup & Restore > Config History. This defaults to "Made unknown change via API". 
This is only necessary if your API model writes changes to the configuration.


#### Other Base Model Properties ####
There are other properties inherited from APIBaseModel that are not intended (and shouldn't be) overridden by your
custom API model:

- `$this->client` : An APIAuth object that contains information about the client (for more see Accessing Client Data)
- `$this->initial_data` : The request data as it was when the object was created
- `$this->validated_data` : An array for validators to use to populate data that has been validated
- `$this->errors` : An array to populate any errors encountered. Should be an array of APIResponse values.
- `$this->config` : Our pfSense configuration array. You may read current configuration values using this array or write
changes to the configuration by updating it's values. If you do make changes to the configuration, you must use call
`$this->write_config()` to apply them. 

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
require_once("api/framework/APIBaseModel.inc");

class NewAPIModel extends APIBaseModel {
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-diagnostics-arptable"];
        $this->requires_auth = false;
    }

    # Overrides our APIBaseModel validate_payload method to validate data within our initial_data property
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
overriding the APIBaseModel's `action()` method. This method should simply be a set of tasks to perform upon a 
successful API call. If you are writing an endpoint to perform a change that is usually performed via the pfSense 
webConfigurator, it may be helpful to look at the PHP code for the webConfigurator page. This method must return an
APIResponse item.

As a basic example, if I wanted to add our validated input to our pfSense configuration:

```php
class NewAPIModel extends APIBaseModel {
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
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

#### Writing API endpoints ####
API models are not useful if we do not have an API endpoint that calls upon the model. API endpoints are the PHP scripts
that will be placed in pfSense's web path. To create a new API endpoint, add an index.php file in 
`/files/usr/local/www/api/v1/` wherever makes the most sense for your API call. You may also create new directories
if none of the existing ones describe your API call well enough. For example, if I wanted to create a new API call
that would be available at https://pfsensehost/api/v1/system/test/, I would need to create a new directory within
/files/usr/local/www/api/v1/system/ called 'test' and add my index.php in that directory. 

All that is needed for API endpoints is the following. Be sure to change the API model class name to your own:
```php
<?php
require_once("api/api_models/YourAPIModel.inc");

# Creates our API model object and listens for API calls
(new YourAPIModel())->listen();    
```



#### Accessing Client Data ####
If for any reason you need to access client data from within your API model class, you can access the `$this->client`
property. This is an APIAuth object that contains details about the client:

- `$this->client->username` : Our client's corresponding pfSense username
- `$this->client->ip_address` : The IP address of our client
- `$this->client->is_authenticated` : Whether the client successfully authenticated. Keep in mind the APIBaseModel will
handle all authentication and authorization for you, so this is likely unneeded.
- `$this->client->is_authorized` : Whether the client was authorized. Keep in mind the APIBaseModel will
handle all authentication and authorization for you, so this is likely unneeded.
- `$this->client->privs` : An array of privileges this user has
- `$this->client->auth_mode` : The authentication mode that was used to authenticate the user

### Writing API responses ###
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


### Writing tool functions ###
Often times you will need to create functions to condense redundant tasks. You can place any necessary tool functions in
`/files/etc/inc/api/framework/APITools.inc`. You may then access the tool function from your API model like this:

`$some_variable = APITools\your_custom_tool_function();`

### Adding Models and Endpoints to the Package
The package Makefile and pkg-plist files are auto generated by `tools/make_package.py`. This simply pulls the files and
directories needed to build our package from the `files` directory. For more information on building the package, refer
to `tools/README.md`

    
Questions
---------
There are some complex components to this project. Please feel free to reach out with any questions, 
issues, or insight you have when creating API endpoints. Time permitting I am happy to help any way I can. 