<?php

namespace api\responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class that indicates the requested services is temporarily unavailable. This exception generally 
 * corresponds with a 503 Service Unavailable error code.
 */
class ServiceUnavailableResponse extends Response {
    public $code = 503;
    public string $help_text = "The requested service is temporarily unavailable.";

    public function __construct(string $message, string $response_id, int $retry_after = 5, array $data = []) {
        # Set the Retry-After header with the number of seconds we expect for the services to become available
        # https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Retry-After
        header("Retry-After: $retry_after");
        
        parent::__construct($message, $response_id, $data);
    }
}
