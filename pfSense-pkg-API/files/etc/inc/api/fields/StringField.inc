<?php

namespace API\Fields;

use API;

class StringField extends API\Core\Field {
    public int $max_length;

    public function __construct(
        int $max_length = 128,
        array $validators = [],
        array $choices = [],
        bool $allow_empty = false,
        bool $allow_null = false,
        bool $required = false,
        mixed $default = null,
        bool $read_only = false,
        bool $many = false,
        string $delimiter = ",",
        string $description = "",
        string $internal_name = ""
    )
    {
        parent::__construct(
            types: ["string"],
            validators: $validators + [new API\Validators\MaxLengthValidator($max_length)],
            choices: $choices,
            allow_empty: $allow_empty,
            allow_null: $allow_null,
            required: $required,
            default: $default,
            read_only: $read_only,
            many: $many,
            delimiter: $delimiter,
            description: $description,
            internal_name: $internal_name
        );
        $this->max_length = $max_length;
    }
}
