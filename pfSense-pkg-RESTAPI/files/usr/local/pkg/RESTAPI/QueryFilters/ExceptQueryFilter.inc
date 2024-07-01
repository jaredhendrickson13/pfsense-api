<?php

namespace RESTAPI\QueryFilters;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\QueryFilter;

/**
 * Defines a query filter that checks if the given field value does not match the given filter value exactly.
 */
class ExceptQueryFilter extends QueryFilter {
    public string $name = 'except';

    /**
     * Checks if the given field value does not match the given filter value exactly.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value does not match the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool {
        # If either value is null, use strict comparison to check if they are not equal.
        if ($field_value === null or $filter_value === null) {
            return $field_value !== $filter_value;
        }

        return $field_value != $filter_value;
    }
}
