<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class tha indicates a requested resource was not found. This exception generally corresponds with a 404
 * Not Found HTTP code.
 */
class NotFoundError extends Response {
    public $code = 404;
    public string $help_text = "The client has requested a resource that could not be found.";

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $links = new ResourceLinkSet())
    {
        parent::__construct($message, $response_id, $data, $links);
    }
}
