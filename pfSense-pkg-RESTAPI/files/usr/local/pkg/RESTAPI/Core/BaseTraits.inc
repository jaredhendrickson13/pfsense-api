<?php

namespace RESTAPI\Core;

use ReflectionClass;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

/**
 * Defines a standard set of traits for all classes defined by this package. When class use this trait they will
 * automatically inherit all resources included.
 */
trait BaseTraits {
    /**
     * Obtains the shortname of the called class.
     * @return string The shortname for this object's class.
     */
    public static function get_class_shortname(): string {
        return (new ReflectionClass(static::class))->getShortName();
    }

    /**
     * Obtains the fully qualified name of the called class.
     * @return string The FQN for this object's class.
     */
    public static function get_class_fqn(): string {
        return (new ReflectionClass(static::class))->getName();
    }

    /**
     * Obtains all classes associated with this class's current namespace.
     * @return array An array of classes currently in this object's namespace
     */
    public function get_classes_in_namespace(bool $shortnames = false): array {
        return get_classes_from_namespace((new ReflectionClass($this))->getNamespaceName(), shortnames: $shortnames);
    }

    /**
     * Logs an error to the syslog.
     * @param string $message The error message to write to the syslog
     */
    public static function log_error(string $message): void {
        # Call the pfSense `log_error` function
        log_error($message);
    }
}
