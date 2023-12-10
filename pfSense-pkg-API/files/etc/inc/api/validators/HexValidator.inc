<?php

namespace API\Validators;

use API\Core\Validator;
use API\Responses\ValidationError;

class HexValidator extends Validator
{
    /**
     * @const Stores the hexadecimal prefix value `0x`
     */
    const HEX_PREFIX = "0x";
    /**
     * @var int $expected_length The maximum character length the hexadecimal value can have. This value is determined
     * by the assigned $bytes value. This is generally twice the $bytes value.
     */
    private int $expected_length;

    /**
     * Constructs the HexValidator object.
     * @param int $bytes The expected byte length of the hexadecimal value. Set to 0 to allow hex values of any length.
     * @param bool $allow_hex_prefix Set to true to allow the hex prefix (0x) to be present in the value.
     */
    public function __construct(public int $bytes, public bool $allow_hex_prefix = true)
    {
        $this->expected_length = 2 * $this->bytes;
    }

    /**
     * Checks if a given value is a valid hexadecimal value of the provide byte length.
     * @param mixed $value The incoming value to be validated.
     * @param string $field_name The name of the field being validated.
     * @throws ValidationError When the $value is greater than or less than $this->maximum_length.
     * @throws ValidationError When the $value is not a hexadecimal string.
     */
    public function validate(mixed $value, string $field_name = "")
    {
        # Strip the hex prefix if the hex prefix is allowed
        if ($this->allow_hex_prefix and str_starts_with($value, self::HEX_PREFIX)) {
            $value = str_replace(self::HEX_PREFIX, "", $value);
        }

        # Ensure the value only contains hex characters
        if (!ctype_xdigit($value)) {
            throw new ValidationError(
                message: "Field `$field_name` must only contain hex characters.",
                response_id: "HEX_VALIDATOR_VALUE_CONTAINS_NOT_HEX_CHARS"
            );
        }

        # Ensure the matches the expected character length if an exact byte length was assigned
        if ($this->bytes and strlen($value) !== $this->expected_length) {
            throw new ValidationError(
                message: "Field `$field_name` must contain a hex value of $this->expected_length characters exactly.",
                response_id: "HEX_VALIDATOR_VALUE_IS_NOT_EXPECTED_LENGTH"
            );
        }
    }
}
