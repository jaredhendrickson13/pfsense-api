<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\RegexQueryFilter;

class APIQueryFiltersRegexQueryFilterTestCase extends TestCase {
    /**
     * Checks that the 'evaluate' method correctly returns true when the field value matches a given regex pattern.
     */
    public function test_evaluate(): void {
        $filter = new RegexQueryFilter();

        # Check with string values
        $this->assert_is_true($filter->evaluate('test', '/^test$/'));
        $this->assert_is_true($filter->evaluate('test', '/^t/'));
        $this->assert_is_true($filter->evaluate('test', '/st$/'));
        $this->assert_is_false($filter->evaluate('test', '/^es/'));
        $this->assert_is_false($filter->evaluate('test', '/^test2/'));
        $this->assert_is_false($filter->evaluate('test', '/test2$/'));
    }
}
