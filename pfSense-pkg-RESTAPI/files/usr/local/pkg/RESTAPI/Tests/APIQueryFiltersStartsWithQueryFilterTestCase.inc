<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\ContainsQueryFilter;
use RESTAPI\QueryFilters\ExactQueryFilter;
use RESTAPI\QueryFilters\StartsWithQueryFilter;

class APIQueryFiltersStartsWithQueryFilterTestCase extends TestCase
{
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value starts with the filter value.
     */
    public function test_evaluate(): void {
        $filter = new StartsWithQueryFilter();

        # Check with string values
        $this->assert_is_true($filter->evaluate('test', 'te'));
        $this->assert_is_true($filter->evaluate('test', 't'));
        $this->assert_is_false($filter->evaluate('test', 'es'));

        # Check with array values
        $this->assert_is_true($filter->evaluate(['test', 'test2'], 'test'));
        $this->assert_is_false($filter->evaluate(['test', 'test2'], 'test2'));
    }
}