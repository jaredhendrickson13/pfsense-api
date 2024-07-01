<?php

namespace RESTAPI\Responses;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates insufficient privileges or an attempt to access a protected API resource. This
 * exception generally corresponds with a 403 forbidden HTTP code.
 */
class ForbiddenError extends Response {
    public $code = 403;
    public string $help_text = 'The client does not have sufficient privileges to make this API call.';

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $_links = new ResourceLinkSet(),
    ) {
        parent::__construct(message: $message, response_id: $response_id, data: $data, _links: $_links);
    }
}
