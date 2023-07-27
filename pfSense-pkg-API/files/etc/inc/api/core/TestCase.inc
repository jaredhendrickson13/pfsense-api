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
    public string $method = "unknown";

    /**
     * Runs this TestCase when the object is created. Any method that starts with `test` will automatically be run.
     */
    public function run() {
        # use a property to keep track of the method that is currently running
        $this->method = "unknown";

        # Loop through all methods in this object
        foreach (get_class_methods($this) as $method) {
            # If this method starts with `test`, run the function.
            if (str_starts_with($method, "test")) {
                $this->method = $method;
                $this->$method();
            }
        }
    }

    /**
     * Checks if two values are equal.
     * @param mixed $value1 The first value to evaluate.
     * @param mixed $value2 The second value to evaluate.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When $value1 and $value2 are not equal.
     */
    protected function assert_equals(mixed $value1, mixed $value2, string $message = "Expected values to be equal.") {
        # Throw a TestError if the two values do not match
        if ($value1 !== $value2) {
            throw new AssertionError($message);
        }
    }

    /**
     * Checks if two values are not equal.
     * @param mixed $value1 The first value to evaluate.
     * @param mixed $value2 The second value to evaluate.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When $value1 and $value2 are equal.
     */
    protected function assert_not_equals(
        mixed $value1, mixed $value2, string $message = "Expected values to not be equal."
    )
    {
        # Throw a TestError if the two values match
        if ($value1 === $value2) {
            throw new AssertionError($message);
        }
    }

    /**
     * Checks if an embedded callable raises a specific error
     * @param array $exceptions An array of Exceptions this method will expect the callable to raise.
     * @param mixed $callable An anonymous function that raises one of the $exceptions.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When the $callable did not raise one of the $exceptions.
     */
    protected function assert_throws(array $exceptions, mixed $callable, string $message = "") {
        # Variables
        $exceptions_str = implode(", ", $exceptions);
        $message = ($message) ?: "Expected callable to raise one of [$exceptions_str]";

        # Try to run the call able
        try {
            $callable();
        }
        # If an exception was thrown, make sure it was one of the expected exceptions.
        catch (Exception | Error $exc) {
            $class_name = $exc::class;
            if (!in_array($class_name, $exceptions)) {
                $message = $message." Received '$class_name'.";
                throw new AssertionError($message);
            }
            return;
        }

        # If we ended up here, no exception was thrown. Throw an AssertionError
        throw new AssertionError($message);
    }

    /**
     * Checks if an embedded callable raises a specific API\Core\Response exception and ensures that the expected
     * 'return' code is given
     * @param string $exception The specific Response exception this method will expect the callable to raise.
     * @param string $ref_id The unique response return code string to check the Response object for.
     * @param mixed $callable An anonymous function that raises one of the $exceptions.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When the $callable did not raise one of the $exceptions.
     */
    protected function assert_throws_response(string $ref_id, mixed $callable) {
        # Try to run the call able
        try {
            $callable();
        }
            # If an exception was thrown, make sure it was one of the expected exceptions.
        catch (Exception | Error $exc) {
            # Capture the class of this exception object
            $exc_class_name = $exc::class;
            if ($exception == $exc_class_name) {
                $message = "Expected callable to raise '$exception' Received '$exc_class_name'.";
                throw new AssertionError($message);
            }
            return;
        }

        # If we ended up here, no exception was thrown. Throw an AssertionError
        throw new AssertionError($message);
    }

    /**
     * Checks if a given value is a `true` boolean.
     * @param mixed $value The value to evaluate.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When the value is not `true`.
     */
    protected function assert_is_true (
        mixed $value, string $message = "Expected value to equal 'true'."
    ) {
        # Check if this value is true
        if ($value !== true ) {
            throw new AssertionError($message);
        }
    }

    /**
     * Checks if a given value is a `false` boolean.
     * @param mixed $value The value to evaluate.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When the value is not `false`.
     */
    protected function assert_is_false (
        mixed $value, string $message = "Expected value to equal 'false'."
    ) {
        # Check if this value is false
        if ($value !== false ) {
            throw new AssertionError($message);
        }
    }

    /**
     * Checks if a given value is empty.
     * @param mixed $value The value to evaluate.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When the value is not empty.
     */
    protected function assert_is_empty(mixed $value,string $message = "Expected value to be empty.") {
        # Check if this value is false
        if (!empty($value)) {
            throw new AssertionError($message);
        }
    }

    /**
     * Checks if a given value is of a given type.
     * @param mixed $value The value to evaluate.
     * @param string $type The string representation of the type to expect. This must match a gettype() return value.
     * @param string $message Set the error message embedded in any AssertionError thrown by this method.
     * @throws AssertionError When the $value's type does not match the given $type.
     */
    protected function assert_type(mixed $value, string $type, string $message = "") {
        # Variables
        $message = ($message) ?: "Expected value to be of type '$type'";

        # Throw an assertion error if the type specified does not match the type of our value
        if (gettype($value) !== $type) {
            throw new AssertionError($message);
        }
    }
}
