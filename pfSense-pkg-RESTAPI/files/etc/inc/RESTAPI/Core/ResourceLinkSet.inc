<?php

namespace RESTAPI\Core;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Responses\ServerError;

/**
 * Defines a class that allows the storage and processing of many ResourceLink objects at once.
 */
class ResourceLinkSet
{
    /**
     * Constructs a new ResourceLinkSet object
     * @param array $resource_links An array of ResourceLink objects to include in this ResourceLinkSet
     */
    public function __construct(public array $resource_links = [])
    {
        # Throw an error if any $resource_links value is not a ResourceLink object
        foreach ($this->resource_links as $resource_link) {
            if (!is_object($resource_link) or !in_array("RESTAPI\Core\ResourceLink", class_parents($resource_link))) {
                throw new ServerError(
                    message: "ResourceLinkSets can only contain ResourceLink objects.",
                    response_id: "RESOURCE_LINK_SET_CONTAINS_NON_RESOURCE_LINK"
                );
            }
        }
    }
    
    /**
     * Converts all ResourceLink objects in this ResourceLinkSet into their representation values.
     * @returns array An array representation of each HAL ResourceLink object nested under the HAL standard `_links`
     * array key. 
     */
    public function to_representation() : array {
        # Variables
        $representation = ["_links" => []];
        
        # Loop through each ResourceLink object and add it's representation value
        foreach ($this->resource_links as $rel => $rel_object) {
            $representation["_links"][$rel] = $rel_object;
        }
        
        return $representation;
    }

}
