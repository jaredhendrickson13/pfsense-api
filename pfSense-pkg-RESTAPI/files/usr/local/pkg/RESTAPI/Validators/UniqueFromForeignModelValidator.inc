<?php

namespace RESTAPI\Validators;

use RESTAPI\Core\Validator;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a Validator object that ensures a Field's value is unique from another specified Model's field. This
 * allows you to prevent a Field from being set to the same value as a different Model's Field.
 */
class UniqueFromForeignModelValidator extends Validator {
    public string $foreign_model_name;
    public string $foreign_model_fqn;
    public string $foreign_model_field_name;

    /**
     * Define this validator object by specifying the name of the foreign Model and the specific Field on the foreign
     * Model to check for uniqueness.
     */
    public function __construct(string $model, string $field) {
        # Assign parameters to class properties
        $this->foreign_model_name = $model;
        $this->foreign_model_fqn = "\\RESTAPI\\Models\\$model";
        $this->foreign_model_field_name = $field;
    }

    /**
     * Checks if the value passed into this Validator matches the '$foreign_model_field_name' of any existing object
     * of the $foreign_model_name class
     */
    public function validate(mixed $value, string $field_name = ''): void {
        # Create the foreign model object
        $foreign_model_fqn = $this->foreign_model_fqn;
        $foreign_model_field_name = $this->foreign_model_field_name;
        $foreign_model = new $foreign_model_fqn();

        # For non-many Models, check if the $foreign_model_field_name's value matches this value directly
        if (!$foreign_model->many and $foreign_model->$foreign_model_field_name->value === $value) {
            throw new ValidationError(
                message: "Field '$field_name' cannot be set to '$value' because the value is already in " .
                    "use by field '$foreign_model_field_name' on '$this->foreign_model_name'.",
                response_id: 'UNIQUE_FROM_FOREIGN_NON_MANY_MODEL_VALIDATOR_FAILED',
            );
        }
        # Otherwise, query for matching Model objects and throw an error if found
        elseif ($foreign_model->many) {
            # Query for any object of this Model that has the defined field name set to this value
            $model_query = $foreign_model->query([$foreign_model_field_name => $value]);

            # Throw an error if a match exists
            if ($model_query->exists()) {
                $matched_model_object = $model_query->first();
                throw new ValidationError(
                    message: "Field '$field_name' cannot be set to '$value' because the value is already in " .
                        "use by field '$foreign_model_field_name' on '$this->foreign_model_name' object with ID " .
                        "'$matched_model_object->id'",
                    response_id: 'UNIQUE_FROM_FOREIGN_MANY_MODEL_VALIDATOR_FAILED',
                );
            }
        }
    }
}
