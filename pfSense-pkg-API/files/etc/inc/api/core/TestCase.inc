<?php

namespace API\Core;

use AssertionError;
use Error;
use Exception;

require_once("api/core/Field.inc");
require_once("api/core/Validator.inc");
require_once("api/core/Model.inc");
require_once("api/core/ModelSet.inc");
require_once("api/core/Response.inc");
require_once("api/core/View.inc");

# Allow test case to use all models
foreach(glob("/etc/inc/api/models/*.inc") as $model) {
    require_once($model);
}

# Allow test case to use all fields
foreach(glob("/etc/inc/api/fields/*.inc") as $field) {
    require_once($field);
}

# Allow test case to use all views
foreach(glob("/etc/inc/api/views/*.inc") as $view) {
    require_once($view);
}

/**
 * Defines a TestCase object that can be used to define child TestCase classes that test internal functionality of
 * the API. This is needed because pfSense does not have PHPUnit readily available.
 */
class TestCase {
    public int $exit_code = 0;
    public int $failed_count = 0;

    public function __construct() {
        # Get this TestCase name
        $test_case_name = self::class;

        # Print that we are starting tests
        echo "Starting API unit test case $$test_case_name:".PHP_EOL;

        # Loop through all methods in this object
        foreach (get_class_methods($this) as $method) {
            # If this method starts with `test`, run the function.
            if (str_starts_with($method, "test")) {
                $this->$method();
            }
        }
    }

    protected function assertEquals(mixed $value1, mixed $value2, string $message = "Expected values to be equal.") {
        # Throw a TestError if the two values do not match
        if ($value1 !== $value2) {
            throw new AssertionError($message);
        }
    }

    protected function assertNotEquals(
        mixed $value1, mixed $value2, string $message = "Expected values to not be equal."
    )
    {
        # Throw a TestError if the two values match
        if ($value1 === $value2) {
            throw new AssertionError($message);
        }
    }

    protected function assertThrows(array $exceptions, mixed $callable, string $message = "") {
        # Variables
        $exceptions_str = implode(", ", $exceptions);
        $message = ($message) ?: "Expected callable to raise one of [$exceptions_str]";

        # Try to run the call able
        try {
            $callable();
        }
        catch (Exception | Error $exc) {
            # Throw an AssertionError if this did not throw the expected error
            if (!in_array(get_class($exc), $exceptions )) {

                throw new AssertionError();
            }
        }
    }

    protected function assertTrue(
        mixed $value, bool $strict = true, string $message = "Expected value to equal 'true'."
    ) {
        # With strict typing, check if this value is true
        if ($strict and $value !== true ) {
            throw new AssertionError($message);
        }
    }

    protected function assertType(mixed $value, string $type, string $message = "") {
        # Variables
        $message = ($message) ?: "Expected value to be of type '$type'";

        # Throw an assertion error if the type specified does not match the type of our value
        if (gettype($value) !== $type) {
            throw new AssertionError($message);
        }
    }
}
