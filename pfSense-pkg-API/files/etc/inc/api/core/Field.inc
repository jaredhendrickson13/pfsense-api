<?php

namespace API\Core;

class Field
{
    public mixed $value = null;
    public array $types = [];
    public array $validators = [];
    public string $help_text = "";
    public bool $many = false;
    public array $choices = [];
    public mixed $default = null;

    /**
     * @return bool `true` if all validation succeeded, `false` if any validators failed.
     */
    public function validate() : bool {
        # Ensure the value(s) to validate are always in an array so we can loop.
        $values = ($this->is_many_allowed()) ? $this->value : [$this->value];

        # Loop through each value to be validated
        foreach($values as $value) {
            # Loop through each validator and run it's `validate` method against the current value.
            foreach ($this->validators as $validator) {
                if (!$validator->validate($value));
            }
        }

    }

    /**
     * Checks if `many` is enabled, and if so, ensures `value` is an array.
     * @return bool `true` if `many` is enabled and allowed, `false` if `many` is not enabled.
     * @throws Error When `many` is enabled, but `value` is not an array.
     */
    private function is_many_allowed() : bool {
        # Raise an error if `many` is enabled but the `value` is not an array
        if ($this->many) {
            if (!is_array($this->value)) {
                throw new Error("`many` is enabled but `value` is not an array.");
            }
            return true;
        }

        return false;
    }

}
