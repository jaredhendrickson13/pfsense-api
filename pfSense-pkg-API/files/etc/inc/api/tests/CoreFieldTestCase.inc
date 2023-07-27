<?php

namespace API\Tests;

use API;

class CoreFieldTestCase extends API\Core\TestCase
{
    /**
     * Ensures an error is thrown when a Field object has a `default` value that is not specified as a `choice`.
     */
    function test_default_with_choices () {
        $this->assertThrows(
            exceptions: [API\Core\ServerError::class],
            callable: function () {
                new API\Core\Field(choices: ["a", "b"], default: "NOT_A_CHOICE");
            }
        );
    }
}
