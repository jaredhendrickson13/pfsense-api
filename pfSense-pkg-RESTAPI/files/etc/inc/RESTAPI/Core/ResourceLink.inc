<?php

namespace RESTAPI\Core;

use RESTAPI\Responses\ServerError;
use const RESTAPI\Core\Globals\HAL_REL_NAMESPACE;

require_once("RESTAPI/autoloader.inc");

/**
 * Defines a ResourceLink object that represents a HAL link object. These objects are primarily used by Endpoint
 * classes to populate HAL links for objects that are related to the API call the Endpoint is serving.
 * @link https://stateless.group/hal_specification.html
 */
class ResourceLink
{
    /**
     * @var $custom_attributes array
     * Defines custom attributes to include in the link object. Any custom attributes defined should be well documented.
     * For example, the "title" attribute is a common custom HAL attribute.
     */
    public array $custom_attributes;

    /**
     * Constructs a new HAL ResourceLink object.
     * @link https://stateless.group/hal_specification.html
     * @param string $rel The HAL related resource name for this ResourceLink. This name should accurately represent
     * the relationship between the current resource and the linked resource. Any non-started related resource names
     * should be well documented.
     * @param string $href The URL to the linked resource. This should include any required query/path parameters
     * required to locate the linked resource.
     * @param bool $use_namespace Set to true to prefix the HAL namespace for this package to the $rel value or set to
     * false to use the $rel value directly. Unless $rel is a standard HAL related resource name, this should be set to
     * true.
     * @param mixed ...$custom_attributes Provide any custom attributes that should be included with this resource
     * link. Any custom attributes provided should be well documented.
     */
    public function __construct(
        public string $rel,
        public string $href,
        public bool $use_namespace = true,
        mixed ...$custom_attributes
    )
    {
        # Prefix the HAL namespace for this package to $rel if requested
        $this->rel = ($this->use_namespace) ? HAL_REL_NAMESPACE.":".$this->rel : $this->rel;

        # Assign custom attributes passed into this object during construction
        $this->custom_attributes = $custom_attributes;
    }

    /**
     * Converts this object to an array representation.
     * @returns array An array representation of this HAL resource link.
     */
    public function serialize() : array {
        return [
            $this->rel => ["href" => $this->href] + $this->custom_attributes
        ];
    }

    /**
     * Automatically determines ResourceLink objects for any related Model objects related to a given Model
     * and it's assigned Model class.
     * @param
     * @returns array An array of ResourceLink objects associated with this Model object.
     */
    public function get_related_model_links() : array {

    }
}
