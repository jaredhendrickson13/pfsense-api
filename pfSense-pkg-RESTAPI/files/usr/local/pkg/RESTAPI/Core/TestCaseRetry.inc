<?php

namespace RESTAPI\Core;

use Attribute;

/**
 * A class to represent retry settings for test cases.  This is intended to be defined in methods as a method attribute.
 */
#[Attribute(Attribute::TARGET_METHOD)]
class TestCaseRetry
{

    /**
     * @param int $retries The number of retries to attempt
     * @param int $delay The delay in seconds between retries
     */
    public function __construct(public int $retries = 0, public int $delay = 0)
    {
        # Ensure values are non-negative
        $this->retries = max(0, $retries);
        $this->delay = max(0, $delay);
    }

}