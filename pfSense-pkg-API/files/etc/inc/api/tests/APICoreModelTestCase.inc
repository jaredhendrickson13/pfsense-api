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
            response_id: "MODEL_WITH_ID_AND_REPRESENTATION",
            code: 500,
            callable: function () {
                new API\Core\Model(id: 0, representation_data: ["id" => 0]);
            }
        );
    }

    /**
     * Checks that we are able to pass in an API\Core\Auth object to relay external client information like username
     * and IP address. This is mostly commonly used by API\Core\Endpoint to provide Models information about the client
     * that authenticated a REST API call.
     */
    public function test_model_allows_provided_auth_obj() {
        # Create an API\Core\Auth object with custom values.
        $test_client = new API\Core\Auth();
        $test_client->username = "test_user";
        $test_client->ip_address = "1.2.3.4";

        # Create a Model object and provide the Auth object
        $test_model = new API\Models\Test(client: $test_client);

        # Ensure the Auth object was accepted and is using the correct values
        $this->assert_equals($test_model->client->username, $test_client->username);
        $this->assert_equals($test_model->client->ip_address, $test_client->ip_address);

    }

    /**
     * Checks that a default API\Core\Auth object is created when a Model object is created if none was provided.
     */
    public function test_model_default_auth_obj() {
        # Create a Model object and but do not provide an Auth object
        $test_model = new API\Models\Test();

        # Ensure the Auth object was accepted and is using the correct values
        $this->assert_equals($test_model->client->username, API\Core\DEFAULT_CLIENT_USERNAME);
        $this->assert_equals($test_model->client->ip_address, API\Core\DEFAULT_CLIENT_IP_ADDRESS);
    }

    /**
     * Checks the Model's default `verbose_name` and `verbose_name_plural` are properly derived from its class name.
     */
    public function test_model_default_verbose_name() {
        # Create a normal Model object and a FirewallAlias Model object for testing
        $test_model = new API\Models\Test();
        $test_fw_alias = new API\Models\FirewallAlias();

        # Ensure their default names are correct
        $this->assert_equals($test_model->verbose_name, "Test");
        $this->assert_equals($test_model->verbose_name_plural, "Tests");
        $this->assert_equals($test_fw_alias->verbose_name, "Firewall Alias");
        $this->assert_equals($test_fw_alias->verbose_name_plural, "Firewall Aliases");
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
            response_id: "MODEL_NEXT_ID_WITHOUT_CONFIG_PATH",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->config_path = null;
                $test_model->get_next_id();
            }
        );
    }

    /**
     * Checks that we cannot call the default get_next_id() method if the model object does not have `many` enabled.
     */
    public function test_model_cant_get_next_id_without_many() {
        $this->assert_throws_response(
            response_id: "MODEL_NEXT_ID_WITHOUT_MANY",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->many = false;
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
            response_id: "MODEL_DANGEROUS_CONFIG_PATH",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
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
        $test_model = new API\Models\Test();
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
        $test_model = new API\Models\Test();
        $test_model->config_path = "filter/rule";
        $test_model->many = true;
        $this->assert_is_greater_than($test_model->get_next_id(), 1);
    }

    /**
     * Checks that Models can correctly convert to their internal values.
     */
    public function test_model_to_internal() {
        # Since the default Model does not have any fields, use a Test Model to test this.
        $test_model = new API\Models\Test();
        $test_model->test_bool->value = true;
        $test_model->test_integer->value = 3;
        $test_model->test_string_unique->value = "test_unique_value";
        $test_model->test_string_many->value = ["a","b","c","d","e"];
        $test_model->test_string_namespace->value = "test string namespace value";
        $test_model->test_string_conditional->value = "Conditions were met so I will be present!";
        $test_model->validate();

        # Ensure the internal value of this object matches what is expected
        $this->assert_equals(
            $test_model->to_internal(),
            [
                $test_model->test_bool->internal_name => $test_model->test_bool->indicates_true,
                $test_model->test_integer->internal_name => strval($test_model->test_integer->value),
                $test_model->test_string_conditional->internal_name => $test_model->test_string_conditional->value,
                $test_model->test_string_unique->internal_name => $test_model->test_string_unique->value,
                $test_model->test_string_many->internal_name => implode(
                    $test_model->test_string_many->delimiter, $test_model->test_string_many->value
                ),
                $test_model->test_string_namespace->internal_namespace => [
                    $test_model->test_string_namespace->internal_name=>$test_model->test_string_namespace->value
                ]
            ]
        );
    }

    /**
     * Checks that the Model's get_fields() method properly finds Field objects and their assigned names.
     */
    public function test_model_get_fields() {
        # Use the Test model since the base Model has no fields assigned.
        $this->assert_equals(
            (new API\Models\Test())->get_fields(),
            [
                "test_bool",
                "test_integer",
                "test_string_conditional",
                "test_string_unique",
                "test_string_many",
                "test_string_namespace"
            ]
        );
    }

    /**
     * Checks to ensure Models cannot have Field objects with the same internal namespace and internal name.
     */
    public function test_model_field_internal_name_unique_to_namespace() {
        $this->assert_throws_response(
            response_id: "MODEL_FIELDS_WITH_CONFLICTING_INTERNAL_NAMES",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->test_bool->internal_namespace = "same_namespace";
                $test_model->test_bool->internal_name = "same_name";
                $test_model->test_string_unique->internal_namespace = "same_namespace";
                $test_model->test_string_unique->internal_name = "same_name";
                $test_model->validate();
            }
        );
    }

    /**
     * Checks to ensure the validate() method properly throws an error if a required pfSense package for the Model
     * is missing.
     */
    public function test_model_validate_packages_missing_package() {
        $this->assert_throws_response(
            response_id: "MDOEL_MISSING_REQUIRED_PACKAGE",
            code: 404,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->packages = ["pfSense-pkg-some_package_we_dont_have"];
                $test_model->validate();
            }
        );
    }

    /**
     * Checks to ensure the validate() method is successful when we have the correct packages installed.
     */
    public function test_model_validate_packages_installed_package() {
        $test_model = new API\Models\Test();
        $test_model->packages = ["pfSense-pkg-API"];    # Use pfSense-pkg-API since we know we have it.
        $test_model->validate();
    }

    /**
     * Checks to ensure the validate() method properly throws an error if we cannot include a required package include
     * file because it is not found in the PHP path.
     */
    public function test_model_validate_missing_package_includes() {
        $this->assert_throws_response(
            response_id: "MODEL_WITH_FAILED_INCLUDE",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->package_includes = ["this_inc_file_does_not_exist.inc"];
                $test_model->validate();
            }
        );
    }

    /**
     * Check default Model object `create()` method to ensure it throws an error if `many` is not set to `true`. Model
     * objects cannot be created unless the Model supports many objects OR the base Model class's `_create()` method
     * is overridden.
     */
    public function test_model_create_without_many() {
        $this->assert_throws_response(
            response_id: "MODEL_REQUIRE_OVERRIDDEN_CREATE_METHOD",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->many = false;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->create();
            }
        );
    }

    /**
     * Check default Model object `create()` method to ensure it throws an error if `config_path` is not set. Model
     * objects cannot be created unless the Model has a valid config path OR the base Model class's `_create()` method
     * is overridden.
     */
    public function test_model_create_without_config_path() {
        $this->assert_throws_response(
            response_id: "MODEL_REQUIRE_OVERRIDDEN_CREATE_METHOD",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->config_path = "";

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->create();
            }
        );
    }

    /**
     * Checks the default `create()` method correctly validates and writes the Model object to config.
     */
    public function test_model_create() {
        # Create a new test model object in config
        $test_model = new API\Models\Test();
        $test_model->test_bool->value = true;
        $test_model->test_string_unique->value ="test_model_create";
        $test_model->test_string_many->value = ["a", "b", "c"];
        $test_model->create();

        # Attempt to load the written object from config using it's ID and ensure the values match.
        $test_model_from_config = new API\Models\Test($test_model->id);
        $this->assert_equals(
            $test_model->test_bool->value,
            $test_model_from_config->test_bool->value
        );
        $this->assert_equals(
            $test_model->test_string_unique->value,
            $test_model_from_config->test_string_unique->value
        );
        $this->assert_equals(
            $test_model->test_string_many->value,
            $test_model_from_config->test_string_many->value
        );
        $test_model->delete();
    }

    /**
     * Check default Model object `update()` method to ensure it throws an error if `config_path` is not set. Model
     * objects cannot be updated unless the Model has a valid config path OR the base Model class's `_update()` method
     * is overridden.
     */
    public function test_model_update_without_config_path() {
        $this->assert_throws_response(
            response_id: "MODEL_REQUIRE_OVERRIDDEN_UPDATE_METHOD",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->config_path = "";
                $test_model->id = 0;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->update();
            }
        );
    }

    /**
     * Checks the deafult Model object `update()` method to ensure it throws an error if `many` is set but no `id`
     * value has been assigned. We must have an ID to determine which object is being updated.
     */
    public function test_model_update_without_id() {
        $this->assert_throws_response(
            response_id: "MODEL_REQUIRES_ID",
            code: 400,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->config_path = "test/path";
                $test_model->many = true;
                $test_model->update();
            }
        );
    }

    /**
     * Checks the default `update()` method correctly validates, updates and writes the Model object to config.
     */
    public function test_model_update() {
        # Create a new test model object in config
        $test_model = new API\Models\Test();
        $test_model->test_bool->value = true;
        $test_model->test_string_unique->value ="test_model_update initial value";
        $test_model->test_string_many->value = ["a", "b", "c"];
        $test_model->create();

        # Create a new test model object in config
        $test_model_to_update = new API\Models\Test($test_model->id);
        $test_model_to_update->test_bool->value = false;
        $test_model_to_update->test_string_unique->value ="test_model_update updated value";
        $test_model_to_update->test_string_many->value = ["c", "b", "a"];
        $test_model_to_update->update();

        # Attempt to load the written object from config using it's ID and ensure the values match.
        $test_model_from_config = new API\Models\Test($test_model_to_update->id);
        $this->assert_equals(
            $test_model_to_update->test_bool->value,
            $test_model_from_config->test_bool->value
        );
        $this->assert_equals(
            $test_model_to_update->test_string_unique->value,
            $test_model_from_config->test_string_unique->value
        );
        $this->assert_equals(
            $test_model_to_update->test_string_many->value,
            $test_model_from_config->test_string_many->value
        );
        $test_model->delete();
    }

    /**
     * Check default Model object `delete()` method to ensure it throws an error if `many` is not set to `true`. Model
     * objects cannot be deleted unless the Model supports many objects OR the base Model class's `_delete()` method
     * is overridden.
     */
    public function test_model_delete_without_many() {
        $this->assert_throws_response(
            response_id: "MODEL_REQUIRE_OVERRIDDEN_DELETE_METHOD",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->many = false;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->delete();
            }
        );
    }

    /**
     * Check default Model object `delete()` method to ensure it throws an error if `config_path` is not set. Model
     * objects cannot be deleted unless the Model has a valid config path OR the base Model class's `_delete()` method
     * is overridden.
     */
    public function test_model_delete_without_config_path() {
        $this->assert_throws_response(
            response_id: "MODEL_REQUIRE_OVERRIDDEN_DELETE_METHOD",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->config_path = "";
                $test_model->id = 0;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->delete();
            }
        );
    }

    /**
     * Checks the default Model object `delete()` method to ensure it throws an error if `many` is set but no `id`
     * value has been assigned. We must have an ID to determine which object is being deleted.
     */
    public function test_model_delete_without_id() {
        $this->assert_throws_response(
            response_id: "MODEL_REQUIRES_ID",
            code: 400,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->config_path = "test/path";
                $test_model->many = true;
                $test_model->delete();
            }
        );
    }

    /**
     * Checks that the default `delete()` method actually deletes the object from configuration.
     */
    public function test_model_delete() {
        # Ensure we cannot find the object from config after it's deleted.
        $this->assert_throws_response(
            response_id: "MODEL_OBJECT_NOT_FOUND",
            code: 404,
            callable: function () {
                # Create a new test model object in config
                $test_model = new API\Models\Test();
                $test_model->create();

                # Delete the object and try to load the object from config
                $test_model->delete();
                new API\Models\Test(id: $test_model->id);
            }
        );
    }

    /**
     * Checks that the Test Model's `apply()` method runs after creations, updates and deletion when requested. The
     * Test model's `apply()` method has been overridden to write a text file to /tmp/.api_model_test_apply.txt whenever
     * it is called. This test checks for that file after `create()`, `update()` or `delete()` is called.
     */
    public function test_model_calls_apply_when_requested() {
        # In case a test file was leftover, try to remove the test file first
        unlink("/tmp/.api_models_test_apply.txt");

        # Create a new test model object in config and request to apply the change immediately.
        $test_model = new API\Models\Test();
        $test_model->create(apply: true);

        # Ensure the expected txt file is present and remove it.
        $this->assert_is_true(file_exists("/tmp/.api_models_test_apply.txt"));
        unlink("/tmp/.api_models_test_apply.txt");

        # Update the model and ensure the txt file is present once again
        $test_model->update(apply: true);
        $this->assert_is_true(file_exists("/tmp/.api_models_test_apply.txt"));
        unlink("/tmp/.api_models_test_apply.txt");

        # Delete the model and ensure the txt file is present once again
        $test_model->delete(apply: true);
        $this->assert_is_true(file_exists("/tmp/.api_models_test_apply.txt"));
        unlink("/tmp/.api_models_test_apply.txt");
    }

    /**
     * Checks that all fields specified in `unique_together_fields` must be existing Fields assigned to the Model.
     */
    public function test_unique_together_fields() {
        $this->assert_throws_response(
            response_id: "MODEL_UNIQUE_TOGETHER_FIELDS_WITH_UNKNOWN_FIELD",
            code: 500,
            callable: function () {
                $test_model = new API\Models\Test();
                $test_model->unique_together_fields = ["this_field_doesnt_exist"];
                $test_model->validate();
            }
        );
    }

    /**
     * Checks the unique together validation correctly requires a group of Fields to be unique from all other Model
     * objects of its kind.
     */
    public function test_unique_together_validation() {
        # Create a Model object that uses the unique fields
        $original_model_obj = new API\Models\Test(
            representation_data: [
                "test_bool" => false,
                "test_string_namespace" => "This must be unique with `test_bool` being `false`",
                "test_integer" => 30
            ]
        );
        $original_model_obj->create();

        # Ensure we can't create another object with the same combination of values when unique_together_fields are set
        $this->assert_throws_response(
            response_id: "MODEL_FIELDS_NOT_UNIQUE_TOGETHER",
            code: 400,
            callable: function () {
                $non_unique_model_obj = new API\Models\Test(
                    representation_data: [
                        "test_bool" => false,
                        "test_string_namespace" => "This must be unique with `test_bool` being `false`",
                        "test_integer" => 30
                    ]
                );
                $non_unique_model_obj->unique_together_fields = ["test_bool", "test_string_namespace", "test_integer"];
                $non_unique_model_obj->create();
            }
        );

        # Delete the original Model object
        $original_model_obj->delete();
    }

    /**
     * Checks that we are able to construct a Model object with an array of representation data.
     */
    public function test_model_constructs_from_representation() {
        # Construct a new test model object using representation data
        $test_model = new API\Models\Test(
            representation_data: [
                "test_bool" => true,
                "test_string_unique" => "test_model_constructs_from_representation",
                "test_string_many" => ["a", "b", "c"]
            ]
        );

        # Ensure fields were properly set from the representation data
        $this->assert_equals(
            $test_model->test_bool->value,
            true
        );
        $this->assert_equals(
            $test_model->test_string_unique->value,
            "test_model_constructs_from_representation"
        );
        $this->assert_equals(
            $test_model->test_string_many->value,
            ["a", "b", "c"]
        );
    }

    /**
     * Checks that `to_openapi_schema()` correctly converts the Model object to an OpenAPI schema.
     */
    public function test_model_to_openapi_schema() {
        # Create a model to test with and generate it's OpenAPI schema
        $test_model = new API\Models\Test();
        $test_model_schema = $test_model->to_openapi_schema();

        # Ensure the schema type is an object
        $this->assert_equals($test_model_schema["type"], "object");

        # Ensure each property in the schema is a Field object's OpenAPI property
        foreach ($test_model_schema["properties"] as $field => $property) {
            $this->assert_equals($test_model->$field->to_openapi_property(), $property);
        }
    }
}

