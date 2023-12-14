<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates the requested services is temporarily unavailable. This exception generally 
 * corresponds with a 503 Service Unavailable error code.
 */
class ServiceUnavailableError extends Response {
    public $code = 503;
    public string $help_text = "The requested service is temporarily unavailable.";

    /**
     * Constructs a new Response object.
     * @param mixed $message The verbose message to include in the Response.
     * @param string $response_id A unique response ID that can be used to identify this specific response. This should
     * be unique from any other Response object in the codebase!
     * @param int $retry_after The amount of time (in seconds) we expect the service to become available again.
     * @param Model|ModelSet $data A Model or ModelSet this Response relates to. This is typically only included
     * for successful Responses.
     * @param ResourceLinkSet $links A ResourceLinkSet containing any resource links associated with this response.
     */
    public function __construct(
        $message,
        string $response_id,
        int $retry_after = 5,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $links = new ResourceLinkSet())
    {
        # Set the Retry-After header with the number of seconds we expect for the services to become available
        # https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Retry-After
        header("Retry-After: $retry_after");
        parent::__construct($message, $response_id, $data, $links);
    }
}
