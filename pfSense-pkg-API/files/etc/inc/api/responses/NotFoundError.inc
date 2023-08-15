<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class tha indicates a requested resource was not found. This exception generally corresponds with a 404
 * Not Found HTTP code.
 */
class NotFoundError extends Response {
    public $code = 404;
    public string $help_text = "The client has requested a resource that could not be found.";

    public function __construct(string $message, string $response_id, array $data = []) {
        parent::__construct($message, $response_id, $data);
    }
}
