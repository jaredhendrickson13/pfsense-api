<?php

namespace RESTAPI\Responses;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates the requested method is not allowed. This exception generally corresponds with a
 * 405 method not allowed HTTP code.
 */
class MethodNotAllowedError extends Response {
    public $code = 405;
    public string $help_text = 'The client has requested an HTTP method that is not supported by the current URL.';

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $_links = new ResourceLinkSet(),
    ) {
        parent::__construct(message: $message, response_id: $response_id, data: $data, _links: $_links);
    }
}
