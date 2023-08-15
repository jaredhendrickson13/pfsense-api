<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class that indicates the requested method is not allowed. This exception generally corresponds with a
 * 405 method not allowed HTTP code.
 */
class MethodNotAllowedError extends Response {
    public function __construct(string $message, string $response_id, int $code = 405, array $data = [],) {
        parent::__construct($message, $response_id, $code, $data);
    }
}