<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class that indicates a server or programming error. This exception generally corresponds with a 500
 * Internal Server Error HTTP code.
 */
class ServerError extends Response {
    public string $help_text = "The server encountered an unexpected error and cannot complete the request.";

    public function __construct(string $message, string $response_id, int $code = 500, array $data = []) {
        parent::__construct($message, $response_id, $code, $data);
    }
}