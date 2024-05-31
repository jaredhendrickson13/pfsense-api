<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\ExactQueryFilter;
use RESTAPI\QueryFilters\GreaterThanEqualQueryFilter;
use RESTAPI\QueryFilters\LessThanEqualQueryFilter;
use RESTAPI\QueryFilters\LessThanQueryFilter;

class APIQueryFiltersGreaterThanEqualQueryFilterTestCase extends TestCase
{
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value is greater than or equal to the filter value.
     */
    public function test_evaluate(): void {
        $query_filter = new GreaterThanEqualQueryFilter();
        $this->assert_is_true($query_filter->evaluate(2, 1));
        $this->assert_is_true($query_filter->evaluate(2, 2));
        $this->assert_is_true($query_filter->evaluate("2", "2"));
        $this->assert_is_false($query_filter->evaluate(1, 2));
    }
}