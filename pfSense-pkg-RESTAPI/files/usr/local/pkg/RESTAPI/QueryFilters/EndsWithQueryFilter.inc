<?php

namespace RESTAPI\QueryFilters;

require_once('RESTAPI/autoloader.inc');

use RESTAPI\Core\QueryFilter;

/**
 * Defines a query filter that checks if the given field value ends with the given filter value.
 */
class EndsWithQueryFilter extends QueryFilter
{
    public string $name = 'endswith';

    /**
     * Checks if the given field value ends with the given filter value. If the value is an array, this filter will
     * evaluate the last element in the array for an exact match. Otherwise, this filter will evaluate the value directly.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value ends with the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool
    {
        if (is_array($field_value)) {
            $field_value = end($field_value);
        }

        return (str_ends_with(strval($field_value), strval($filter_value)));
    }
}