<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class that indicates a successful transaction occurred. This exception generally corresponds with a 200
 * OK HTTP code and is typically not thrown as an error.
 */
class Success extends Response
{
    public string $help_text = "The client has made a successful request.";

    public function __construct(string $message, string $response_id, int $code = 200, array $data = []) {
        parent::__construct($message, $response_id, $code, $data);
    }
}