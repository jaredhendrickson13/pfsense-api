<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class tha indicates an input validation error. This exception generally corresponds with a 400 Bad Request
 * HTTP code.
 */
class ValidationError extends Response {
    public $code = 400;
    public string $help_text = "The client request data has one or more input validation errors.";

    public function __construct(string $message, string $response_id, array $data = []) {
        parent::__construct($message, $response_id, $data);
    }
}
