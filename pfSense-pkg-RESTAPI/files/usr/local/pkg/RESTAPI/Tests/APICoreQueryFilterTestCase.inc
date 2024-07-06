<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\QueryFilter;
use RESTAPI\Core\TestCase;
use RESTAPI\QueryFilters\ContainsQueryFilter;
use RESTAPI\QueryFilters\EndsWithQueryFilter;
use RESTAPI\QueryFilters\ExactQueryFilter;
use RESTAPI\QueryFilters\ExceptQueryFilter;
use RESTAPI\QueryFilters\GreaterThanEqualQueryFilter;
use RESTAPI\QueryFilters\GreaterThanQueryFilter;
use RESTAPI\QueryFilters\LessThanEqualQueryFilter;
use RESTAPI\QueryFilters\LessThanQueryFilter;
use RESTAPI\QueryFilters\RegexQueryFilter;
use RESTAPI\QueryFilters\StartsWithQueryFilter;

class APICoreQueryFilterTestCase extends TestCase {
    /**
     * Checks that the get_all_names() method returns an array of all available QueryFilter names.
     */
    public function test_get_all_names(): void {
        $filter_names = QueryFilter::get_all_names();
        $this->assert_is_true(in_array('exact', $filter_names));
        $this->assert_is_true(in_array('except', $filter_names));
        $this->assert_is_true(in_array('contains', $filter_names));
        $this->assert_is_true(in_array('startswith', $filter_names));
        $this->assert_is_true(in_array('endswith', $filter_names));
        $this->assert_is_true(in_array('gt', $filter_names));
        $this->assert_is_true(in_array('lt', $filter_names));
        $this->assert_is_true(in_array('gte', $filter_names));
        $this->assert_is_true(in_array('lte', $filter_names));
        $this->assert_is_true(in_array('regex', $filter_names));
    }

    /**
     * Checks that the get_by_name() method returns the correct QueryFilter object for a given name.
     */
    public function test_get_by_name(): void {
        $this->assert_is_true(QueryFilter::get_by_name('exact') instanceof ExactQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('except') instanceof ExceptQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('contains') instanceof ContainsQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('startswith') instanceof StartsWithQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('endswith') instanceof EndsWithQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('gt') instanceof GreaterThanQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('lt') instanceof LessThanQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('gte') instanceof GreaterThanEqualQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('lte') instanceof LessThanEqualQueryFilter);
        $this->assert_is_true(QueryFilter::get_by_name('regex') instanceof RegexQueryFilter);
    }
}
