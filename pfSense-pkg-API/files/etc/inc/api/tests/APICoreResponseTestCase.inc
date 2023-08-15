<?php

namespace API\Tests;

use API;
use API\Responses\AuthenticationError;
use API\Responses\ForbiddenError;
use API\Responses\MethodNotAllowedError;
use API\Responses\NotFoundError;
use API\Responses\ServerError;
use API\Responses\ValidationError;

/**
 * Defines test case for testing API\Core\Response.
 */
class APICoreResponseTestCase extends API\Core\TestCase {
    /**
     * Checks that Response objects are throwable and checks that it corresponds with an HTTP 500 status code.
     */
    public function test_response_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 500,
            callable: function () {
                throw new API\Core\Response(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }

    /**
     * Checks that ServerError objects are throwable and checks that it corresponds with an HTTP 500 status code.
     */
    public function test_server_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 500,
            callable: function () {
                throw new ServerError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }

    /**
     * Checks that ValidationError objects are throwable and checks that it corresponds with an HTTP 400 status code.
     */
    public function test_validation_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 400,
            callable: function () {
                throw new ValidationError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }

    /**
     * Checks that NotFoundError objects are throwable and checks that it corresponds with an HTTP 404 status code.
     */
    public function test_not_found_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 404,
            callable: function () {
                throw new NotFoundError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }

    /**
     * Checks that AuthenticationError objects are throwable and checks that it corresponds with an HTTP
     * 401 status code.
     */
    public function test_authentication_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 401,
            callable: function () {
                throw new AuthenticationError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }

    /**
     * Checks that ForbiddenError objects are throwable and checks that it corresponds with an HTTP 403 status code.
     */
    public function test_forbidden_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 403,
            callable: function () {
                throw new ForbiddenError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }

    /**
     * Checks that MethodNotAllowedError objects are throwable and checks that it corresponds with an HTTP 405
     * status code.
     */
    public function test_method_not_allowed_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 405,
            callable: function () {
                throw new MethodNotAllowedError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
