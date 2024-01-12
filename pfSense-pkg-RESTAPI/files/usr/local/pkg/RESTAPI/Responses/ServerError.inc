<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates a server or programming error. This exception generally corresponds with a 500
 * Internal Server Error HTTP code.
 */
class ServerError extends Response {
    public $code = 500;
    public string $help_text = "The server encountered an unexpected error and cannot complete the request.";

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $_links = new ResourceLinkSet()
    )
    {
        parent::__construct(message: $message, response_id: $response_id, data: $data, _links: $_links);
    }
}
