<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\ContainsQueryFilter;
use RESTAPI\QueryFilters\EndsWithQueryFilter;
use RESTAPI\QueryFilters\ExactQueryFilter;
use RESTAPI\QueryFilters\StartsWithQueryFilter;

class APIQueryFiltersEndsWithQueryFilterTestCase extends TestCase
{
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value ends with the filter value.
     */
    public function test_evaluate(): void {
        $filter = new EndsWithQueryFilter();

        # Check with string values
        $this->assert_is_true($filter->evaluate('test', 'st'));
        $this->assert_is_true($filter->evaluate('test', 't'));
        $this->assert_is_false($filter->evaluate('test', 'es'));

        # Check with array values
        $this->assert_is_true($filter->evaluate(['test', 'test2'], 'test2'));
        $this->assert_is_false($filter->evaluate(['test', 'test2'], 'test'));
    }
}