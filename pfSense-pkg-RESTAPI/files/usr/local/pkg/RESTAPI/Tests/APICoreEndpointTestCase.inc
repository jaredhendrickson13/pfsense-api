<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Model;
use RESTAPI\Core\TestCase;
use RESTAPI\Endpoints\DiagnosticsARPTableEndpoint;
use RESTAPI\Endpoints\FirewallAliasEndpoint;
use RESTAPI\Endpoints\FirewallAliasesEndpoint;
use RESTAPI\Endpoints\ServicesDNSResolverHostOverrideAliasEndpoint;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\NetworkInterface;
use RESTAPI\Models\RESTAPIAccessListEntry;
use RESTAPI\Models\RESTAPISettings;
use RESTAPI\Models\VirtualIP;

class APICoreEndpointTestCase extends TestCase {
    /**
     * Sets up this test case with the necessary settings.
     */
    public function setup(): void {
        # Disable login protection, so we don't flood syslog with login attempts during tests
        $api_settings = new RESTAPISettings(login_protection: false);
        $api_settings->update();
    }

    /**
     * Tears down this test case, cleaning up any changed data.
     */
    public function teardown(): void {
        # Reset the API settings to their default values
        $api_settings = new RESTAPISettings(login_protection: true);
        $api_settings->update();
    }

    /**
     * Checks that the Endpoint is constructed correctly.
     */
    public function test_construct(): void {
        # Mock the request method being set to POST
        $_SERVER['REQUEST_METHOD'] = 'POST';

        # Use the FirewallAliasEndpoint to test this
        $endpoint = new FirewallAliasEndpoint();

        # Ensure was constructed properly
        $this->assert_equals('POST', $endpoint->request_method);
        $this->assert_is_true($endpoint->model instanceof FirewallAlias);
        $this->assert_equals($endpoint->tag, 'FIREWALL');
        $this->assert_is_not_empty($endpoint->get_help_text);
        $this->assert_is_not_empty($endpoint->post_help_text);
        $this->assert_is_not_empty($endpoint->patch_help_text);
        $this->assert_is_not_empty($endpoint->delete_help_text);
        $this->assert_is_not_empty($endpoint->get_privileges);
        $this->assert_is_not_empty($endpoint->post_privileges);
        $this->assert_is_not_empty($endpoint->patch_privileges);
        $this->assert_is_not_empty($endpoint->delete_privileges);
    }

    /**
     * Checks that the check_construct() method works correctly.
     */
    public function test_check_construct(): void {
        # Mock the request method being set to POST
        $_SERVER['REQUEST_METHOD'] = 'POST';

        # Ensure endpoints cannot be a many endpoint without a many model
        $this->assert_throws_response(
            response_id: 'ENDPOINT_MANY_WITHOUT_MANY_MODEL',
            code: 500,
            callable: function () {
                # Mock a 'many' endpoint with a non many model
                $endpoint = new FirewallAliasEndpoint();
                $endpoint->model->many = false;
                $endpoint->many = true;
                $endpoint->check_construct();
            },
        );

        # Ensure support methods must be known
        $this->assert_throws_response(
            response_id: 'ENDPOINT_WITH_UNSUPPORTED_REQUEST_METHOD_OPTION',
            code: 500,
            callable: function () {
                # Mock a 'many' endpoint with a non many model
                $endpoint = new FirewallAliasEndpoint();
                $endpoint->request_method_options = ['UNKNOWN'];
                $endpoint->check_construct();
            },
        );

        # Ensure endpoints cannot be many if th model has a parent model
        $this->assert_throws_response(
            response_id: 'ENDPOINT_MANY_WHEN_MODEL_HAS_PARENT',
            code: 500,
            callable: function () {
                # Mock a 'many' endpoint with a non many model
                $endpoint = new ServicesDNSResolverHostOverrideAliasEndpoint();
                $endpoint->many = true;
                $endpoint->request_method_options = ['GET'];
                $endpoint->check_construct();
            },
        );
    }

    public function test_get_pagination_resource_links(): void {
        # Mock obtaining pagination resource links
        $endpoint = new FirewallAliasEndpoint();
        $endpoint->get_pagination_resource_links(limit: 2, offset: 5);

        # Ensure the 'prev' resource link correctly shows the previous page
        $this->assert_equals($endpoint->resource_link_set->resource_links[0]->rel, 'prev');
        $this->assert_equals(
            $endpoint->resource_link_set->resource_links[0]->href,
            $endpoint->url . '?offset=3&limit=2',
        );
        $this->assert_equals($endpoint->resource_link_set->resource_links[1]->rel, 'next');
        $this->assert_equals(
            $endpoint->resource_link_set->resource_links[1]->href,
            $endpoint->url . '?offset=7&limit=2',
        );
    }

    /**
     * Checks that the pfSense privileges are correctly generated for the endpoint.
     */
    public function test_generate_pfsense_privs(): void {
        $endpoint = new FirewallAliasEndpoint();
        $this->assert_equals($endpoint->generate_pfsense_privs(), [
            'api-v2-firewall-alias-get' => [
                'name' => 'REST API - /api/v2/firewall/alias GET',
                'descr' => 'Allow GET request to /api/v2/firewall/alias',
            ],
            'api-v2-firewall-alias-post' => [
                'name' => 'REST API - /api/v2/firewall/alias POST',
                'descr' => 'Allow POST request to /api/v2/firewall/alias',
            ],
            'api-v2-firewall-alias-patch' => [
                'name' => 'REST API - /api/v2/firewall/alias PATCH',
                'descr' => 'Allow PATCH request to /api/v2/firewall/alias',
            ],
            'api-v2-firewall-alias-delete' => [
                'name' => 'REST API - /api/v2/firewall/alias DELETE',
                'descr' => 'Allow DELETE request to /api/v2/firewall/alias',
            ],
        ]);
    }

    /**
     * Checks that the check_auth() method correct enforces authentication
     */
    public function test_check_auth(): void {
        # Ensure the endpoint requires authentication when the $requires_auth flag is set
        $_SERVER['PHP_AUTH_USER'] = null;
        $_SERVER['PHP_AUTH_PW'] = null;
        $_SERVER['REQUEST_METHOD'] = 'GET';
        $endpoint = new FirewallAliasEndpoint();
        $endpoint->requires_auth = true;
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'AUTH_AUTHENTICATION_FAILED');
        $this->assert_equals($json_resp->code, 401);

        # Ensure the endpoint allows unauthenticated requests if the $requires_auth flag is not set
        $endpoint->requires_auth = false;
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->response_id, 'AUTH_AUTHENTICATION_FAILED');
        $this->assert_not_equals($json_resp->code, 401);
    }

    /**
     * Checks that the API must be enabled for the endpoint to process requests
     */
    public function test_check_api_enabled(): void {
        # Ensure disabling the API results in endpoints returning 503
        $api_settings = new RESTAPISettings(enabled: false);
        $api_settings->update();
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_REST_API_IS_NOT_ENABLED');
        $this->assert_equals($json_resp->code, 503);

        # Ensure assigning the $ignore_enabled flag to true allows the endpoint to process requests even if the API is disabled
        $endpoint->ignore_enabled = true;
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->code, 503);
    }

    /**
     * Checks that the interface that received the API call must be allowed to access the API
     */
    public function test_check_interface_allowed(): void {
        # Ensure have any empty allowed interfaces list allows all interfaces to access the API
        $api_settings = new RESTAPISettings(allowed_interfaces: []);
        $api_settings->update();
        $_SERVER['SERVER_ADDR'] = '1.2.3.4';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->response_id, 'ENDPOINT_INTERFACE_NOT_ALLOWED');

        # Ensure that localhost interface is only allowed to access the API if configured
        $api_settings = new RESTAPISettings(allowed_interfaces: ['lo0']);
        $api_settings->update();
        $_SERVER['SERVER_ADDR'] = '127.0.0.1';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->response_id, 'ENDPOINT_INTERFACE_NOT_ALLOWED');

        $api_settings = new RESTAPISettings(allowed_interfaces: ['lan']);
        $api_settings->update();
        $_SERVER['SERVER_ADDR'] = '127.0.0.1';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_INTERFACE_NOT_ALLOWED');

        # Ensure that individual interfaces can be allowed to access the API
        $api_settings = new RESTAPISettings(allowed_interfaces: ['lan']);
        $api_settings->update();
        $_SERVER['SERVER_ADDR'] = (new NetworkInterface(id: 'lan'))->get_current_ipv4();
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->response_id, 'ENDPOINT_INTERFACE_NOT_ALLOWED');

        $api_settings = new RESTAPISettings(allowed_interfaces: ['wan']);
        $api_settings->update();
        $_SERVER['SERVER_ADDR'] = (new NetworkInterface(id: 'lan'))->get_current_ipv4();
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_INTERFACE_NOT_ALLOWED');

        # Ensure the interface's virtual IPs are also accepted
        $api_settings = new RESTAPISettings(allowed_interfaces: ['lan']);
        $api_settings->update();
        $vip = new VirtualIP(interface: 'lan', mode: 'ipalias', subnet: '192.168.1.2', subnet_bits: 24, async: false);
        $vip->create(apply: true);
        $_SERVER['SERVER_ADDR'] = $vip->subnet->value;
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->response_id, 'ENDPOINT_INTERFACE_NOT_ALLOWED');
        $vip->delete(apply: true);
    }

    /**
     * Checks that the check_request_method() method correctly stops unsupported request methods.
     */
    public function test_check_request_method(): void {
        # check_request_method() runs after authenticating, so we need to authenticate first
        $_SERVER['PHP_AUTH_USER'] = 'admin';
        $_SERVER['PHP_AUTH_PW'] = 'pfsense';
        $_SERVER['REMOTE_ADDR'] = '1.2.3.4';

        # Ensure the OPTIONS request method is always allowed
        $_SERVER['REQUEST_METHOD'] = 'OPTIONS';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->response_id, 'ENDPOINT_METHOD_NOT_ALLOWED');

        # Ensure unsupported request methods are blocked
        $_SERVER['REQUEST_METHOD'] = 'UNKNOWN';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_METHOD_NOT_ALLOWED');

        # Ensure only GET requests are allowed when the API is in read-only mode
        $api_settings = new RESTAPISettings(read_only: true);
        $api_settings->update();
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_METHOD_NOT_ALLOWED_IN_READ_ONLY_MODE');

        $api_settings = new RESTAPISettings(read_only: false);
        $api_settings->update();
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_not_equals($json_resp->response_id, 'ENDPOINT_METHOD_NOT_ALLOWED_IN_READ_ONLY_MODE');
    }

    /**
     * Ensure the check_request_data() method correctly obtains and decodes requested data from the client.
     */
    public function test_check_request_data(): void {
        # check_request_method() runs after authenticating, so we need to authenticate first
        $_SERVER['PHP_AUTH_USER'] = 'admin';
        $_SERVER['PHP_AUTH_PW'] = 'pfsense';
        $_SERVER['REMOTE_ADDR'] = '1.2.3.4';

        # Ensure the request data is correctly obtained and decoded
        $_SERVER['REQUEST_METHOD'] = 'GET';
        $_SERVER['HTTP_CONTENT_TYPE'] = 'application/x-www-form-urlencoded';
        $_SERVER['QUERY_STRING'] = 'key=value';
        $endpoint = new FirewallAliasEndpoint();
        $endpoint->process_request();
        $this->assert_equals($endpoint->request_data, ['key' => 'value']);
    }

    /**
     * Ensures that endpoint specific fields are correctly validated.
     */
    public function test_validate_endpoint_fields(): void {
        # check_request_method() runs after authenticating, so we need to authenticate first
        $_SERVER['PHP_AUTH_USER'] = 'admin';
        $_SERVER['PHP_AUTH_PW'] = 'pfsense';
        $_SERVER['REMOTE_ADDR'] = '1.2.3.4';

        # Ensure the request data is correctly obtained and decoded
        $_SERVER['REQUEST_METHOD'] = 'GET';
        $_SERVER['HTTP_CONTENT_TYPE'] = 'application/x-www-form-urlencoded';

        $_SERVER['QUERY_STRING'] = 'async=invalid';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_ASYNC_FIELD_INVALID_TYPE');

        $_SERVER['QUERY_STRING'] = 'limit=invalid';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_LIMIT_FIELD_INVALID_TYPE');

        $_SERVER['QUERY_STRING'] = 'offset=invalid';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_OFFSET_FIELD_INVALID_TYPE');
    }

    /**
     * Ensures the process_request() correctly handles 'POST' requests
     */
    public function test_process_request(): void {
        # check_request_method() runs after authenticating, so we need to authenticate first
        $_SERVER['PHP_AUTH_USER'] = 'admin';
        $_SERVER['PHP_AUTH_PW'] = 'pfsense';
        $_SERVER['REMOTE_ADDR'] = '1.2.3.4';

        # Mock an example POST request and ensure the response is correct
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $_SERVER['HTTP_CONTENT_TYPE'] = 'application/x-www-form-urlencoded';
        $_SERVER['QUERY_STRING'] = 'name=test_alias&type=host&descr=test description';
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request(), associative: true);
        $this->assert_equals($json_resp['data'], $endpoint->model->to_representation());
        $id = $json_resp['data']['id'];
        $alias = new FirewallAlias(id: $id);

        # Mock an example GET request and ensure the response is correct
        $_SERVER['REQUEST_METHOD'] = 'GET';
        $_SERVER['QUERY_STRING'] = "id=$id";
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request(), associative: true);
        $this->assert_equals($json_resp['data'], $alias->to_representation());

        # Mock an example PATCH request and ensure the response is correct
        $_SERVER['REQUEST_METHOD'] = 'PATCH';
        $_SERVER['QUERY_STRING'] = "id=$id&descr=new description";
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request(), associative: true);
        $alias = new FirewallAlias(id: $id);
        $this->assert_equals($json_resp['data']['descr'], 'new description');
        $this->assert_equals($json_resp['data'], $alias->to_representation());

        # Mock an example DELETE request and ensure the response is correct
        $_SERVER['REQUEST_METHOD'] = 'DELETE';
        $_SERVER['QUERY_STRING'] = "id=$id";
        $endpoint = new FirewallAliasEndpoint();
        $json_resp = json_decode($endpoint->process_request(), associative: true);
        $this->assert_equals($json_resp['data'], $alias->to_representation());
        $this->assert_is_false(FirewallAlias::query(id: $id)->exists());
    }

    /**
     * Ensures the get_endpoint_filepath() method correctly constructs the endpoint's PHP file path
     * in the pfSense webroot.
     */
    public function test_get_endpoint_filepath(): void {
        # Test a few different endpoints and ensure the filepath is correctly constructed
        $endpoint = new FirewallAliasEndpoint();
        $this->assert_equals($endpoint->get_endpoint_filepath(), '/usr/local/www/api/v2/firewall/alias/index.php');

        $endpoint = new DiagnosticsARPTableEndpoint();
        $this->assert_equals(
            $endpoint->get_endpoint_filepath(),
            '/usr/local/www/api/v2/diagnostics/arp_table/index.php',
        );

        $endpoint = new ServicesDNSResolverHostOverrideAliasEndpoint();
        $this->assert_equals(
            $endpoint->get_endpoint_filepath(),
            '/usr/local/www/api/v2/services/dns_resolver/host_override/alias/index.php',
        );
    }

    /**
     * Ensures the build_endpoint_url() method correctly constructs the endpoint's PHP file in the pfSense
     * webroot.
     */
    public function test_build_endpoint_url(): void {
        # Use the FirewallAliasEndpoint to test this
        $endpoint = new FirewallAliasEndpoint();

        # Remove the existing endpoint file
        unlink($endpoint->get_endpoint_filepath());
        $this->assert_is_false(file_exists($endpoint->get_endpoint_filepath()));

        # Run the build_endpoint_url() method and ensure the file is created
        $endpoint->build_endpoint_url();
        $this->assert_is_true(file_exists($endpoint->get_endpoint_filepath()));
        $this->assert_is_not_empty(file_get_contents($endpoint->get_endpoint_filepath()));
    }

    /**
     * Ensures we can make a PUT request to 'many' endpoints and receive a successful response.
     */
    public function test_put_many_e2e(): void {
        # Ensure a PUT request to a `many` endpoint requires the request data to be an indexed array
        $json_resp = \RESTAPI\Core\Tools\http_request(
            url: 'https://localhost/api/v2/firewall/aliases',
            method: 'PUT',
            data: ['not_an_indexed_array' => 'not_an_indexed_array'],
            headers: ['Content-Type' => 'application/json'],
            username: 'admin',
            password: 'pfsense',
            validate_certs: false,
        );
        $resp = json_decode($json_resp);
        $this->assert_equals($resp->response_id, 'ENDPOINT_REQUEST_DATA_MUST_BE_INDEXED_ARRAY');

        # Ensure we can make a successful PUT request to a `many` endpoint
        $json_resp = \RESTAPI\Core\Tools\http_request(
            url: 'https://localhost/api/v2/firewall/aliases',
            method: 'PUT',
            data: [
                ['name' => 'test_alias', 'type' => 'host', 'descr' => 'test description', 'address' => ['1.2.3.4']],
                ['name' => 'test_alias2', 'type' => 'host', 'descr' => 'test description2', 'address' => ['4.3.2.1']],
            ],
            headers: ['Content-Type' => 'application/json'],
            username: 'admin',
            password: 'pfsense',
            validate_certs: false,
        );
        $resp = json_decode($json_resp);
        $this->assert_equals($resp->code, 200);
    }

    /**
     * Ensures we can make a DELETE request to `many` endpoints and receive a successful response.
     */
    public function test_delete_many_e2e(): void {
        # Use a PUT request to create new aliases
        \RESTAPI\Core\Tools\http_request(
            url: 'https://localhost/api/v2/firewall/aliases',
            method: 'PUT',
            data: [
                ['name' => 'host_alias', 'type' => 'host'],
                ['name' => 'network_alias1', 'type' => 'network'],
                ['name' => 'network_alias2', 'type' => 'network'],
                ['name' => 'port_alias', 'type' => 'port'],
            ],
            headers: ['Content-Type' => 'application/json'],
            username: 'admin',
            password: 'pfsense',
            validate_certs: false,
        );

        # Ensure we can make a successful DELETE request to a `many` endpoint
        $json_resp = \RESTAPI\Core\Tools\http_request(
            url: 'https://localhost/api/v2/firewall/aliases',
            method: 'DELETE',
            data: ['type' => 'network'],
            headers: ['Content-Type' => 'application/x-www-form-urlencoded'],
            username: 'admin',
            password: 'pfsense',
            validate_certs: false,
        );
        $resp = json_decode($json_resp);
        $this->assert_equals($resp->code, 200);
        $this->assert_equals(count($resp->data), 2);
        $this->assert_equals($resp->data[0]->name, 'network_alias1');
        $this->assert_equals($resp->data[1]->name, 'network_alias2');
    }

    /**
     * Ensures that Endpoints correctly allow or deny access based on the REST API Access List.
     */
    public function test_check_acl(): void {
        # check_request_method() runs after authenticating, so we need to authenticate first
        $_SERVER['PHP_AUTH_USER'] = 'admin';
        $_SERVER['PHP_AUTH_PW'] = 'pfsense';

        # Mock the client's IP address as an IP that is not in the ACL
        $_SERVER['REMOTE_ADDR'] = '4.3.2.1';

        # Mock a GET request
        $_SERVER['REQUEST_METHOD'] = 'GET';

        # Ensure the Endpoint throws a Forbidden response when the client is not in the ACL
        $acl = new RESTAPIAccessListEntry();
        $acl->replace_all([['type' => 'allow', 'network' => '1.2.3.4/32']]);

        $endpoint = new FirewallAliasesEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->response_id, 'ENDPOINT_CLIENT_NOT_ALLOWED_BY_ACL');
        $this->assert_equals($json_resp->code, 403);

        # Mock the client's IP address as an IP that is in the ACL and ensure the Endpoint processes the request
        $_SERVER['REMOTE_ADDR'] = '1.2.3.4';
        $endpoint = new FirewallAliasesEndpoint();
        $json_resp = json_decode($endpoint->process_request());
        $this->assert_equals($json_resp->code, 200);
    }
}
