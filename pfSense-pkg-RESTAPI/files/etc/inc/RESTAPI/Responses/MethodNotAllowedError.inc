<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Response;

/**
 * Exception class that indicates the requested method is not allowed. This exception generally corresponds with a
 * 405 method not allowed HTTP code.
 */
class MethodNotAllowedError extends Response {
    public $code = 405;
    public string $help_text = "The client has requested an HTTP method that is not supported by the current URL.";

    public function __construct(string $message, string $response_id, array $data = [],) {
        parent::__construct($message, $response_id, $data);
    }
}
