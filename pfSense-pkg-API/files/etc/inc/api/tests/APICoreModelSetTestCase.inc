<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API;

/**
 * Defines test case for testing API\Core\ModelSet.
 */
class APICoreModelSetTestCase extends API\Core\TestCase {
    /**
     * Checks that a ServerError is thrown in a ModelSet is assigned non-Model objects during construction.
     */
    public function test_throws_server_error_with_non_models() {
        $this->assert_throws_response(
            response_id: "MODEL_SET_CONTAINS_NON_MODEL",
            code: 500,
            callable: function () {
                new API\Core\ModelSet(model_objects: ["Not a Model!"]);
            }
        );
    }

    /**
     * Checks that the ModelSet's `to_representation()` method correctly returns the representations of all
     * contained Model objects.
     */
    public function test_modelset_to_representation() {
        # Create Models to add to the test ModelSet
        $test_model1 = new API\Models\Test(representation_data: ["test_bool" => false, "test_integer" => 10]);
        $test_model2 = new API\Models\Test(representation_data: ["test_bool" => true, "test_integer" => 15]);

        # Create a ModelSet using the previously created Models
        $test_modelset = new API\Core\ModelSet(model_objects: [$test_model1, $test_model2]);

        # Ensure the representation of the ModelSet contains the representations of both objects
        $this->assert_is_true($test_modelset->to_representation()[0] === $test_model1->to_representation());
        $this->assert_is_true($test_modelset->to_representation()[1] === $test_model2->to_representation());
    }
}
