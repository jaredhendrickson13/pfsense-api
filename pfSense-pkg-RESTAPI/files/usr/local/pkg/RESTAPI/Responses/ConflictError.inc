<?php

namespace RESTAPI\Responses;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates their is a conflict with the requested action. This exception generally corresponds
 * with a 409 conflict HTTP code.
 */
class ConflictError extends Response {
    public $code = 409;
    public string $help_text = "The client's request cannot be fulfilled due to one or more conflicts.";

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $_links = new ResourceLinkSet(),
    ) {
        parent::__construct(message: $message, response_id: $response_id, data: $data, _links: $_links);
    }
}
