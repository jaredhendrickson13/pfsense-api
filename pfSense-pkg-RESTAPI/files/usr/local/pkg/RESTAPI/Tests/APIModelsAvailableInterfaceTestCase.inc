<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\AvailableInterface;

class APIModelsAvailableInterfaceTestCase extends TestCase
{
    /**
     * Checks that we can correctly read AvailableInterface objects
     */
    public function test_read(): void
    {
        # Create a AvailableInterface to populate an interface
        $available_interface = AvailableInterface::read_all();
        $this->assert_is_true($available_interface->exists());
        $this->assert_is_not_empty($available_interface->first()->if->value);
        $this->assert_is_not_empty($available_interface->first()->mac->value);
        $this->assert_is_not_empty($available_interface->first()->in_use_by->value);
    }
}