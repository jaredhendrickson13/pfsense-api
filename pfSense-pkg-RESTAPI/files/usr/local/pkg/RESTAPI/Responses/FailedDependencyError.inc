<?php

namespace RESTAPI\Responses;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates a requested resource requires a dependency which is not installed. This exception
 * generally corresponds with a 424 Failed Dependency HTTP code.
 */
class FailedDependencyError extends Response {
    public $code = 424;
    public string $help_text = 'The client has requested a resource that requires a dependency which is not installed.';

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $_links = new ResourceLinkSet(),
    ) {
        parent::__construct(message: $message, response_id: $response_id, data: $data, _links: $_links);
    }
}
