<?php
//   Copyright 2023 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

namespace API\Core;

/**
 * Defines the API response object. Response objects are throwable objects that contain response information such as
 * HTTP status code, HTTP status message, API return code, error messages, and API response data. All API Error classes
 * will originate as Response objects.
 */
class Response extends \Error {
    public $message;
    public $code;
    public string $return;
    public string $status;
    public array $data;

    public function __construct(
        string $message,
        string $return,
        int $code = 500,
        array $data = [],
    ) {
        parent::__construct(strtolower($message), $code);
        $this->message = strtolower($message);
        $this->code = $code;
        $this->status = strtolower($this->get_http_status_message($code));
        $this->return = strtoupper($return);
        $this->data = $data;
    }

    /**
     * Converts this Response object to an array representation of it's contents.
     * @return array An array containing the response details.
     */
    public function to_array() : array {
        return [
            "code" => $this->code,
            "status" => $this->status,
            "message" => $this->message,
            "data" => $this->data
        ];
    }

    /**
     * Fetch HTTP status message associated with a given HTTP status code.
     * @param int $http_status The HTTP status code to fetch the descriptive message for.
     * @return string The HTTP status message associated with the give HTTP response code OR `unknown` if not found.
     */
    public static function get_http_status_message(int $http_status) : string {
        return match ($http_status) {
            100 => "continue",
            101 => "switching protocols",
            102 => "processing",
            103 => "checkpoint",
            200 => "ok",
            201 => "created",
            202 => "accepted",
            203 => "non-authoritative information",
            204 => "no content",
            205 => "reset content",
            206 => "partial content",
            207 => "multi-status",
            300 => "multiple choices",
            301 => "moved permanently",
            302 => "found",
            303 => "see other",
            304 => "not modified",
            305 => "use proxy",
            306 => "switch proxy",
            307 => "temporary redirect",
            400 => "bad request",
            401 => "unauthorized",
            402 => "payment required",
            403 => "forbidden",
            404 => "not found",
            405 => "method not allowed",
            406 => "not acceptable",
            407 => "proxy authentication required",
            408 => "request timeout",
            409 => "conflict",
            410 => "gone",
            411 => "length required",
            412 => "precondition failed",
            413 => "request entity too large",
            414 => "request-uri too long",
            415 => "unsupported media type",
            416 => "requested range not satisfiable",
            417 => "expectation failed",
            418 => "i'm a teapot",
            422 => "unprocessable entity",
            423 => "locked",
            424 => "failed dependency",
            425 => "unordered collection",
            426 => "upgrade required",
            449 => "retry with",
            450 => "blocked by windows parental controls",
            500 => "internal server error",
            501 => "not implemented",
            502 => "bad gateway",
            503 => "service unavailable",
            504 => "gateway timeout",
            505 => "http version not supported",
            506 => "variant also negotiates",
            507 => "insufficient storage",
            509 => "bandwidth limit exceeded",
            510 => "not extended",
            default => "unknown",
        };
    }
}

/**
 * Exception class that indicates a server or programming error. This exception generally corresponds with a 500
 * Internal Server Error HTTP code.
 */
class ServerError extends Response {
    public function __construct(string $message, string $return, int $code = 500, array $data = []) {
        parent::__construct($message, $return, $code, $data);
    }
}

/**
 * Exception class tha indicates an authentication error. This exception generally corresponds with a 401 unauthorized
 * HTTP code.
 */
class AuthenticationError extends Response {
    public function __construct(string $message, string $return, int $code = 401, array $data = []) {
        parent::__construct($message, $return, $code, $data);
    }
}

/**
 * Exception class that indicates insufficient privileges or an attempt to access a protected API resource. This
 * exception generally corresponds with a 403 forbidden HTTP code.
 */
class ForbiddenError extends Response {
    public function __construct(string $message, string $return, int $code = 403, array $data = []) {
        parent::__construct($message, $return, $code, $data);
    }
}

/**
 * Exception class that indicates the requested method is not allowed. This exception generally corresponds with a
 * 405 method not allowed HTTP code.
 */
class MethodNotAllowedError extends Response {
    public function __construct(string $message, string $return, int $code = 405, array $data = [],) {
        parent::__construct($message, $return, $code, $data);
    }
}

/**
 * Exception class tha indicates an input validation error. This exception generally corresponds with a 400 Bad Request
 * HTTP code.
 */
class ValidationError extends Response {
    public function __construct(string $message, string $return, int $code = 400, array $data = []) {
        parent::__construct($message, $return, $code, $data);
    }
}
