<?php

namespace RESTAPI\Tests;

use ReflectionClass;
use ReflectionException;
use RESTAPI;
use RESTAPI\Caches\RESTAPIVersionReleasesCache;
use RESTAPI\Core\Model;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\SystemStatus;
use RESTAPI\Models\Test;

class APICoreModelTestCase extends RESTAPI\Core\TestCase {
    /**
     * Checks that a Model object cannot be created when both an ID and representation data are provided. This
     * ensures there is no confusion about which object ID to use when obtaining the model from its internal value.
     */
    public function test_no_model_with_id_and_representation_data() {
        $this->assert_throws_response(
            response_id: 'MODEL_WITH_ID_AND_REPRESENTATION',
            code: 500,
            callable: function () {
                new Model(id: 0, data: ['id' => 0]);
            },
        );
    }

    /**
     * Checks that we are able to pass in an RESTAPI\Core\Auth object to relay external client information like username
     * and IP address. This is mostly commonly used by RESTAPI\Core\Endpoint to provide Models information about the client
     * that authenticated a REST API call.
     */
    public function test_model_allows_provided_auth_obj() {
        # Create an RESTAPI\Core\Auth object with custom values.
        $test_client = new RESTAPI\Core\Auth();
        $test_client->username = 'test_user';
        $test_client->ip_address = '1.2.3.4';

        # Create a Model object and provide the Auth object
        $test_model = new Test(client: $test_client);

        # Ensure the Auth object was accepted and is using the correct values
        $this->assert_equals($test_model->client->username, $test_client->username);
        $this->assert_equals($test_model->client->ip_address, $test_client->ip_address);
    }

    /**
     * Checks that a default RESTAPI\Core\Auth object is created when a Model object is created if none was provided.
     */
    public function test_model_default_auth_obj() {
        # Create a Model object and but do not provide an Auth object
        $test_model = new Test();

        # Ensure the Auth object was accepted and is using the correct values
        $this->assert_equals($test_model->client->username, RESTAPI\Core\DEFAULT_CLIENT_USERNAME);
        $this->assert_equals($test_model->client->ip_address, RESTAPI\Core\DEFAULT_CLIENT_IP_ADDRESS);
    }

    /**
     * Checks the Model's default `verbose_name` and `verbose_name_plural` are properly derived from its class name.
     */
    public function test_model_default_verbose_name() {
        # Create a normal Model object and a FirewallAlias Model object for testing
        $test_model = new Test();
        $test_fw_alias = new FirewallAlias();

        # Ensure their default names are correct
        $this->assert_equals($test_model->verbose_name, 'Test');
        $this->assert_equals($test_model->verbose_name_plural, 'Tests');
        $this->assert_equals($test_fw_alias->verbose_name, 'Firewall Alias');
        $this->assert_equals($test_fw_alias->verbose_name_plural, 'Firewall Aliases');
    }

    /**
     * Checks that we are able to read items from the internal pfSense configuration by path.
     */
    public function test_model_get_config_valid_path() {
        # Try to read the WAN interface configuration since it is always present in the config
        $this->assert_type(Model::get_config('interfaces/wan'), 'array');
    }

    /**
     * Checks that a specified default value is returned if the config path does not lead to an existing value.
     */
    public function test_model_get_config_bad_path() {
        # Try to read the WAN interface configuration since it is always present in the config
        $this->assert_equals(Model::get_config('does/not/exists', []), []);
        $this->assert_equals(Model::get_config('does/not/exists', false), false);
        $this->assert_equals(Model::get_config('does/not/exists'), null);
    }

    /**
     * Checks that we cannot call the default get_next_id() method if the model object does not have a config path set.
     */
    public function test_model_cant_get_next_id_without_config_path() {
        $this->assert_throws_response(
            response_id: 'MODEL_NEXT_ID_WITHOUT_CONFIG_PATH',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->config_path = null;
                $test_model->get_next_id();
            },
        );
    }

    /**
     * Checks that we cannot call the default get_next_id() method if the model object does not have `many` enabled.
     */
    public function test_model_cant_get_next_id_without_many() {
        $this->assert_throws_response(
            response_id: 'MODEL_NEXT_ID_WITHOUT_MANY',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->many = false;
                $test_model->get_next_id();
            },
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
            response_id: 'MODEL_DANGEROUS_CONFIG_PATH',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->config_path = 'system/hostname';
                $test_model->many = true;
                $test_model->get_next_id();
            },
        );
    }

    /**
     * Checks that we can successfully obtain the next ID for a non-existing config path. Empty paths should always
     * return the first available ID, `0`.
     */
    public function test_model_can_get_next_id_at_empty_path() {
        $test_model = new Test();
        $test_model->config_path = 'this/path/does/not/exist/yet';
        $test_model->many = true;
        $this->assert_equals($test_model->get_next_id(), 0);
    }

    /**
     * Checks that we can successfully obtain the next ID for an existing config path if it's an array. This test
     * checks that the ID returned for filter/rule is greater than 0 because it's generally safe to assume there
     * will always be at least one item in filter/rule.
     */
    public function test_model_can_get_next_id_from_existing_array_path() {
        $test_model = new Test();
        $test_model->config_path = 'filter/rule';
        $test_model->many = true;
        $this->assert_is_greater_than($test_model->get_next_id(), 1);
    }

    /**
     * Checks that Models can correctly convert to their internal values.
     */
    public function test_model_to_internal() {
        # Since the default Model does not have any Fields, use a Test Model to test this.
        $test_model = new Test();
        $test_model->test_bool->value = true;
        $test_model->test_integer->value = 3;
        $test_model->test_string_unique->value = 'test_unique_value';
        $test_model->test_string_many->value = ['a', 'b', 'c', 'd', 'e'];
        $test_model->test_string_namespace->value = 'test string namespace value';
        $test_model->test_string_conditional->value = 'Conditions were met so I will be present!';
        $test_model->validate();

        # Ensure the internal value of this object matches what is expected
        $this->assert_equals($test_model->to_internal(), [
            $test_model->test_bool->internal_name => $test_model->test_bool->indicates_true,
            $test_model->test_integer->internal_name => strval($test_model->test_integer->value),
            $test_model->test_string_conditional->internal_name => $test_model->test_string_conditional->value,
            $test_model->test_string_unique->internal_name => $test_model->test_string_unique->value,
            $test_model->test_string_many->internal_name => implode(
                $test_model->test_string_many->delimiter,
                $test_model->test_string_many->value,
            ),
            $test_model->test_string_namespace->internal_namespace => [
                $test_model->test_string_namespace->internal_name => $test_model->test_string_namespace->value,
            ],
        ]);
    }

    /**
     * Checks that the Model's get_fields() method properly finds Field objects and their assigned names.
     */
    public function test_model_get_fields() {
        # Use the Test model since the base Model has no Fields assigned.
        $this->assert_equals((new Test())->get_fields(), [
            'test_bool',
            'test_integer',
            'test_string_conditional',
            'test_string_unique',
            'test_string_many',
            'test_string_namespace',
        ]);
    }

    /**
     * Checks to ensure Models cannot have Field objects with the same internal namespace and internal name.
     */
    public function test_model_field_internal_name_unique_to_namespace() {
        $this->assert_throws_response(
            response_id: 'MODEL_FIELDS_WITH_CONFLICTING_INTERNAL_NAMES',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->test_bool->internal_namespace = 'same_namespace';
                $test_model->test_bool->internal_name = 'same_name';
                $test_model->test_string_unique->internal_namespace = 'same_namespace';
                $test_model->test_string_unique->internal_name = 'same_name';
                $test_model->validate();
            },
        );
    }

    /**
     * Checks to ensure the validate() method properly throws an error if a required pfSense package for the Model
     * is missing.
     */
    public function test_model_validate_packages_missing_package() {
        $this->assert_throws_response(
            response_id: 'MODEL_MISSING_REQUIRED_PACKAGE',
            code: 404,
            callable: function () {
                $test_model = new Test();
                $test_model->packages = ['pfSense-pkg-some_package_we_dont_have'];
                $test_model->validate();
            },
        );
    }

    /**
     * Checks to ensure the validate() method is successful when we have the correct packages installed.
     */
    public function test_model_validate_packages_installed_package() {
        $test_model = new Test();
        $test_model->packages = ['pfSense-pkg-RESTAPI']; # Use pfSense-pkg-RESTAPI since we know we have it.
        $test_model->validate();
    }

    /**
     * Checks to ensure the validate() method properly throws an error if we cannot include a required package include
     * file because it is not found in the PHP path.
     */
    public function test_model_validate_missing_package_includes() {
        $this->assert_throws_response(
            response_id: 'MODEL_WITH_FAILED_INCLUDE',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->package_includes = ['this_inc_file_does_not_exist.inc'];
                $test_model->validate();
            },
        );
    }

    /**
     * Check default Model object `create()` method to ensure it throws an error if `many` is not set to `true`. Model
     * objects cannot be created unless the Model supports many objects OR the base Model class's `_create()` method
     * is overridden.
     */
    public function test_model_create_without_many() {
        $this->assert_throws_response(
            response_id: 'MODEL_REQUIRE_OVERRIDDEN_CREATE_METHOD',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->many = false;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->create();
            },
        );
    }

    /**
     * Check default Model object `create()` method to ensure it throws an error if `config_path` is not set. Model
     * objects cannot be created unless the Model has a valid config path OR the base Model class's `_create()` method
     * is overridden.
     */
    public function test_model_create_without_config_path() {
        $this->assert_throws_response(
            response_id: 'MODEL_REQUIRE_OVERRIDDEN_CREATE_METHOD',
            code: 500,
            callable: function () {
                $test_model = new Test();

                # We need to use an mock internal callable to test this for creates, otherwise the
                # Model::check_many_minimum() method will throw an error about no internal callable being set
                $test_model->config_path = null;
                $test_model->internal_callable = 'test_internal_callable';

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->create();
            },
        );
    }

    /**
     * Checks the default `create()` method correctly validates and writes the Model object to config.
     */
    public function test_model_create() {
        # Create a new test model object in config
        $test_model = new Test();
        $test_model->test_bool->value = true;
        $test_model->test_string_unique->value = 'test_model_create';
        $test_model->test_string_many->value = ['a', 'b', 'c'];
        $test_model->create();

        # Attempt to load the written object from config using it's ID and ensure the values match.
        $test_model_from_config = new Test($test_model->id);
        $this->assert_equals($test_model->test_bool->value, $test_model_from_config->test_bool->value);
        $this->assert_equals(
            $test_model->test_string_unique->value,
            $test_model_from_config->test_string_unique->value,
        );
        $this->assert_equals($test_model->test_string_many->value, $test_model_from_config->test_string_many->value);
        $test_model->delete();
    }

    /**
     * Check default Model object `update()` method to ensure it throws an error if `config_path` is not set. Model
     * objects cannot be updated unless the Model has a valid config path OR the base Model class's `_update()` method
     * is overridden.
     */
    public function test_model_update_without_config_path() {
        $this->assert_throws_response(
            response_id: 'MODEL_REQUIRE_OVERRIDDEN_UPDATE_METHOD',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->config_path = '';
                $test_model->id = 0;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->update();
            },
        );
    }

    /**
     * Checks the deafult Model object `update()` method to ensure it throws an error if `many` is set but no `id`
     * value has been assigned. We must have an ID to determine which object is being updated.
     */
    public function test_model_update_without_id() {
        $this->assert_throws_response(
            response_id: 'MODEL_REQUIRES_ID',
            code: 400,
            callable: function () {
                $test_model = new Test();
                $test_model->config_path = 'test/path';
                $test_model->many = true;
                $test_model->update();
            },
        );
    }

    /**
     * Checks the default `update()` method correctly validates, updates and writes the Model object to config.
     */
    public function test_model_update() {
        # Create a new test model object in config
        $test_model = new Test();
        $test_model->test_bool->value = true;
        $test_model->test_string_unique->value = 'test_model_update initial value';
        $test_model->test_string_many->value = ['a', 'b', 'c'];
        $test_model->create();

        # Create a new test model object in config
        $test_model_to_update = new Test($test_model->id);
        $test_model_to_update->test_bool->value = false;
        $test_model_to_update->test_string_unique->value = 'test_model_update updated value';
        $test_model_to_update->test_string_many->value = ['c', 'b', 'a'];
        $test_model_to_update->update();

        # Attempt to load the written object from config using it's ID and ensure the values match.
        $test_model_from_config = new Test($test_model_to_update->id);
        $this->assert_equals($test_model_to_update->test_bool->value, $test_model_from_config->test_bool->value);
        $this->assert_equals(
            $test_model_to_update->test_string_unique->value,
            $test_model_from_config->test_string_unique->value,
        );
        $this->assert_equals(
            $test_model_to_update->test_string_many->value,
            $test_model_from_config->test_string_many->value,
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
            response_id: 'MODEL_REQUIRE_OVERRIDDEN_DELETE_METHOD',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->many = false;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->delete();
            },
        );
    }

    /**
     * Check default Model object `delete()` method to ensure it throws an error if `config_path` is not set. Model
     * objects cannot be deleted unless the Model has a valid config path OR the base Model class's `_delete()` method
     * is overridden.
     */
    public function test_model_delete_without_config_path() {
        $this->assert_throws_response(
            response_id: 'MODEL_REQUIRE_OVERRIDDEN_DELETE_METHOD',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->config_path = '';
                $test_model->id = 0;

                # Prevent this unique field from throwing an error first
                $test_model->test_string_unique->unique = false;

                $test_model->delete();
            },
        );
    }

    /**
     * Checks the default Model object `delete()` method to ensure it throws an error if `many` is set but no `id`
     * value has been assigned. We must have an ID to determine which object is being deleted.
     */
    public function test_model_delete_without_id() {
        $this->assert_throws_response(
            response_id: 'MODEL_REQUIRES_ID',
            code: 400,
            callable: function () {
                $test_model = new Test();
                $test_model->config_path = 'test/path';
                $test_model->many = true;
                $test_model->delete();
            },
        );
    }

    /**
     * Checks that the default `delete()` method actually deletes the object from configuration.
     */
    public function test_model_delete() {
        # Ensure we cannot find the object from config after it's deleted.
        $this->assert_throws_response(
            response_id: 'MODEL_OBJECT_NOT_FOUND',
            code: 404,
            callable: function () {
                # Create a new test model object in config
                $test_model = new Test();
                $test_model->create();

                # Delete the object and try to load the object from config
                $test_model->delete();
                new Test(id: $test_model->id);
            },
        );
    }

    /**
     * Checks that the Test Model's `apply()` method runs after creations, updates and deletion when requested. The
     * Test model's `apply()` method has been overridden to write a text file to /tmp/.api_model_test_apply.txt whenever
     * it is called. This test checks for that file after `create()`, `update()` or `delete()` is called.
     */
    public function test_model_calls_apply_when_requested() {
        # In case a test file was leftover, try to remove the test file first
        unlink('/tmp/.api_models_test_apply.txt');

        # Create a new test model object in config and request to apply the change immediately.
        $test_model = new Test();
        $test_model->create(apply: true);

        # Ensure the expected txt file is present and remove it.
        $this->assert_is_true(file_exists('/tmp/.api_models_test_apply.txt'));
        unlink('/tmp/.api_models_test_apply.txt');

        # Update the model and ensure the txt file is present once again
        $test_model->update(apply: true);
        $this->assert_is_true(file_exists('/tmp/.api_models_test_apply.txt'));
        unlink('/tmp/.api_models_test_apply.txt');

        # Delete the model and ensure the txt file is present once again
        $test_model->delete(apply: true);
        $this->assert_is_true(file_exists('/tmp/.api_models_test_apply.txt'));
        unlink('/tmp/.api_models_test_apply.txt');
    }

    /**
     * Checks that all Fields specified in `unique_together_fields` must be existing Fields assigned to the Model.
     */
    public function test_unique_together_fields() {
        $this->assert_throws_response(
            response_id: 'MODEL_UNIQUE_TOGETHER_FIELDS_WITH_UNKNOWN_FIELD',
            code: 500,
            callable: function () {
                $test_model = new Test();
                $test_model->unique_together_fields = ['this_field_doesnt_exist'];
                $test_model->validate();
            },
        );
    }

    /**
     * Checks the unique together validation correctly requires a group of Fields to be unique from all other Model
     * objects of its kind.
     */
    public function test_unique_together_validation() {
        # Create a Model object that uses the unique Fields
        $original_model_obj = new Test(
            data: [
                'test_bool' => false,
                'test_string_namespace' => 'This must be unique with `test_bool` being `false`',
                'test_integer' => 30,
            ],
        );
        $original_model_obj->create();

        # Ensure we can't create another object with the same combination of values when unique_together_fields are set
        $this->assert_throws_response(
            response_id: 'MODEL_FIELDS_NOT_UNIQUE_TOGETHER',
            code: 400,
            callable: function () {
                $non_unique_model_obj = new Test(
                    data: [
                        'test_bool' => false,
                        'test_string_namespace' => 'This must be unique with `test_bool` being `false`',
                        'test_integer' => 30,
                    ],
                );
                $non_unique_model_obj->unique_together_fields = ['test_bool', 'test_string_namespace', 'test_integer'];
                $non_unique_model_obj->create();
            },
        );

        # Delete the original Model object
        $original_model_obj->delete();
    }

    /**
     * Checks that we are able to construct a Model object with an array of representation data.
     */
    public function test_model_constructs_from_representation() {
        # Construct a new test model object using representation data
        $test_model = new Test(
            data: [
                'test_bool' => true,
                'test_string_unique' => 'test_model_constructs_from_representation',
                'test_string_many' => ['a', 'b', 'c'],
            ],
        );

        # Ensure Fields were properly set from the representation data
        $this->assert_equals($test_model->test_bool->value, true);
        $this->assert_equals($test_model->test_string_unique->value, 'test_model_constructs_from_representation');
        $this->assert_equals($test_model->test_string_many->value, ['a', 'b', 'c']);
    }

    /**
     * Checks that `to_openapi_schema()` correctly converts the Model object to an OpenAPI schema.
     */
    public function test_model_to_openapi_schema() {
        # Create a model to test with and generate it's OpenAPI schema
        $test_model = new Test();
        $test_model_schema = $test_model->to_openapi_schema();

        # Ensure the schema type is an object
        $this->assert_equals($test_model_schema['type'], 'object');

        # Ensure each property in the schema is a Field object's OpenAPI property
        foreach ($test_model_schema['properties'] as $field => $property) {
            $this->assert_equals($test_model->$field->to_openapi_property(), $property);
        }
    }

    /**
     * Checks that the Models with a $cache_class assigned are construct with a specific Cache object.
     */
    public function test_cache_class_creates_cache_object_during_construction(): void {
        # Define a model with a cache class. Use the RESTAPIVersionReleasesCache for testing.
        $model = new Model();
        $model->cache_class = 'RESTAPIVersionReleasesCache';
        $model->__construct();

        # Ensure the $cache property now has the Cache object
        $this->assert_is_true($model->cache instanceof RESTAPIVersionReleasesCache);

        # Remove the $cache_class and ensure the $cache property is null after construction
        $model->cache_class = '';
        $model->__construct();
        $this->assert_equals($model->cache, null);
    }

    /**
     * Checks that the Model's $placement property correctly sets the placement of the Model object in the pfSense
     * configuration during creation and updates.
     */
    public function test_set_placement(): void {
        # Create three FirewallAlias models in order
        $alias_0 = new FirewallAlias(name: 'test0', type: 'host');
        $alias_0->create();
        $alias_1 = new FirewallAlias(name: 'test1', type: 'host');
        $alias_1->create();
        $alias_2 = new FirewallAlias(name: 'test2', type: 'host');
        $alias_2->create();

        # Create another FirewallAlias model but set it's placement to 1
        $alias_3 = new FirewallAlias(name: 'test3', type: 'host', placement: 1);
        $alias_3->create();

        # Ensure the alias with ID 1 is now $alias_4 instead of $alias_1 due to the placement
        $this->assert_equals($alias_3->id, 1);
        $this->assert_equals(FirewallAlias::query(id: 1)->first()->name->value, $alias_3->name->value);

        # Update $alias_4 to set it's position to 4
        $alias_3->placement = 3;
        $alias_3->update();

        # Ensure the alias with ID 3 is now $alias_3
        $this->assert_equals($alias_3->id, 3);
        $this->assert_equals(FirewallAlias::query(id: 3)->first()->name->value, $alias_3->name->value);

        # Delete all the aliases
        $alias_0->delete();
        $alias_1->delete();
        $alias_2->delete();
        $alias_3->delete();
    }

    /**
     * Checks that the get_all_model_classes() method correctly returns all Model classes that extend the Model class.
     */
    public function test_get_all_model_classes(): void {
        # Variables
        $class_fqns = Model::get_all_model_classes();
        $class_shortnames = Model::get_all_model_classes(shortnames: true);

        # Get all Model classes and ensure they are all instances of the Model class
        foreach ($class_fqns as $index => $model_class) {
            $class_obj = new $model_class();
            $this->assert_is_true(is_subclass_of($class_obj, Model::class));
            $this->assert_equals($class_obj->get_class_shortname(), $class_shortnames[$index]);
        }
    }

    /**
     * Checks that each Model class in /usr/local/pkg/RESTAPI/Models/ has the correct constructor signature.
     */
    public function test_model_constructor_signature(): void {
        # Variables
        $class_fqns = Model::get_all_model_classes();

        # Ensure each Model class has the correct constructor signature
        foreach ($class_fqns as $model_class) {
            # Get the constructor parameters
            $reflection = new ReflectionClass($model_class);
            $constructor = $reflection->getConstructor();
            $parameters = $constructor->getParameters();
            $param_names = array_map(function ($param) {
                return $param->getName();
            }, $parameters);

            # Ensure all required parameters are present
            $this->assert_is_true(in_array('id', $param_names), "$model_class is missing 'id' parameter");
            $this->assert_is_true(in_array('parent_id', $param_names), "$model_class is missing 'parent_id' parameter");
            $this->assert_is_true(in_array('data', $param_names), "$model_class is missing 'data' parameter");
            $this->assert_is_true(in_array('options', $param_names), "$model_class is missing 'options' parameter");
        }
    }

    /**
     * Checks that the skip_init property correctly skips loading a Model's values from internal in the Model's
     * __construct() method when set to true.
     */
    public function test_skip_init(): void {
        # Create a new Test model object with skip_init set to true
        $test_model = new SystemStatus(skip_init: true);

        # Ensure the object's values are not loaded from internal
        foreach ($test_model->get_fields() as $field) {
            $this->assert_equals($test_model->$field->value, null);
        }

        # Ensure the object's values are loaded from internal when skip_init is set to false
        $test_model = new SystemStatus(skip_init: false);
        $this->assert_is_not_empty($test_model->cpu_load_avg->value);
    }

    /**
     * Checks that the Model::from_internal_object() correctly loads the Model object using the internal configuration
     * values.
     */
    public function check_from_internal_object(): void {
        # Use a FirewallAlias model to test this
        $alias = new FirewallAlias();

        # Load the FirewallAlias object's properties using an internal config and ensure it was loaded successfully
        $alias->from_internal_object(
            internal_object: [
                'name' => 'test_alias',
                'type' => 'host',
                'address' => '1.2.3.4',
                'detail' => 'Example',
            ],
        );
        $this->assert_equals($alias->name->value, 'test_alias');
        $this->assert_equals($alias->type->value, 'host');
        $this->assert_equals($alias->address->value, ['1.2.3.4']);
        $this->assert_equals($alias->detail->value, ['Example']);

        # Ensure attempts to load a non-array internal object just results in the default values being loaded
        $alias = new FirewallAlias();
        $alias->from_internal_object('');
        $this->assert_equals($alias->name->value, null);
        $this->assert_equals($alias->type->value, null);
        $this->assert_equals($alias->address->value, []);
        $this->assert_equals($alias->detail->value, []);
    }

    /**
     * Checks that the 'delete_many()' method correctly deletes all Model objects that match the query.
     */
    public function test_delete_many(): void {
        # Create FirewallAlias models
        $host_alias_0 = new FirewallAlias(name: 'host0', type: 'host');
        $host_alias_0->create();
        $host_alias_1 = new FirewallAlias(name: 'host1', type: 'host');
        $host_alias_1->create();
        $host_alias_2 = new FirewallAlias(name: 'host2', type: 'host');
        $host_alias_2->create();
        $port_alias_0 = new FirewallAlias(name: 'port0', type: 'port');
        $port_alias_0->create();
        $port_alias_1 = new FirewallAlias(name: 'port1', type: 'port');
        $port_alias_1->create();
        $port_alias_2 = new FirewallAlias(name: 'port2', type: 'port');
        $port_alias_2->create();

        # Delete all the aliases with type 'port'
        $deleted_aliases = FirewallAlias::delete_many(type: 'port');

        # Loop through the aliases that were deleted and ensure they correctly matched the query and were really deleted
        foreach ($deleted_aliases->model_objects as $deleted_alias) {
            $this->assert_equals($deleted_alias->type->value, 'port');
            $this->assert_is_false(FirewallAlias::query(name: $deleted_alias->name->value)->exists());
        }

        # Ensure the host aliases were not deleted
        $this->assert_equals(FirewallAlias::query(name__startswith: 'host')->count(), 3);
    }

    /**
     * Checks that the 'delete_many()' method throws an error if no query parameters were given.
     */
    public function test_delete_many_no_query(): void {
        $this->assert_throws_response(
            response_id: 'MODEL_DELETE_MANY_REQUIRES_QUERY_PARAMS',
            code: 400,
            callable: function () {
                FirewallAlias::delete_many();
            },
        );
    }

    /**
     * Checks that the 'paginate()' method works correctly.
     */
    public function test_paginate(): void {
        # Ensure the paginate method correctly returns the paginated subset of a given array
        $this->assert_equals(Model::paginate([1, 2, 3, 4, 5], limit: 2, offset: 1), [2, 3]);
        $this->assert_equals(Model::paginate([1, 2, 3, 4, 5], limit: 2, offset: 3), [4, 5]);
        $this->assert_equals(Model::paginate([1, 2, 3, 4, 5], limit: 2, offset: 5), []);
        $this->assert_equals(Model::paginate([1, 2, 3, 4, 5], limit: 4, offset: 0), [1, 2, 3, 4]);
        $this->assert_equals(Model::paginate([1, 2, 3, 4, 5], limit: 4, offset: 1), [2, 3, 4, 5]);

        # Ensure a limit of 0 imposes no limit
        $this->assert_equals(Model::paginate([1, 2, 3, 4, 5], 0, 0), [1, 2, 3, 4, 5]);
    }

    /**
     * Ensures that queries are performed before pagination is applied.
     */
    public function test_paginate_with_query(): void {
        # Create FirewallAlias models to test this
        $alias_alias_0 = new FirewallAlias(name: 'alias0', type: 'host');
        $alias_alias_0->create();
        $alias_alias_1 = new FirewallAlias(name: 'alias1', type: 'port');
        $alias_alias_1->create();
        $alias_alias_2 = new FirewallAlias(name: 'alias2', type: 'network');
        $alias_alias_2->create();
        $alias_alias_3 = new FirewallAlias(name: 'alias3', type: 'host');
        $alias_alias_3->create();
        $alias_alias_4 = new FirewallAlias(name: 'alias4', type: 'port');
        $alias_alias_4->create();

        # Query for only port aliases, but limit it to 2
        $port_aliases = FirewallAlias::query(limit: 2, type: 'port');
        $this->assert_equals($port_aliases->count(), 2);
        $this->assert_equals($port_aliases->model_objects[0]->name->value, 'alias1');
        $this->assert_equals($port_aliases->model_objects[1]->name->value, 'alias4');

        # Query again but change to offset to exclude the first port alias
        $port_aliases = FirewallAlias::query(limit: 2, offset: 1, type: 'port');
        $this->assert_equals($port_aliases->count(), 1);
        $this->assert_equals($port_aliases->model_objects[0]->name->value, 'alias4');
    }
}
