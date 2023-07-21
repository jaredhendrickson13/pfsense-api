<?php

namespace API\Core;


/**
 * Defines a Validator object which Field objects can use to quickly validate its value.
 */
class Validator
{
    /**
     * Runs the necessary validation against the incoming value. This method is intended to be overridden by a child
     * class to add the desired validation steps.
     * @param mixed $value
     * @return bool `true` if the value is valid, `false` if it is not.
     */
    public function validate(mixed $value) : bool
    {
        return false;
    }
}
