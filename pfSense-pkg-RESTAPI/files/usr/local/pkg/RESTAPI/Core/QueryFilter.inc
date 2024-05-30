<?php

namespace RESTAPI\Core;

use RESTAPI\Responses\NotFoundError;

/**
 * Defines a query filter that can be used to further filter a ModelSet's 'model_objects'.
 */
abstract class QueryFilter
{
    use BaseTraits;

    /**
     * @var string The name of this query filter. This is used to identify the filter to use during queries.
     */
    public string $name;

    /**
     * Defines the logic used for evaluating filter matches during a query.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value matches the filter_value, false otherwise.
     */
    abstract public function evaluate(mixed $field_value, mixed $filter_value): bool;

    /**
     * Obtains an array of all available QueryFilter names.
     * @return array An array of all available QueryFilter names.
     */
     public static function get_all_names(): array
     {
         $filter_classes = Tools\get_classes_from_namespace(namespace: "\\RESTAPI\\QueryFilters\\");
         $filters = [];
         foreach ($filter_classes as $filter_class) {
             $filter = new $filter_class();
             $filters[] = $filter->name;
         }
         return $filters;
     }

    /**
     * Obtains the QueryFilter object associated with the given name.
     * @param string $name The name of the QueryFilter to obtain.
     * @return QueryFilter The QueryFilter object associated with the given name.
     * @throws NotFoundError If the given name does not correspond to a valid QueryFilter class.
     */
    public static function get_by_name(string $name): QueryFilter {
        # Get all available QueryFilter classes
        $filter_classes = Tools\get_classes_from_namespace(namespace: "\\RESTAPI\\QueryFilters\\");

        # Iterate through each QueryFilter class and return the one with the matching name
        foreach ($filter_classes as $filter_class) {
            $filter = new $filter_class();
            if ($filter->name === $name) {
                return $filter;
            }
        }

        # If no matching QueryFilter class was found, throw an error
        throw new NotFoundError(
            message: "No QueryFilter class found with the name '$name'.",
            response_id: "QUERY_FILTER_NOT_FOUND"
        );
    }
}