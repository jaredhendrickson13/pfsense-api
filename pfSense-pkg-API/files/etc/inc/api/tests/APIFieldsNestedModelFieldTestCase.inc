<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Fields\NestedModelField;

class APIFieldsNestedModelFieldTestCase extends TestCase
{
    /**
     * Ensures NestedModelField objects are constructed with the necessary properties.
     */
    public function test_construct() {
        # Create a NestedModelObject to test with
        $field = new NestedModelField("FirewallAlias", required: true);

        # Ensure the nested Model object is created and matches the assigned `model_class`
        $this->assert_equals($field->model_class, "\\API\\Models\\FirewallAlias");
        $this->assert_equals($field->model_class, "\\".get_class($field->model));

        # Ensure this field inherits its `many` value from the nested Model's `many` value
        $this->assert_equals($field->many, $field->model->many);

        # Ensure the internal_name and internal_namespace derive from the nested Model's config path
        $this->assert_equals($field->internal_name, "alias");
        $this->assert_equals($field->internal_namespace, "aliases");
    }

    /**
     * Ensures that the NestedModelField `validate_extra()` method performs the nested Model's `validate()` method.
     */
    public function test_nested_validate() {
        # Create a NestedModelObject to test with
        $field = new NestedModelField("FirewallAlias", required: true);

        # Leave out the required `name` parameter and ensure the NestedModelField `validate_extra()` fails
        $this->assert_throws_response(
            response_id: "FIELD_IS_REQUIRED",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra(["not_name" => "test"]);
            }
        );

        # Set an invalid `type` choice and ensure the NestedModelField catches it
        $this->assert_throws_response(
            response_id: "FIELD_INVALID_CHOICE",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra(["name" => "test", "type" => "not an alias type"]);
            }
        );
    }

    /**
     * Ensures `unique` validation works for NestedModelFields.
     */
    public function test_unique_validation() {
        # Create a NestedModelObject to test with
        $field = new NestedModelField("FirewallAlias", required: true);
        $field->name = "test";

        $this->assert_throws_response(
            response_id: "MODEL_FIELD_MUST_BE_UNIQUE",
            code: 400,
            callable: function () use ($field) {
                $field->value = [
                    ["name" => "thismustbeunique", "type" => "host"],
                    ["name" => "thismustbeunique", "type" => "host"]
                ];
                $field->validate();
            }
        );
    }

    /**
     * Ensures `unique_together` validation works for NestedModelFields.
     */
    public function test_unique_together_validation() {
        # Create a NestedModelObject to test with. Use the DNSResolverHostOverride model for this test since its `host`
        # and `domain` fields must be unique together.
        $field = new NestedModelField("DNSResolverHostOverride", required: true);
        $field->name = "test";

        $this->assert_throws_response(
            response_id: "MODEL_FIELDS_NOT_UNIQUE_TOGETHER",
            code: 400,
            callable: function () use ($field) {
                $field->value = [
                    ["host" => "mustbeuniquewithdomain", "domain" => "mustbeuniquewithhost", "ip" => ["127.0.0.1"]],
                    ["host" => "mustbeuniquewithdomain", "domain" => "mustbeuniquewithhost", "ip" => ["127.0.0.2"]],
                ];
                $field->validate();
            }
        );
    }
}
