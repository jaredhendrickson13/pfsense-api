<?php

namespace RESTAPI\Responses;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;

/**
 * Exception class that indicates a successful transaction occurred. This exception generally corresponds with a 200
 * OK HTTP code and is typically not thrown as an error.
 */
class Success extends Response
{
    public $code = 200;
    public string $help_text = "The client has made a successful request.";

    public function __construct(
        $message,
        string $response_id,
        Model|ModelSet $data = new ModelSet(),
        ResourceLinkSet $links = new ResourceLinkSet())
    {
        parent::__construct($message, $response_id, $data, $links);
    }
}
