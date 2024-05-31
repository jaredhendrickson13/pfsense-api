<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\ExactQueryFilter;

class APIQueryFiltersExactQueryFilterTestCase extends TestCase {
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value matches the filter value exactly.
     */
    public function test_evaluate(): void {
        $query_filter = new ExactQueryFilter();
        $this->assert_is_true($query_filter->evaluate('test', 'test'));
        $this->assert_is_false($query_filter->evaluate('test', 'test2'));
    }
}
