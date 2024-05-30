<?php

namespace RESTAPI\QueryFilters;

require_once('RESTAPI/autoloader.inc');

use RESTAPI\Core\QueryFilter;

/**
 * Defines a query filter that checks if the given field value starts with the given filter value.
 */
class StartsWithQueryFilter extends QueryFilter
{
    public string $name = 'startswith';

    /**
     * Checks if the given field value starts with the given filter value. If the value is an array, this filter will
     * evaluate the first element in the array for an exact match. Otherwise, this filter will evaluate the value directly.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value starts with the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool
    {
        if (is_array($field_value)) {
            $field_value = reset($field_value);
        }

        return (str_starts_with(strval($field_value), strval($filter_value)));
    }
}