<?php

namespace RESTAPI\QueryFilters;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\QueryFilter;

/**
 * Defines a query filter that checks if the given field value is less than the given filter value.
 */
class LessThanQueryFilter extends QueryFilter {
    public string $name = 'lt';

    /**
     * Checks if the given field value is less than the given filter value.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value is less than the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool {
        if (is_numeric($field_value) and is_numeric($filter_value)) {
            if (floatval($field_value) < floatval($filter_value)) {
                return true;
            }
        }
        return false;
    }
}
