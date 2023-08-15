<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class tha indicates an authentication error. This exception generally corresponds with a 401 unauthorized
 * HTTP code.
 */
class AuthenticationError extends Response {
    public string $help_text = "The client has failed to authenticate the API call.";

    public function __construct(string $message, string $response_id, int $code = 401, array $data = []) {
        parent::__construct($message, $response_id, $code, $data);
    }
}