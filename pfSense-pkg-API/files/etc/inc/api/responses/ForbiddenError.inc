<?php

namespace API\Responses;

require_once("api/auto_loader.inc");

use API\Core\Response;

/**
 * Exception class that indicates insufficient privileges or an attempt to access a protected API resource. This
 * exception generally corresponds with a 403 forbidden HTTP code.
 */
class ForbiddenError extends Response {
    public $code = 403;
    public string $help_text = "The client does not have sufficient privileges to make this API call.";

    public function __construct(string $message, string $response_id, array $data = []) {
        parent::__construct($message, $response_id, $data);
    }
}
