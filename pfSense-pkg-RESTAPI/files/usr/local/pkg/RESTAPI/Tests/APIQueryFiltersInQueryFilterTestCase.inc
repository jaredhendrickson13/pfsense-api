<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\InQueryFilter;

class APIQueryFiltersInQueryFilterTestCase extends TestCase {
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value is found within the filter value,
     * and false when it is not.
     */
    public function test_evaluate(): void {
        $filter = new InQueryFilter();

        # Check with string values
        $this->assert_is_true($filter->evaluate('test', 'test'));
        $this->assert_is_false($filter->evaluate('test', 'notfound'));

        # Check with array filter value and non-array field value
        $this->assert_is_true($filter->evaluate('apple', ['apple', 'banana']));
        $this->assert_is_false($filter->evaluate('cherry', ['apple', 'banana']));
    }
}
