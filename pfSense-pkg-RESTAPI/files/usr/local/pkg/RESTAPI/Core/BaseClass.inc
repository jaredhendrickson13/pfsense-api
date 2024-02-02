<?php

namespace RESTAPI\Core;

use ReflectionClass;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

/**
 * Defines the base class for all classes defined by this package. By extending this class, other classes are able
 * to inherit a standard toolset that can easily be updated across all this package's classes.
 */
class BaseClass
{
    /**
     * Obtains the shortname of the called class.
     * @return string The shortname for this object's class.
     */
    public function get_class_shortname(): string
    {
        return (new ReflectionClass($this))->getShortName();
    }

    /**
     * Obtains the fully qualified name of the called class.
     * @return string The FQN for this object's class.
     */
    public function get_class_fqn(): string
    {
        return (new ReflectionClass($this))->getName();
    }

    /**
     * Obtains all classes associated with this class's current namespace.
     * @return array An array of classes currently in this object's namespace
     */
    public function get_classes_in_namespace(bool $shortnames = false): array
    {
        return get_classes_from_namespace((new ReflectionClass($this))->getNamespaceName(), shortnames: $shortnames);
    }
}
