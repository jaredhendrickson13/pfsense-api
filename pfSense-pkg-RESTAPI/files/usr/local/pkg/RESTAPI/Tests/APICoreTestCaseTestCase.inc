<?php

namespace RESTAPI\Tests;

use AssertionError;
use Error;
use Exception;
use RESTAPI\Core\Response;
use RESTAPI\Core\TestCase;
use RESTAPI\Responses\NotFoundError;

/**
 * Defines test case for testing RESTAPI\Core\TestCase.
 */
class APICoreTestCaseTestCase extends TestCase
{
    /**
     * Checks that the TestCase's assert_does_not_throw() passes test if no exception was thrown.
     */
    public function test_assert_does_not_throw(): void {
        $this->assert_does_not_throw(
            callable: function (): void {}
        );
    }

    /**
     * Checks that the TestCase's assert_does_not_throw() fails test if any exception was thrown.
     */
    public function test_bad_assert_does_not_throw(): void {
        try {
            $this->assert_does_not_throw(
                callable: function (): void {
                    throw new Exception("test");
                }
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for throwing an exception.");
    }

    /**
     * Checks that the TestCase's assert_throws() passes test if specified exception was thrown.
     */
    public function test_assert_throws(): void {
        # Ensure assertion passes when proper exception is thrown
        $this->assert_throws(
            exceptions: [Exception::class],
            callable: function (): void {
                throw new Exception();
            }
        );
    }

    /**
     * Checks that the TestCase's assert_throws() fails test if the wrong exception was thrown.
     */
    public function test_wrong_exception_assert_throws(): void {
        # Ensure assertion fails when wrong exception is thrown.
        try {
            $this->assert_throws(
                exceptions: [Exception::class],
                callable: function (): void {
                    throw new Error();
                }
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving wrong exception.");
    }

    /**
     * Checks that the TestCase's assert_throws() fails test if no exception was thrown.
     */
    public function test_no_exception_assert_throws(): void {
        # Ensure assertion fails when no exception is thrown.
        try {
            $this->assert_throws(
                exceptions: [Exception::class],
                callable: function (): void {}
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving no exception.");

    }

    /**
     * Checks that the TestCase's assert_equals() passes test if both values were equal.
     */
    public function test_assert_equals(): void {
        $this->assert_equals("a", "a");
    }

    /**
     * Checks that the TestCase's assert_equals() passes test if values were not equal.
     */
    public function test_bad_assert_equals(): void {
        try {
            $this->assert_equals("a", "b");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving Fields that are not equal.");
    }

    /**
     * Checks that the TestCase's assert_is_greater_than() passes the test if $value1 is greater than $value2.
     */
    public function test_assert_is_greater_than(): void {
        $this->assert_is_greater_than(2, 1);
    }

    /**
     * Checks that the TestCase's assert_is_greater_than() fails the test if $value1 is not greater than $value2.
     */
    public function test_bad_assert_is_greater_than(): void {
        try {
            $this->assert_is_greater_than(2, 2);
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for second value being greater or equal to first value.");
    }

    /**
     * Checks that the TestCase's assert_is_greater_than() passes test if $value1 is greater than or equal to $value2.
     */
    public function test_assert_is_greater_than_or_equal(): void {
        $this->assert_is_greater_than_or_equal(2, 2);
        $this->assert_is_greater_than_or_equal(3, 2);
    }

    /**
     * Checks that the TestCase's assert_is_greater_than() fails test if $value1 is not greater than or equal to $value2.
     */
    public function test_bad_assert_is_greater_than_or_equal(): void {
        try {
            $this->assert_is_greater_than_or_equal(1, 2);
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for second value being greater or equal to first value.");
    }

    /**
     * Checks that the TestCase's assert_is_less_than() passes the test if $value1 is less than $value2.
     */
    public function test_assert_is_less_than(): void {
        $this->assert_is_less_than(1, 2);
    }

    /**
     * Checks that the TestCase's assert_is_greater_than() fails the test if $value1 is not less than $value2.
     */
    public function test_bad_assert_is_less_than(): void {
        try {
            $this->assert_is_less_than(2, 2);
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for second value being less or equal to first value.");
    }

    /**
     * Checks that the TestCase's assert_is_less_than() passes test if $value1 is less than or equal to $value2.
     */
    public function test_assert_is_less_than_or_equal(): void {
        $this->assert_is_less_than_or_equal(2, 2);
        $this->assert_is_less_than_or_equal(1, 2);
    }

    /**
     * Checks that the TestCase's assert_is_less_than() fails test if $value1 is not less than or equal to $value2.     
     */
    public function test_bad_assert_is_less_than_or_equal(): void {
        try {
            $this->assert_is_less_than_or_equal(3, 2);
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for second value being less or equal to first value.");
    }

    /**
     * Checks that the TestCase's assert_throws_response() passes test if a Response object or one of its children
     * were thrown with a specific 'response_id' and 'code'.
     */
    public function test_assert_throws_response(): void {
        $this->assert_throws_response(
            response_id: "EXAMPLE_ID",
            code: 500,
            callable: function (): void {
                throw new Response(
                    message: "Test",
                    response_id: "EXAMPLE_ID"
                );
            }
        );
    }

    /**
     * Checks that the TestCase's assert_throws_response() fails test if a Response object or one of its children
     * was thrown with the wrong 'response_id'.
     */
    public function test_bad_response_id_assert_throws_response(): void {
        try {
            $this->assert_throws_response(
                response_id: "ID_DOES_NOT_MATCH_RESPONSE",
                code: 500,
                callable: function (): void {
                    throw new Response(
                        message: "Test",
                        response_id: "EXAMPLE_ID"
                    );
                }
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for mismatched `response_id`");
    }

    /**
     * Checks that the TestCase's assert_throws_response() fails test if a Response object or one of its children
     * was thrown with the wrong 'code'.
     */
    public function test_bad_code_assert_throws_response(): void {
        try {
            $this->assert_throws_response(
                response_id: "EXAMPLE_ID",
                code: 500,
                callable: function (): void {
                    throw new NotFoundError(
                        message: "Test",
                        response_id: "EXAMPLE_ID"
                    );
                }
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for mismatched `code`");
    }

    /**
     * Checks that the TestCase's assert_throws_response() fails test if no exception was thrown.
     */
    public function test_no_exception_assert_throws_response(): void {
        try {
            $this->assert_throws_response(
                response_id: "EXAMPLE_ID",
                code: 500,
                callable: function (): void {}
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for no exception being thrown.");
    }

    /**
     * Checks that the TestCase's assert_not_equals() passes test if values are not equal.
     */
    public function test_assert_not_equals(): void {
        $this->assert_not_equals("a", "b");
    }

    /**
     * Checks that the TestCase's assert_not_equals() fails test if values are equal.
     */
    public function test_bad_assert_not_equals(): void {
        try {
            $this->assert_not_equals("a", "a");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving Fields that are equal.");
    }

    /**
     * Checks that the TestCase's assert_is_true() passes test if value is `true`.
     */
    public function test_assert_is_true(): void {
        $this->assert_is_true(true);
    }

    /**
     * Checks that the TestCase's assert_is_true() fails test if value is not `true`.
     */
    public function test_bad_assert_is_true(): void {
        try {
            $this->assert_is_true("NOT_TRUE");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is not `true`.");
    }

    /**
     * Checks that the TestCase's assert_is_false() passes test if value is `false`.
     */
    public function test_assert_is_false(): void {
        $this->assert_is_false(false);
    }

    /**
     * Checks that the TestCase's assert_is_false() passes test if value is not `false`.
     */
    public function test_bad_assert_is_false(): void {
        try {
            $this->assert_is_false("NOT_FALSE");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is not `false`.");
    }

    /**
     * Checks that the TestCase's test_assert_is_empty() passes test if value is empty.
     */
    public function test_assert_is_empty(): void {
        $this->assert_is_empty(false);
        $this->assert_is_empty(0);
        $this->assert_is_empty("");
        $this->assert_is_empty(null);
        $this->assert_is_empty([]);
    }

    /**
     * Checks that the TestCase's test_assert_is_empty() fails test if value is not empty.
     */
    public function test_bad_assert_is_empty(): void {
        try {
            $this->assert_is_empty("NOT_EMPTY");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is not empty.");
    }

    /**
     * Checks that the TestCase's assert_is_not_empty() passes test if value is not empty.
     */
    public function test_assert_is_not_empty(): void {
        $this->assert_is_not_empty(true);
        $this->assert_is_not_empty(1);
        $this->assert_is_not_empty("not empty");
        $this->assert_is_not_empty([1,2,3,4]);
    }

    /**
     * Checks that the TestCase's assert_is_not_empty() fails test if value is empty.
     */
    public function test_bad_assert_is_not_empty(): void {
        try {
            $this->assert_is_not_empty("");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is empty.");
    }

    /**
     * Checks that the TestCase's assert_str_contains() passes test if string contains another string.
     */
    public function test_assert_str_contains(): void {
        $this->assert_str_contains("this is a test string", "test");
    }

    /**
     * Checks that the TestCase's assert_str_contains() fails test if string does not contain another string
     */
    public function test_bad_assert_str_contains(): void {
        try {
            $this->assert_str_contains("this is a test string", "not in the string");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for not containing a string.");
    }

    /**
     * Checks that the TestCase's assert_str_does_not_contain() passes test if string does not contain another string.
     */
    public function test_assert_assert_str_does_not_contain(): void {
        $this->assert_str_does_not_contain("this is a test string", "not in the string");
    }

    /**
     * Checks that the TestCase's assert_str_does_not_contain() fails test if string does not contain another string
     */
    public function test_bad_assert_str_does_not_contain(): void {
        try {
            $this->assert_str_does_not_contain("this is a test string", "test");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for containing a string.");
    }

    /**
     * Checks that the TestCase's test_assert_type() passes test if value is of given the type.
     */
    public function test_assert_type(): void {
        $this->assert_type("", "string");
        $this->assert_type(false, "boolean");
        $this->assert_type(0, "integer");
        $this->assert_type(null, "NULL");
        $this->assert_type(0.01, "double");
        $this->assert_type([], "array");
    }

    /**
     * Checks that the TestCase's test_assert_type() fails test if value is not of given the type.
     */
    public function test_bad_assert_type(): void {
        try {
            $this->assert_type("NOT_A_BOOLEAN", "boolean");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for expecting boolean but receiving string.");
    }
}
