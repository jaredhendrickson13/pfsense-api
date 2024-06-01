<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\ExactQueryFilter;
use RESTAPI\QueryFilters\ExceptQueryFilter;

class APIQueryFiltersExceptQueryFilterTestCase extends TestCase {
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value does not match the filter value.
     */
    public function test_evaluate(): void {
        $query_filter = new ExceptQueryFilter();
        $this->assert_is_true($query_filter->evaluate('test', 'test2'));
        $this->assert_is_true($query_filter->evaluate(0, null));
        $this->assert_is_false($query_filter->evaluate('test', 'test'));
    }
}
