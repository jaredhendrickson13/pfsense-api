<?php

namespace RESTAPI\QueryFilters;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\QueryFilter;

class RegexQueryFilter extends QueryFilter {
    public string $name = 'regex';

    /**
     * Checks if the given field value matches the given filter value using a regular expression.
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value matches the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool {
        return preg_match($filter_value, $field_value) === 1;
    }
}
