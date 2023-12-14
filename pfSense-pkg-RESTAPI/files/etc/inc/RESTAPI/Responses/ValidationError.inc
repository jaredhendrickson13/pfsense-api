<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class tha indicates an input validation error. This exception generally corresponds with a 400 Bad Request
 * HTTP code.
 */
class ValidationError extends Response {
    public $code = 400;
    public string $help_text = "The client request data has one or more input validation errors.";

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $links = new ResourceLinkSet())
    {
        parent::__construct($message, $response_id, $data, $links);
    }
}
