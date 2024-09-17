<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Models\FirewallAlias;

class APIFieldsNestedModelFieldTestCase extends TestCase {
    /**
     * Ensures NestedModelField objects are constructed with the necessary properties.
     */
    public function test_construct() {
        # Create a NestedModelObject to test with
        $field = new NestedModelField('FirewallAlias', required: true);

        # Ensure the nested Model object is created and matches the assigned `model_class`
        $this->assert_equals($field->model_class, '\\RESTAPI\\Models\\FirewallAlias');
        $this->assert_equals($field->model_class, '\\' . get_class($field->model));

        # Ensure this field inherits its `many` value from the nested Model's `many` value
        $this->assert_equals($field->many, $field->model->many);

        # Ensure the internal_name and internal_namespace derive from the nested Model's config path
        $this->assert_equals($field->internal_name, 'alias');
        $this->assert_equals($field->internal_namespace, 'aliases');
    }

    /**
     * Ensures that the NestedModelField `validate_extra()` method performs the nested Model's `validate()` method.
     */
    public function test_nested_validate() {
        # Create a NestedModelObject to test with
        $field = new NestedModelField('FirewallAlias', required: true);

        # Leave out the required `name` parameter and ensure the NestedModelField `validate_extra()` fails
        $this->assert_throws_response(
            response_id: 'FIELD_IS_REQUIRED',
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra(['not_name' => 'test']);
            },
        );

        # Set an invalid `type` choice and ensure the NestedModelField catches it
        $this->assert_throws_response(
            response_id: 'FIELD_INVALID_CHOICE',
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra(['name' => 'test', 'type' => 'not an alias type']);
            },
        );
    }

    /**
     * Ensures `unique` validation works for NestedModelFields.
     */
    public function test_unique_validation() {
        # Create a NestedModelObject to test with
        $field = new NestedModelField('FirewallAlias', required: true);
        $field->name = 'test';

        $this->assert_throws_response(
            response_id: 'FIELD_MUST_BE_UNIQUE',
            code: 400,
            callable: function () use ($field) {
                $field->value = [
                    ['name' => 'thismustbeunique', 'type' => 'host'],
                    ['name' => 'thismustbeunique', 'type' => 'host'],
                ];
                $field->validate();
            },
        );
    }

    /**
     * Ensures `unique_together` validation works for NestedModelFields.
     */
    public function test_unique_together_validation() {
        # Create a NestedModelObject to test with. Use the DNSResolverHostOverride model for this test since its `host`
        # and `domain` Fields must be unique together.
        $field = new NestedModelField('DNSResolverHostOverride', required: true);
        $field->name = 'test';

        $this->assert_throws_response(
            response_id: 'MODEL_FIELDS_NOT_UNIQUE_TOGETHER',
            code: 400,
            callable: function () use ($field) {
                $field->value = [
                    ['host' => 'mustbeuniquewithdomain', 'domain' => 'mustbeuniquewithhost', 'ip' => ['127.0.0.1']],
                    ['host' => 'mustbeuniquewithdomain', 'domain' => 'mustbeuniquewithhost', 'ip' => ['127.0.0.2']],
                ];
                $field->validate();
            },
        );
    }

    /**
     * Ensures nested model objects are correctly converted to their internal Forms.
     */
    public function test_to_internal() {
        # Create a NestedModelObject to test with.
        $field = new NestedModelField('FirewallAlias', required: true);
        $field->name = 'test';

        # Create a direct FirewallAlias object to compare with
        $alias = new FirewallAlias(data: ['name' => 'testalias', 'type' => 'host', 'address' => ['127.0.0.1']]);
        $alias->validate();

        # Add the $alias representation to the $field values and ensure they match when `to_internal()` runs.
        $field->value = [$alias->to_representation()];
        $field->validate();
        $this->assert_equals([$alias->to_internal()], $field->to_internal());
    }

    /**
     * Ensures nested model objects are correctly converted from their internal Forms.
     */
    public function test_from_internal() {
        # Create a NestedModelObject to test with.
        $field = new NestedModelField('FirewallAlias', required: true);
        $field->name = 'test';

        # Create a direct FirewallAlias object to compare with
        $alias = new FirewallAlias(data: ['name' => 'testalias', 'type' => 'host', 'address' => ['127.0.0.1']]);
        $alias->create();

        # Set the value from the internal value of $alias and ensure it matches the representation value afterward
        $field->from_internal([$alias->to_internal()]);
        $this->assert_equals([$alias->to_representation()], $field->value);

        # Delete the FirewallAlias used for testing
        $alias->delete();
    }

    /**
     * Ensures the `sort()` method correctly sorts values based on the nested Model's sort criteria.
     */
    public function test_sort(): void {
        # Create a NestedModelObject to test with.
        $field = new NestedModelField('FirewallAlias', required: true);
        $field->name = 'test';

        # Assign the nested Model sort criteria for testing - by field `name` in alphabetical order
        $field->model->sort_by = 'name';
        $field->model->sort_option = SORT_ASC;

        # Set the value to an array of alias objects that are out of order
        $field->value = [
            ['name' => 'ccc', 'type' => 'host'],
            ['name' => 'aaa', 'type' => 'port'],
            ['name' => 'ddd', 'type' => 'host'],
            ['name' => 'bbb', 'type' => 'network'],
        ];

        # Ensure we validate the value to populate the ModelSet
        $field->validate();

        # Sort the values and ensure they are now placed in alphabetical order
        $field->sort();
        $this->assert_equals($field->value, [
            ['id' => 0, 'name' => 'aaa', 'type' => 'port', 'descr' => '', 'address' => [], 'detail' => []],
            ['id' => 1, 'name' => 'bbb', 'type' => 'network', 'descr' => '', 'address' => [], 'detail' => []],
            ['id' => 2, 'name' => 'ccc', 'type' => 'host', 'descr' => '', 'address' => [], 'detail' => []],
            ['id' => 3, 'name' => 'ddd', 'type' => 'host', 'descr' => '', 'address' => [], 'detail' => []],
        ]);

        # Sort again but this time in descending order
        $field->model->sort_option = SORT_DESC;
        $field->sort();
        $this->assert_equals($field->value, [
            ['id' => 0, 'name' => 'ddd', 'type' => 'host', 'descr' => '', 'address' => [], 'detail' => []],
            ['id' => 1, 'name' => 'ccc', 'type' => 'host', 'descr' => '', 'address' => [], 'detail' => []],
            ['id' => 2, 'name' => 'bbb', 'type' => 'network', 'descr' => '', 'address' => [], 'detail' => []],
            ['id' => 3, 'name' => 'aaa', 'type' => 'port', 'descr' => '', 'address' => [], 'detail' => []],
        ]);
    }
}
