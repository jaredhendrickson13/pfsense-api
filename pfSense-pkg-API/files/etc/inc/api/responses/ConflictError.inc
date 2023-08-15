<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class that indicates their is a conflict with the requested action. This exception generally corresponds
 * with a 409 conflict HTTP code.
 */
class ConflictError extends Response {
    public string $help_text = "The client's request cannot be fulfilled due to one or more conflicts.";

    public function __construct(string $message, string $response_id, int $code = 409, array $data = [],) {
        parent::__construct($message, $response_id, $code, $data);
    }
}