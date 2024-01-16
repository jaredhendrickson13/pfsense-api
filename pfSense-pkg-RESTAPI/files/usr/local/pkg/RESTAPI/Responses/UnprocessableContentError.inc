<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates a request was understood but cannot be processed for one or more reasons. This exception
 * generally corresponds with a 422 Unprocessable Content HTTP code.
 */
class UnprocessableContentError extends Response {
    public $code = 422;
    public string $help_text = "The client has requested a resource that requires a dependency which is not installed.";

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
