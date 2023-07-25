<?php

namespace API\Core;


/**
 * Defines a Validator object which Field objects can use to quickly validate its value.
 */
class Validator {
    /**
     * Runs the necessary validation against a specified value. This method is intended to be overridden by a child
     * class to add the desired validation steps.
     * @param mixed $value
     * @throws ServerError When the validate() method is called by a extended child class but has not overridden the
     * default validate() method.
     */
    public function validate(mixed $value) {
        throw new ServerError("Validators must override default validate() method.");
    }
}
