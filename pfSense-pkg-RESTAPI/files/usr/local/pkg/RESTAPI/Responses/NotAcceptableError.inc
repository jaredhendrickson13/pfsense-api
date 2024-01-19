<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class tha indicates the request was not acceptable. This exception generally corresponds with a 406
 * Not Acceptable HTTP code.
 */
class NotAcceptableError extends Response {
    public $code = 406;
    public string $help_text = "The client has requested content in a format that is not supported.";

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
