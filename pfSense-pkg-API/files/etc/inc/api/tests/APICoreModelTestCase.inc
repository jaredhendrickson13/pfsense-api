<?php

namespace API\Tests;

use API;

class APICoreModelTestCase extends API\Core\TestCase {
    /**
     * Checks that a Model object cannot be created when both an ID and representation data are provided. This
     * ensures there is no confusion about which object ID to use when obtaining the model from its internal value.
     */
    public function test_no_model_with_id_and_representation_data() {
        $this->assert_throws_response(
            ref_id: "MODEL_WITH_ID_AND_REPRESENTATION",
            code: 500,
            callable: function () {
                new API\Core\Model(id: 0, representation_data: ["id" => 0]);
            }
        );
    }

    /**
     * Checks that we are able to pass in an API\Core\Auth object to relay external client information like username
     * and IP address. This is mostly commonly used by API\Core\View to provide Models information about the client
     * that authenticated a REST API call.
     */
    public function test_model_allows_provided_auth_obj() {
        # Create an API\Core\Auth object with custom values.
        $test_client = new API\Core\Auth();
        $test_client->username = "test_user";
        $test_client->ip_address = "1.2.3.4";

        # Create a Model object and provide the Auth object
        $test_model = new API\Core\Model(client: $test_client);

        # Ensure the Auth object was accepted and is using the correct values
        $this->assert_equals($test_model->client->username, $test_client->username);
        $this->assert_equals($test_model->client->ip_address, $test_client->ip_address);

    }

    /**
     * Checks that a default API\Core\Auth object is created when a Model object is created if none was provided.
     */
    public function test_model_default_auth_obj() {
        # Create a Model object and but do not provide an Auth object
        $test_model = new API\Core\Model();

        # Ensure the Auth object was accepted and is using the correct values
        $this->assert_equals($test_model->client->username, API\Core\DEFAULT_CLIENT_USERNAME);
        $this->assert_equals($test_model->client->ip_address, API\Core\DEFAULT_CLIENT_IP_ADDRESS);
    }

    /**
     * Checks that we are able to read items from the internal pfSense configuration by path.
     */
    public function test_model_get_config_valid_path() {
        # Try to read the WAN interface configuration since it is always present in the config
        $this->assert_type(API\Core\Model::get_config("interfaces/wan"), "array");
    }

    /**
     * Checks that a specified default value is returned if the config path does not lead to an existing value.
     */
    public function test_model_get_config_bad_path() {
        # Try to read the WAN interface configuration since it is always present in the config
        $this->assert_equals(API\Core\Model::get_config("does/not/exists", []), []);
        $this->assert_equals(API\Core\Model::get_config("does/not/exists", false), false);
        $this->assert_equals(API\Core\Model::get_config("does/not/exists"), null);
    }

    /**
     * Checks that we cannot call the default get_next_id() method if the model object does not have a config path set.
     */
    public function test_model_cant_get_next_id_without_config_path() {
        $this->assert_throws_response(
            ref_id: "MODEL_NEXT_ID_WITHOUT_CONFIG_PATH",
            code: 500,
            callable: function () {
                $test_model = new API\Core\Model();
                $test_model->get_next_id();
            }
        );
    }

    /**
     * Checks that we cannot call the default get_next_id() method if the model object does not have `many` enabled.
     */
    public function test_model_cant_get_next_id_without_many() {
        $this->assert_throws_response(
            ref_id: "MODEL_NEXT_ID_WITHOUT_MANY",
            code: 500,
            callable: function () {
                $test_model = new API\Core\Model();
                $test_model->config_path = "test/config/path";
                $test_model->get_next_id();
            }
        );
    }

    /**
     * Checks that a response is thrown if we call get_next_id() but the 'config_path' leads to an existing, non-index
     * array value. This helps prevents dangerous write actions as it indicates the 'config_path' is wrong, or there
     * is a problem with the pfSense configuration.
     */
    public function test_model_cant_get_next_id_with_dangerous_config_path() {
        # Try to read the system hostname (which is a primitive string in the pfSense configuration). To get an ID, we
        # expect the value be to an indexed array or an empty value.
        $this->assert_throws_response(
            ref_id: "MODEL_DANGEROUS_CONFIG_PATH",
            code: 500,
            callable: function () {
                $test_model = new API\Core\Model();
                $test_model->config_path = "system/hostname";
                $test_model->many = true;
                $test_model->get_next_id();
            }
        );
    }

    /**
     * Checks that we can successfully obtain the next ID for a non-existing config path. Empty paths should always
     * return the first available ID, `0`.
     */
    public function test_model_can_get_next_id_at_empty_path() {
        $test_model = new API\Core\Model();
        $test_model->config_path = "this/path/does/not/exist/yet";
        $test_model->many = true;
        $this->assert_equals($test_model->get_next_id(), 0);
    }

    /**
     * Checks that we can successfully obtain the next ID for an existing config path if it's an array. This test
     * checks that the ID returned for filter/rule is greater than 0 because it's generally safe to assume there
     * will always be at least one item in filter/rule.
     */
    public function test_model_can_get_next_id_from_existing_array_path() {
        $test_model = new API\Core\Model();
        $test_model->config_path = "filter/rule";
        $test_model->many = true;
        $this->assert_is_greater_than($test_model->get_next_id(), 1);
    }

    /**
     * Checks that Models can correctly convert to their internal values.
     */
    public function test_model_to_internal_simple() {
        # Since the default Model does not have any fields, use a FirewallAlias Model to test this.
        $test_model = new API\Models\FirewallAlias();
        $test_model->name->value = "test_alias";
        $test_model->type->value = "port";
        $test_model->address->value = ["1","2","3","4","5"];
        $test_model->detail->value = ["a", "b", "c", "d", "e"];
        $test_model->validate();

        # Ensure the internal value of this object matches what is expected
        $this->assert_equals(
            $test_model->to_internal(),
            [
                "name"=>$test_model->name->value,
                "type"=>$test_model->type->value,
                "descr"=>$test_model->descr->value,
                "address"=>implode($test_model->address->delimiter, $test_model->address->value),
                "detail"=>implode($test_model->detail->delimiter, $test_model->detail->value)
            ]
        );
    }

    /**
     * Checks that the Model's get_fields() method properly finds Field objects and their assigned names.
     */
    public function test_model_get_fields() {
        # Use a FirewallAlias model since the base Model has no fields assigned.
        $this->assert_equals(
            (new API\Models\FirewallAlias())->get_fields(),
            ["name", "type", "descr", "address", "detail"]
        );
    }

    /**
     * Checks to ensure the validate() method properly throws an error if a required pfSense package for the Model
     * is missing.
     */
    public function test_model_validate_packages_missing_package() {
        $this->assert_throws_response(
            ref_id: "MDOEL_MISSING_REQUIRED_PACKAGE",
            code: 404,
            callable: function () {
                $test_model = new API\Core\Model();
                $test_model->packages = ["pfSense-pkg-some_package_we_dont_have"];
                $test_model->validate();
            }
        );
    }

    /**
     * Checks to ensure the validate() method is successful when we have the correct packages installed.
     */
    public function test_model_validate_packages_installed_package() {
        $test_model = new API\Core\Model();
        $test_model->packages = ["pfSense-pkg-API"];    # Use pfSense-pkg-API since we know we have it.
        $test_model->validate();
    }

    /**
     * Checks to ensure the validate() method properly throws an error if we cannot include a required package include
     * file because it is not found in the PHP path.
     */
    public function test_model_validate_missing_package_includes() {
        $this->assert_throws_response(
            ref_id: "MODEL_WITH_FAILED_INCLUDE",
            code: 404,
            callable: function () {
                $test_model = new API\Core\Model();
                $test_model->package_includes = ["this_inc_file_does_not_exist.inc"];
                $test_model->validate();
            }
        );
    }
}
