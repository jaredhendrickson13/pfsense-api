<?php

namespace API\Core;

use API\Responses\ServerError;

require_once("api/auto_loader.inc");

/**
 * Defines a Validator object which Field objects can use to quickly validate its value.
 */
class Validator {
    public Model|null $model_context = null;
    public Field|null $field_context = null;

    /**
     * Runs the necessary validation against a specified value. This method is intended to be overridden by a child
     * class to add the desired validation steps.
     * @param mixed $value The value to be validated
     * @param string $field_name The name of the field that the value belongs to. This is used for error messages.
     */
    public function validate(mixed $value, string $field_name = "") {
        throw new ServerError(
            message: "Validators must override default validate() method.",
            response_id: "VALIDATOR_INCOMPLETE"
        );
    }

    /**
     * Sets a label on the assigned $field_context if present.
     * @param string $label_name The label to assign to the Field context object
     */
    protected function set_label(string $label_name) {
        # Only assign the label if a $field_context was assigned
        if ($this->field_context) {
            $this->field_context->labels[] = $label_name;
        }
    }
}
