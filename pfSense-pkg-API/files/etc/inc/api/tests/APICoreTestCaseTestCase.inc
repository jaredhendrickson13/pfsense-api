<?php

namespace API\Tests;

use API\Core\Response;
use API\Core\TestCase;
use AssertionError;
use Error;
use Exception;

/**
 * Defines test case for testing API\Core\TestCase.
 */
class APICoreTestCaseTestCase extends TestCase
{
    /**
     * Checks that the TestCase's assert_does_not_throw() passes test if no exception was thrown.
     */
    public function test_assert_does_not_throw() {
        $this->assert_does_not_throw(
            callable: function () {}
        );
    }

    /**
     * Checks that the TestCase's assert_does_not_throw() fails test if any exception was thrown.
     */
    public function test_bad_assert_does_not_throw() {
        try {
            $this->assert_does_not_throw(
                callable: function () {
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
    public function test_assert_throws() {
        # Ensure assertion passes when proper exception is thrown
        $this->assert_throws(
            exceptions: [Exception::class],
            callable: function () {
                throw new Exception();
            }
        );
    }

    /**
     * Checks that the TestCase's assert_throws() fails test if the wrong exception was thrown.
     */
    public function test_wrong_exception_assert_throws() {
        # Ensure assertion fails when wrong exception is thrown.
        try {
            $this->assert_throws(
                exceptions: [Exception::class],
                callable: function () {
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
    public function test_no_exception_assert_throws() {
        # Ensure assertion fails when no exception is thrown.
        try {
            $this->assert_throws(
                exceptions: [Exception::class],
                callable: function () {}
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
    public function test_assert_equals() {
        $this->assert_equals("a", "a");
    }

    /**
     * Checks that the TestCase's assert_equals() passes test if values were not equal.
     */
    public function test_bad_assert_equals() {
        try {
            $this->assert_equals("a", "b");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving fields that are not equal.");
    }

    /**
     * Checks that the TestCase's assert_throws_response() passes test if a Response object or one of its children
     * were thrown with a specific 'ref_id' and 'code'.
     */
    public function test_assert_throws_response() {
        $this->assert_throws_response(
            ref_id: "EXAMPLE_ID",
            code: 500,
            callable: function () {
                throw new Response(
                    message: "Test",
                    ref_id: "EXAMPLE_ID"
                );
            }
        );
    }

    /**
     * Checks that the TestCase's assert_throws_response() fails test if a Response object or one of its children
     * was thrown with the wrong 'ref_id'.
     */
    public function test_bad_ref_id_assert_throws_response() {
        try {
            $this->assert_throws_response(
                ref_id: "ID_DOES_NOT_MATCH_RESPONSE",
                code: 500,
                callable: function () {
                    throw new Response(
                        message: "Test",
                        ref_id: "EXAMPLE_ID"
                    );
                }
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for mismatched `ref_id`");
    }

    /**
     * Checks that the TestCase's assert_throws_response() fails test if a Response object or one of its children
     * was thrown with the wrong 'code'.
     */
    public function test_bad_code_assert_throws_response() {
        try {
            $this->assert_throws_response(
                ref_id: "EXAMPLE_ID",
                code: 500,
                callable: function () {
                    throw new Response(
                        message: "Test",
                        ref_id: "EXAMPLE_ID",
                        code: 502
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
    public function test_no_exception_assert_throws_response() {
        try {
            $this->assert_throws_response(
                ref_id: "EXAMPLE_ID",
                code: 500,
                callable: function () {}
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
    public function test_assert_not_equals() {
        $this->assert_not_equals("a", "b");
    }

    /**
     * Checks that the TestCase's assert_not_equals() fails test if values are equal.
     */
    public function test_bad_assert_not_equals() {
        try {
            $this->assert_not_equals("a", "a");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving fields that are equal.");
    }

    /**
     * Checks that the TestCase's assert_is_true() passes test if value is `true`.
     */
    public function test_assert_is_true() {
        $this->assert_is_true(true);
    }

    /**
     * Checks that the TestCase's assert_is_true() fails test if value is not `true`.
     */
    public function test_bad_assert_is_true() {
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
    public function test_assert_is_false() {
        $this->assert_is_false(false);
    }

    /**
     * Checks that the TestCase's assert_is_false() passes test if value is not `false`.
     */
    public function test_bad_assert_is_false() {
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
    public function test_assert_is_empty() {
        $this->assert_is_empty(false);
        $this->assert_is_empty(0);
        $this->assert_is_empty("");
        $this->assert_is_empty(null);
        $this->assert_is_empty([]);
    }

    /**
     * Checks that the TestCase's test_assert_is_empty() fails test if value is not empty.
     */
    public function test_bad_assert_is_empty() {
        try {
            $this->assert_is_empty("NOT_EMPTY");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is not empty.");
    }

    /**
     * Checks that the TestCase's test_assert_type() passes test if value is of given the type.
     */
    public function test_assert_type() {
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
    public function test_bad_assert_type() {
        try {
            $this->assert_type("NOT_A_BOOLEAN", "boolean");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for expecting boolean but receiving string.");
    }
}
