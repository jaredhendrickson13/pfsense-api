<?php

namespace RESTAPI\Core;

use RESTAPI\Responses\ServerError;

require_once("RESTAPI/autoloader.inc");

/**
 * Defines a ResourceLink object that represents a HAL link object.
 * @link https://stateless.group/hal_specification.html
 */
class ResourceLink
{
    const STANDARD_RELS = ["self", "related", "item", "collection", "parent", "delete"];
    public function __construct(public string $rel, public string $href, ...$custom_attributes)
    {
        # Require $rel to start with this package's HAL namespace if it is not a standard $rel
        if (!in_array($this->rel, self::STANDARD_RELS)) {
            
        }
    }
    
    

}
