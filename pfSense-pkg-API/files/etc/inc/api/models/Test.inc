<?php

namespace API\Models;

use API;
use API\Core\Auth;
use API\Core\Field;
use API\Core\Model;
use API\Fields\BooleanField;
use API\Fields\StringField;

/**
 * Defines a Test model object that can be used for unit testing. This model does not define any real object in pfSense
 * and should only be used for testing purposes!
 */
class Test extends Model {
    public Field $test_bool;
    public Field $test_string_unique;
    public Field $test_string_many;

    public function __construct(mixed $id = null, array $representation_data = [], Auth $client = null) {
        # Mark this model as a `many` model indicating multiple Test model objects can be created. This is also
        # required for the `unique` field to work.
        $this->many = true;

        # Use the dedicate `tests` config section of the API configuration for tests
        $api_pkg_id = API\Core\Tools\get_api_config()[0];
        $this->config_path = "installedpackages/package/$api_pkg_id/conf/tests";

        # Assign fields and field parameters for this model.
        $this->test_bool = new BooleanField(
            indicates_true: "on",
            indicates_false: "off",
            default: true,
            description: "Demonstrates a boolean field that internally uses 'on' to indicate 'true' or 'off' 
                to indicate 'false'."
        );
        $this->test_string_unique = new StringField(
            unique: true,
            required: false,
            default: "test",
            description: "Demonstrates a string field that must be unique from all other Test model objects stored
                in configuration."
        );
        $this->test_string_many = new StringField(
            allow_empty: true,
            default: [],
            many: true,
            delimiter: "/",
            description: "Demonstrates a string field that allows many values to be passed in as an array. When 'many'
                is set, values must be passed in as an array of strings. Also demonstrates that the internal version
                of this value will be the array of strings joined together by '/'."
        );

        # Parent base 'Model' object must be constructed very last to ensure the Model has all configured attributes.
        parent::__construct($id, $representation_data, $client);
    }

}
