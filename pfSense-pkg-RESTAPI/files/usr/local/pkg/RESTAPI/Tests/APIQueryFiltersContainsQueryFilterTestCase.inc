<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\ContainsQueryFilter;
use RESTAPI\QueryFilters\ExactQueryFilter;

class APIQueryFiltersContainsQueryFilterTestCase extends TestCase
{
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value contains the filter value.
     */
    public function test_evaluate(): void {
        $filter = new ContainsQueryFilter();

        # Check with string values
        $this->assert_is_true($filter->evaluate('test', 'test'));
        $this->assert_is_true($filter->evaluate('test', 'es'));
        $this->assert_is_false($filter->evaluate('test', 'test2'));

        # Check with array values
        $this->assert_is_true($filter->evaluate(['test', 'test2'], 'test'));
        $this->assert_is_true($filter->evaluate(['test', 'test2'], 'test2'));
        $this->assert_is_false($filter->evaluate(['test', 'test2'], 'test3'));

    }
}