<?php

namespace RESTAPI\QueryFilters;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\QueryFilter;

/**
 * Defines a query filter that checks if the given field value is within a set of values.
 */
class InQueryFilter extends QueryFilter {
    public string $name = 'in';

    /**
     * Checks if the field's value is one of the values in the given filter value array. If the field value is an
     * array, this filter will check if the field's value matches an item in the filter value array exactly. Otherwise,
     * this filter will check if the field's value is a substring of the filter value string.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value contains the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool {
        # When filter value is an array and field value is not, check if the field value is in the filter value array
        if (is_array($filter_value) and !is_array($field_value)) {
            return in_array($field_value, $filter_value);
        }

        return str_contains(strval($filter_value), strval($field_value));
    }
}
