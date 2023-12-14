<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class tha indicates an authentication error. This exception generally corresponds with a 401 unauthorized
 * HTTP code.
 */
class AuthenticationError extends Response {
    public $code = 401;
    public string $help_text = "The client has failed to authenticate the API call.";
    
    
    public function __construct($message, string $response_id, Model|ModelSet $data = new ModelSet())
    {
        parent::__construct($message, $response_id, $data);
    }
}
