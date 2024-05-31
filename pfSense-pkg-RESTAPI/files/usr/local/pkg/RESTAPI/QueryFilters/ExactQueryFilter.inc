<?php

namespace RESTAPI\QueryFilters;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\QueryFilter;

/**
 * Defines a query filter that obtains matches to a field's value exactly.
 */
class ExactQueryFilter extends QueryFilter {
    public string $name = 'exact';

    /**
     * Checks if the given field value matches the given filter value exactly.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value matches the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool {
        return $field_value == $filter_value;
    }
}
