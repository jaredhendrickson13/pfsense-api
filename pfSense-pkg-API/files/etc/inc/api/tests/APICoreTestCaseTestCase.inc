<?php

namespace API\Tests;

use API\Core\Response;
use API\Core\TestCase;
use AssertionError;
use Error;
use Exception;

/**
 * Tests that test the API\Core\TestCase class.
 */
class APICoreTestCaseTestCase extends TestCase
{
    public function test_assert_does_not_throw() {
        $this->assert_does_not_throw(
            callable: function () {}
        );
    }

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

    public function test_assert_throws() {
        # Ensure assertion passes when proper exception is thrown
        $this->assert_throws(
            exceptions: [Exception::class],
            callable: function () {
                throw new Exception();
            }
        );
    }

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

    public function test_assert_equals() {
        $this->assert_equals("a", "a");
    }

    public function test_bad_assert_equals() {
        try {
            $this->assert_equals("a", "b");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving fields that are not equal.");
    }

    public function test_assert_throws_response() {
        $this->assert_throws_response(
            ref_id: "EXAMPLE_ID",
            callable: function () {
                throw new Response(
                    message: "Test",
                    ref_id: "EXAMPLE_ID"
                );
            }
        );
    }

    public function test_bad_ref_id_asset_throws_response() {
        try {
            $this->assert_throws_response(
                ref_id: "ID_DOES_NOT_MATCH_RESPONSE",
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

        throw new AssertionError("Expected AssertionError for mismatched ref_ids");
    }

    public function test_no_exception_asset_throws_response() {
        try {
            $this->assert_throws_response(
                ref_id: "EXAMPLE_ID",
                callable: function () {}
            );
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for no exception being thrown.");
    }

    public function test_assert_not_equals() {
        $this->assert_not_equals("a", "b");
    }

    public function test_bad_assert_not_equals() {
        try {
            $this->assert_not_equals("a", "a");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving fields that are equal.");
    }

    public function test_assert_is_true() {
        $this->assert_is_true(true);
    }

    public function test_bad_assert_is_true() {
        try {
            $this->assert_is_true("NOT_TRUE");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is not `true`.");
    }

    public function test_assert_is_false() {
        $this->assert_is_false(false);
    }

    public function test_bad_assert_is_false() {
        try {
            $this->assert_is_false("NOT_FALSE");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is not `false`.");
    }

    public function test_assert_is_empty() {
        $this->assert_is_empty(false);
        $this->assert_is_empty(0);
        $this->assert_is_empty("");
        $this->assert_is_empty(null);
        $this->assert_is_empty([]);
    }

    public function test_bad_assert_is_empty() {
        try {
            $this->assert_is_empty("NOT_EMPTY");
        }
        catch (AssertionError) {
            return;
        }

        throw new AssertionError("Expected AssertionError for receiving value that is not empty.");
    }

    public function test_assert_type() {
        $this->assert_type("", "string");
        $this->assert_type(false, "boolean");
        $this->assert_type(0, "integer");
        $this->assert_type(null, "NULL");
        $this->assert_type(0.01, "double");
        $this->assert_type([], "array");
    }

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
