<?php

namespace API\Validators;

require_once("api/auto_loader.inc");

use API;
use API\Responses\ValidationError;

/**
 * A Validator object that checks if two or more fields together are unique from all other objects of a given Model.
 */
class UniqueTogetherValidator extends API\Core\Validator {
    public array $fields;
    public API\Core\Model $model;

    public function __construct(array $fields, API\Core\Model &$model) {
        $this->fields = $fields;
        $this->model = &$model;

        # Throw an error if Fields were provided that are not known to the Model
        if (array_intersect($this->model->get_fields(), $fields) != $fields) {
            throw new API\Responses\ServerError(
                message: "Unknown Model fields provided to UniqueTogetherValidator",
                response_id: "UNIQUE_TOGETHER_VALIDATOR_WITH_UNKNOWN_FIELD"
            );
        }
    }

    /**
     * Checks if a given value is a valid IP address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Responses\ValidationError When the value is not a valid IPv4 address.
     */
    public function validate(mixed $value, string $field_name = "") {
        # Throw an error if this validator is used on a non-many model
        if (!$this->model->many) {
            throw new API\Responses\ServerError(
                message: "UniqueTogetherValidator may only be used on Models with `many` enabled.",
                response_id: "UNIQUE_TOGETHER_VALIDATOR_ON_NON_MANY_MODEL"
            );
        }

        # Format the query parameters
        $query_params = [];
        foreach ($this->fields as $field) {
            $query_params[$field] = $this->model->$field->value;
        }

        # Query for existing objects with the same Field values
        $model_query = $this->model->read_all()->query(query_params: $query_params);
    }
}
