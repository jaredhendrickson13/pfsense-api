<?php
//   Copyright 2024 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
namespace RESTAPI\Core;

require_once 'RESTAPI/autoloader.inc';

use Error;
use Exception;
use JsonException;
use RESTAPI;
use RESTAPI\ContentHandlers\JSONContentHandler;
use RESTAPI\Responses\AuthenticationError;
use RESTAPI\Responses\ForbiddenError;
use RESTAPI\Responses\MediaTypeError;
use RESTAPI\Responses\MethodNotAllowedError;
use RESTAPI\Responses\ServerError;
use RESTAPI\Responses\Success;
use RESTAPI\Responses\ValidationError;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

/**
 * Represents an API Endpoint, encapsulating attributes essential for generating the API endpoint
 * and facilitating API calls. description is not adequate.
 */
class Endpoint {
    use BaseTraits;

    /**
     * @var string $url
     * The URL of this endpoint, used to generate a PHP file in the web path pointing
     * back to this Endpoint class.
     */
    public string $url = '';

    /**
     * @var string $model_name
     * The name of the Model class this Endpoint interacts with (excluding the Model
     * class's namespace).
     */
    public string $model_name = '';

    /**
     * @var Model $model
     * The actual Model object created for the assigned $model_name.
     */
    protected Model $model;

    /**
     * @var string $tag
     * The OpenAPI tag applied to this Endpoint, nesting it under a specific name in
     * the OpenAPI documentation. Defaults to the first value in the $url after
     * /api/v2/.
     */
    public string $tag = '';

    /**
     * @var array $errors
     * An array to store errors encountered during API calls to this Endpoint.
     */
    protected array $errors = [];

    /**
     * @var Auth $client
     * The \RESTAPI\Core\Auth object created by this Endpoint during API calls, contains
     * client authentication information.
     */
    protected Auth $client;

    /**
     * @var array $api_config
     * Contains the current API configuration.
     */
    public array $api_config;

    /**
     * @var array $request_data
     * The request body or parameters sent by the remote client.
     */
    public array $request_data = [];

    /**
     * @var bool $many
     * Indicates whether this Endpoint interacts with a single Model object (false) or
     * many Model objects (true).
     */
    public bool $many = false;

    /**
     * @var string $request_method
     * The HTTP request method sent by the remote client.
     */
    public string $request_method;

    /**
     * @var array $request_method_options
     * Sets the allowed HTTP request methods for this Endpoint. For $many=true, only
     * GET and PUT methods can be specified here. GET will use the Model's `read_all()`
     * method, and PUT will use the Model's `replace_all()` method. For Endpoints with
     * $many=false, GET, POST, PATCH, and DELETE methods can be specified here. GET will
     * use the Model's `read()` method, POST will use the Model's `create()` method.
     * PATCH will use the Model's `update()` method, and DELETE will use the Model's
     * `delete()` method.
     */
    public array $request_method_options = [];

    /**
     * @property bool $requires_auth
     * Specify whether this Endpoint should require remote clients to authenticate
     * before allowing them to interact with the assigned Model's objects. In almost
     * every situation, this should be set to `true`!
     */
    public bool $requires_auth = true;

    /**
     * @var array $auth_methods
     * Explicitly specify authentication methods this Endpoint uses. Leave blank to
     * use the authentication methods allowed by the REST API settings. If any
     * authentication methods are specified here, this Endpoint will always allow
     * authentication using this method, even if the method is not enabled in the
     * REST API settings.
     */
    public array $auth_methods = [];

    /**
     * @var bool $ignore_read_only
     * Allow this Endpoint to ignore the current `read_only` API setting. By default,
     * when the `read_only` API setting is enabled, Endpoints are only allowed to
     * respond to GET requests. Setting this value to `true` will allow requests using
     * any HTTP method even if `read_only` is enabled.
     */
    public bool $ignore_read_only = false;

    /**
     * @var bool $ignore_interfaces
     * Allow this Endpoint to ignore the `allowed_interfaces` API setting. By default,
     * if an Endpoint receives an API call on an interface not specified in the
     * `allowed_interfaces` API setting, a ForbiddenError will be thrown. Setting this
     * value to `true` will allow this Endpoint to respond to requests over any
     * interface regardless of what is specified in `allowed_interfaces`.
     */
    public bool $ignore_interfaces = false;

    /**
     * @var bool $ignore_enabled
     * Allow this Endpoint to ignore the `enabled` API setting. By default, Endpoints
     * will not accept API calls when the API's `enabled` setting is disabled. Setting
     * this value to `true` will allow this Endpoint to respond to API calls even if
     * the API is disabled.
     */
    public bool $ignore_enabled = false;

    /**
     * @var ResourceLinkSet|null $resource_link_set
     * A ResourceLinkSet containing ResourceLinks that should always be included in
     * the root _links for this response.
     */
    public ResourceLinkSet|null $resource_link_set = null;

    /**
     * @var array $get_privileges
     * Assigns pfSense privileges that should allow GET requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $get_privileges = [];

    /**
     * @var array $post_privileges
     * Assigns pfSense privileges that should allow POST requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $post_privileges = [];

    /**
     * @var array $patch_privileges
     * Assigns pfSense privileges that should allow PATCH requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $patch_privileges = [];

    /**
     * @var array $put_privileges
     * Assigns pfSense privileges that should allow PUT requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $put_privileges = [];

    /**
     * @var array $delete_privileges
     * Assigns pfSense privileges that should allow DELETE requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $delete_privileges = [];

    /**
     * @var string $get_help_text
     * Sets the GET request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $get_help_text = '';

    /**
     * @var string $post_help_text
     * Sets the POST request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $post_help_text = '';

    /**
     * @var string $put_help_text
     * Sets the PUT request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $put_help_text = '';

    /**
     * @var string $patch_help_text
     * Sets the PATCH request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $patch_help_text = '';

    /**
     * @var string $delete_help_text
     * Sets the DELETE request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $delete_help_text = '';

    /**
     * Sets values whenever the Endpoint object is crated.
     */
    public function __construct() {
        # Prevent PHP warnings in API Responses.
        error_reporting(E_ERROR);

        # Fetch the current API configuration whenever the object is first created.
        $this->api_config = RESTAPI\Models\RESTAPISettings::get_pkg_config();

        # Fetch request method whenever the object is first created.
        $this->request_method = strtoupper($_SERVER['REQUEST_METHOD']);

        # Assign the default $resource_links value if not specified by a child class
        $this->resource_link_set = $this->resource_link_set ?: new ResourceLinkSet();

        # Initialize the assigned model
        $model_class = 'RESTAPI\\Models\\' . $this->model_name;
        $this->model = new $model_class();

        # Set the default OpenAPI tag for this endpoint if no tag was explicitly assigned
        $this->set_default_tag();

        # Set default help text for any help text properties that were not set by the child class
        $this->set_default_help_text();

        # Check for conflicting attributes
        $this->check_construct();

        # Set default endpoint privileges
        $this->get_default_privs();
    }

    /**
     * Checks for conflicting attributes during object construction.
     */
    private function check_construct(): void {
        # Do not allow `many` Endpoints that are assigned non `many` Models
        if ($this->many and !$this->model->many) {
            throw new ServerError(
                message: 'Endpoints cannot enable `many` when the assigned Model is not a `many` Model',
                response_id: 'ENDPOINT_MANY_WITHOUT_MANY_MODEL',
            );
        }

        # Do not allow Endpoints to allow unsupported request methods
        $supported_methods = $this->many ? ['GET', 'PUT'] : ['GET', 'POST', 'PATCH', 'DELETE'];
        foreach ($this->request_method_options as $method_option) {
            if (!in_array($method_option, $supported_methods)) {
                throw new ServerError(
                    "Endpoint contains unsupported request method option `$method_option`.",
                    response_id: 'ENDPOINT_WITH_UNSUPPORTED_REQUEST_METHOD_OPTION',
                );
            }
        }

        # Do not allow `many` Endpoints that are assigned a Model with a parent Model
        if ($this->many and $this->model->parent_model_class) {
            throw new ServerError(
                message: 'Endpoints cannot enable `many` when the assigned Model has a parent Model',
                response_id: 'ENDPOINT_MANY_WHEN_MODEL_HAS_PARENT',
            );
        }
    }

    /**
     * Use the endpoint's URL to determine a default OpenAPI tag for this endpoint.
     */
    private function set_default_tag(): void {
        # Only assume the default if a tag was not set
        if (!$this->tag) {
            # Set the tag for this URL according to it's root namespace (the section in URL after /api/v2/)
            $this->tag = strtoupper(explode('/', $this->url)[3]);
        }
    }

    /**
     * Determines the appropriate default help text for each method according to the assigned Model and Endpoint properties.
     */
    private function set_default_help_text(): void {
        # Variables
        $model_verbose_name = $this->model->verbose_name;
        $model_verbose_name_plural = $this->model->verbose_name_plural;

        # For non `many` Endpoints, set default help text with an applicable context for `many` Models
        if (!$this->many and $this->model->many) {
            $this->get_help_text = $this->get_help_text ?: "Reads an existing $model_verbose_name.";
            $this->post_help_text = $this->post_help_text ?: "Creates a new $model_verbose_name.";
            $this->patch_help_text = $this->patch_help_text ?: "Updates an existing $model_verbose_name.";
            $this->delete_help_text = $this->delete_help_text ?: "Deletes an existing $model_verbose_name.";
        }
        # For `many` Vies, set default help text with an applicable context for `many` Models
        elseif ($this->many and $this->model->many) {
            $this->get_help_text = $this->get_help_text ?: "Reads all existing $model_verbose_name_plural.";
            $this->put_help_text = $this->put_help_text ?: "Replaces all existing $model_verbose_name_plural.";
        }
        # Set default help text with an applicable context for non `many` Models
        else {
            $this->get_help_text = $this->get_help_text ?: "Reads current $model_verbose_name_plural.";
            $this->post_help_text = $this->post_help_text ?: "Creates $model_verbose_name_plural.";
            $this->patch_help_text = $this->patch_help_text ?: "Updates current $model_verbose_name_plural.";
            $this->delete_help_text = $this->delete_help_text ?: "Deletes current $model_verbose_name_plural.";
        }
    }

    /**
     * Obtains the `prev` and `next` HATEOAS resource links for pagination use. These links supply the previous and next
     * set of objects from the dataset.
     * @param int $limit The maximum number of Model objects to retrieve. This will be used to determine
     * the `prev` and `next` link values.
     * @param int $offset The starting point in the dataset requested for pagination. This will be used to determine
     * the `prev` and `next` link values.
     */
    public function get_pagination_resource_links(int $limit, int $offset): void {
        # Calculate the offset for the `prev` and `next` resource link values
        $prev_offset = max($offset - $limit, 0);
        $next_offset = $offset + $limit;

        # Format the URLs using the calculate offsets
        $prev_url = $this->url . "?offset=$prev_offset&limit=$limit";
        $next_url = $this->url . "?offset=$next_offset&limit=$limit";

        # Include the pagination links to our $resource_link_set property
        $this->resource_link_set->resource_links[] = new ResourceLink(
            rel: 'prev',
            href: $prev_url,
            use_namespace: false,
        );
        $this->resource_link_set->resource_links[] = new ResourceLink(
            rel: 'next',
            href: $next_url,
            use_namespace: false,
        );
    }

    /**
     * Generates the privilege name necessary to execute a specific request method on this Endpoint.
     * @param string $method The HTTP method for which the resulting privilege name is intended.
     * @returns string The complete pfSense privilege name needed to authorize API requests to this endpoint using
     * the specified $method.
     */
    private function get_method_priv_name(string $method): string {
        # Convert the URL and request method to a pfSense privilege name
        $priv_name_prefix = str_replace('/', '-', $this->url) . '-';

        # Replace underscores with hyphens
        $priv_name_prefix = str_replace('_', '-', $priv_name_prefix);

        # Remove leading `-` if present, this occurs because the URL starts with a `/` which gets replaced with `-`
        if (str_starts_with($priv_name_prefix, '-')) {
            $priv_name_prefix = substr($priv_name_prefix, offset: 1);
        }

        return $priv_name_prefix . strtolower($method);
    }

    /**
     * Assigns the default privileges required to make API calls to this endpoint. This method will automatically
     * populate the following properties:
     *
     *      - $this->get_privileges
     *      - $this->post_privileges
     *      - $this->patch_privileges
     *      - $this->put_privileges
     *      - $this->delete_privileges
     */
    private function get_default_privs(): void {
        $page_all_priv = 'page-all';
        $this->get_privileges = [$page_all_priv, $this->get_method_priv_name('GET')];
        $this->post_privileges = [$page_all_priv, $this->get_method_priv_name('POST')];
        $this->patch_privileges = [$page_all_priv, $this->get_method_priv_name('PATCH')];
        $this->put_privileges = [$page_all_priv, $this->get_method_priv_name('PUT')];
        $this->delete_privileges = [$page_all_priv, $this->get_method_priv_name('DELETE')];
    }

    /**
     * Retrieves the necessary privileges for processing the received API call.
     * @return array An array of required privileges for the current API call.
     */
    private function get_required_privs(): array {
        # Determine the privileges required for this API call
        return match ($this->request_method) {
            'GET' => $this->get_privileges,
            'POST' => $this->post_privileges,
            'PUT' => $this->put_privileges,
            'PATCH' => $this->patch_privileges,
            'DELETE' => $this->delete_privileges,
            default => [],
        };
    }

    /**
     * Utilizes the $url and $request_method_options properties of this Endpoint to generate pfSense privileges
     * specific to this Endpoint class. The resulting array is then supplied to the pfSense privilege system
     * located at /etc/inc/priv/.
     * @returns array The pfSense priv list entry array corresponding to the privileges of this Endpoint.
     */
    public function generate_pfsense_privs(): array {
        # Set an array to populate pfSense priv entries for this Endpoint
        $privs = [];

        # Loop through each support request method and create a pfSense privilege entry for it
        foreach ($this->request_method_options as $method_option) {
            # Set the privilege config to pass to pfSense's privilege system
            $privs[$this->get_method_priv_name($method_option)] = [
                'name' => "REST API - $this->url " . strtoupper($method_option),
                'descr' => "Allow $method_option request to $this->url",
            ];
        }

        return $privs;
    }

    /**
     * Checks if authentication and authorization for this Endpoint is successful. This will attempt authentication
     * and authorization for each allowed auth method. Only one auth method needs to succeed to allow access.
     */
    private function check_auth(): void {
        # Only require auth if the Endpoint is configured to do so or this is an OPTIONS request
        if (!$this->requires_auth or $this->request_method === 'OPTIONS') {
            $this->client = new Auth();
            return;
        }

        # Attempt to authenticate the remote client
        $this->client = Auth::attempt_multi_auth(
            auth_classes: $this->auth_methods,
            required_privileges: $this->get_required_privs(),
        );
    }

    /**
     * Checks if the API is enabled before allowing the call.
     */
    private function check_enabled(): void {
        if (!$this->api_config['enabled'] == 'enabled' and !$this->ignore_enabled) {
            header('Location: /');
            exit();
        }
    }

    /**
     * Checks if the interface receiving the API call is allowed to answer API calls.
     */
    private function check_server_ip(): void {
        # Variables
        $allow_ifs = $this->api_config['allowed_interfaces'];
        $whitelist = explode(',', $allow_ifs);

        # Allow the action if the allowed interfaces is empty
        if (empty($allow_ifs)) {
            return;
        }

        # Check if our server IP is in our whitelist
        foreach ($whitelist as $wif) {
            $if_info = get_interface_info($wif);
            # Check if our server IP is a valid if address, VIP, localhost, or any
            if ($_SERVER['SERVER_ADDR'] === $if_info['ipaddr']) {
                return;
            } elseif ($_SERVER['SERVER_ADDR'] === $if_info['ipaddrv6']) {
                return;
            } elseif (in_array($_SERVER['SERVER_ADDR'], ['::1', '127.0.0.1', 'localhost']) and $wif === 'localhost') {
                return;
            } elseif ($wif === 'any') {
                return;
            } elseif ($this->ignore_interfaces) {
                return;
            }
        }

        # Throw a forbidden error if this API call was made to a non-API enabled interface
        throw new ForbiddenError(
            message: 'The requested action is not allowed by admin policy',
            response_id: 'ENDPOINT_ACTION_NOT_ALLOWED',
        );
    }

    /**
     * Check if the HTTP method requested by the API client is supported by this view.
     */
    private function check_request_method(): void {
        # Always allow OPTIONS requests
        if ($this->request_method === 'OPTIONS') {
            return;
        }

        # Check if the requested HTTP method is a supported HTTP method.
        if (!in_array($this->request_method, $this->request_method_options)) {
            throw new MethodNotAllowedError(
                message: "Resource at $this->url does not support the requested HTTP method '$this->request_method'",
                response_id: 'ENDPOINT_METHOD_NOT_ALLOWED',
            );
        }

        # Throw an error if the API is currently in read mode and this is not a GET or OPTIONS request
        $read_only = ($this->api_config['read_only'] === 'enabled' and !$this->ignore_read_only);
        if ($read_only and $this->request_method !== 'GET') {
            throw new MethodNotAllowedError(
                message: "Resource at $this->url cannot support the requested HTTP method `$this->request_method` " .
                    'because the REST API is currently in read-only mode.',
                response_id: 'ENDPOINT_METHOD_NOT_ALLOWED_IN_READ_ONLY_MODE',
            );
        }
    }

    /**
     * Checks if the API client's request data was successfully received and parsed by the API. This method populates
     * the $this->request_data property according to the client's requested content-type.
     */
    private function check_request_data(): void {
        # Obtain the ContentHandler associated with the HTTP Content-Type header sent by the client
        $content_handler = ContentHandler::get_decode_handler();
        $this->request_data = $content_handler->decode();
        $this->validate_endpoint_fields();
    }

    /**
     * Performs validation on non-Model fields. These fields control how Models are initially obtained/constructed.
     * @throws ValidationError When one or more Endpoint releated fields fail validation.
     */
    private function validate_endpoint_fields(): void {
        # Ensure the special async, limit and offset parameters are of the correct type
        if (!isset($this->request_data['async'])) {
            $this->request_data['async'] = true;
        }
        if (!is_bool($this->request_data['async'])) {
            throw new ValidationError(
                message: 'Field `async` must be of type `boolean`.',
                response_id: 'ENDPOINT_ASYNC_FIELD_INVALID_TYPE',
            );
        }
        if (!isset($this->request_data['limit'])) {
            $this->request_data['limit'] = 0;
        }
        if (!is_integer($this->request_data['limit'])) {
            throw new ValidationError(
                message: 'Field `limit` must be of type `integer`.',
                response_id: 'ENDPOINT_LIMIT_FIELD_INVALID_TYPE',
            );
        }
        if (!isset($this->request_data['offset'])) {
            $this->request_data['offset'] = 0;
        }
        if (!is_integer($this->request_data['offset'])) {
            throw new ValidationError(
                message: 'Field `offset` must be of type `integer`.',
                response_id: 'ENDPOINT_OFFSET_FIELD_INVALID_TYPE',
            );
        }
    }

    /**
     * Processes the API request and returns the serialized API response to send back to the client.
     * @return string The serialized API response to send back to the client.
     */
    public function process_request(): string {
        # Variables
        $resp_data = null;
        $content_handler = new JSONContentHandler();

        try {
            # Ensure API is enabled and the receiving interface is allowed to answer API calls.
            $this->check_server_ip();

            # Checks client authentication and authorization if required
            if ($this->requires_auth) {
                $this->check_auth();
            }

            # Ensure we can obtain the ContentHandler that will be responsible for encoding response content
            # This will throw an error if the client's Accept header is not supported by an existing ContentHandler
            $content_handler = ContentHandler::get_encode_handler();

            # Checks that request data was properly received and parsed.
            $this->check_request_method();
            $this->check_request_data();

            # Assign the authenticated client object context to the Model
            $this->model->client = $this->client;

            # Set this Model's async property if specified in the request data
            if (isset($this->request_data['async'])) {
                $this->model->async = $this->request_data['async'];
            }

            # Perform the appropriate action based on the request method
            switch ($this->request_method) {
                case 'GET':
                    $resp_data = $this->get();
                    break;
                case 'POST':
                    $resp_data = $this->post();
                    break;
                case 'PUT':
                    $resp_data = $this->put();
                    break;
                case 'PATCH':
                    $resp_data = $this->patch();
                    break;
                case 'DELETE':
                    $resp_data = $this->delete();
                    break;
                case 'OPTIONS':
                    $this->options();
                    break;
                default:
                    throw new MethodNotAllowedError(
                        message: "Unknown HTTP method '$this->request_method'",
                        response_id: 'ENDPOINT_REQUEST_METHOD_UNKNOWN',
                    );
            }

            # Create our success API response object if no errors were thrown
            $response = new Success(
                message: '',
                response_id: 'SUCCESS',
                data: $resp_data ?: new ModelSet(),
                _links: $this->resource_link_set,
            );
        } catch (Response $response) {
        } catch (Exception | Error $error) {
            $response = new ServerError(
                message: $error->getMessage() . PHP_EOL . $error->getTraceAsString(),
                response_id: 'ENDPOINT_RECEIVED_UNEXPECTED_ERROR',
            );
        }

        # Format the HTTP response as JSON and set response code
        http_response_code($response->code);
        return $content_handler->encode($response->to_representation()) . PHP_EOL;
    }

    /**
     * Obtains the object data for GET requests to this Endpoint.
     */
    public function get(): Model|ModelSet {
        # For GET requests on non-many Endpoints, obtain a single object from the assigned Model.
        if ($this->many) {
            $this->get_pagination_resource_links(
                limit: $this->request_data['limit'],
                offset: $this->request_data['offset'],
            );
            return $this->model->query(
                query_params: $this->request_data,
                excluded: ['async', 'limit', 'offset'],
                limit: $this->request_data['limit'],
                offset: $this->request_data['offset'],
            );
        }
        # For GET requests on many Endpoints, obtain all objects from the assigned Model.
        else {
            $this->model->from_representation(data: $this->request_data);
            return $this->model->read();
        }
    }

    /**
     * Creates a new object for the assigned Model using the data submitted in a POST request.
     */
    public function post(): Model|ModelSet {
        # POST request cannot include an ID, strip the ID if present
        unset($this->request_data['id']);
        $this->model->from_representation(data: $this->request_data);
        $this->model->placement = $this->request_data['placement'] ?? null;
        return $this->model->create(apply: $this->request_data['apply'] === true);
    }

    /**
     * Replaces all existing objects for the assigned Model using the data submitted in a PUT request.
     */
    public function put(): Model|ModelSet {
        # Remove magic parameters
        unset($this->request_data['async']);
        unset($this->request_data['limit']);
        unset($this->request_data['offset']);
        return $this->model->replace_all(data: $this->request_data);
    }

    /**
     * Updates an existing object for the assigned Model using the data submitted in a PATCH request.
     */
    public function patch(): Model|ModelSet {
        $this->model->from_representation(data: $this->request_data);
        $this->model->placement = $this->request_data['placement'] ?? null;
        return $this->model->update(apply: $this->request_data['apply'] === true);
    }

    /**
     * Deletes an existing object for the assigned Model using the data submitted in a DELETE request.
     */
    public function delete(): Model|ModelSet {
        $this->model->from_representation(data: $this->request_data);
        return $this->model->delete(apply: $this->request_data['apply'] === true);
    }

    /**
     * Populates associated headers whenever an OPTIONS request is received.
     */
    final public function options(): void {
        header('access-control-allow-methods: ' . implode(', ', $this->request_method_options));
        header('Allow: ' . implode(', ', $this->request_method_options));
    }

    /**
     * Generate an endpoint for this view in the pfSense webroot.
     * @returns bool Returns true if the endpoint was successfully built, returns false otherwise.
     */
    public function build_endpoint_url(): bool {
        # Get the fully qualified and non qualified class names for this object
        $fq_class_name = get_class($this);
        $nq_class_name = (new \ReflectionClass($this))->getShortName();

        # Specify the PHP code to write to the Endpoints index.php file
        $code =
            "<?php\n" .
            "require_once('RESTAPI/Endpoints/$nq_class_name.inc');\n" .
            "echo (new $fq_class_name())->process_request();\n" .
            "header('Referer: no-referrer');\n" .
            "session_destroy();\n" .
            "exit();\n";

        # Create directories and files corresponding with class
        if ($this->url) {
            mkdir('/usr/local/www' . $this->url, 0644, true);
            file_put_contents('/usr/local/www' . $this->url . '/index.php', $code);
        }

        # Return true if the endpoint was successfully built
        if ($this->url and is_file('/usr/local/www' . $this->url . '/index.php')) {
            return true;
        }

        return false;
    }
}
