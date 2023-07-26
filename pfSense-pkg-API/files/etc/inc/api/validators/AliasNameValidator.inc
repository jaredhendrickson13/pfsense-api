<?php

namespace API\Validators;

use API\Core\Validator;
use API\Core\ValidationError;

class AliasNameValidator extends Validator {
    /**
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws ValidationError When the value exceeds the maximum length specified.
     */
    public function validate(mixed $value, string $field_name = "") {
        if (!is_validaliasname($value)) {
            throw new ValidationError(
                message: "Invalid firewall alias name '$value'.",
                return: "INVALID_FIREWALL_ALIAS_NAME"
            );
        }
    }
}
