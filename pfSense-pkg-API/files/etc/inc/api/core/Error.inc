<?php

namespace API\Core;

class Error extends \Error
{
    public function __construct(int $code = 0, ?\Throwable $previous = null)
    {
        $message = Response\get($code)["message"];
        parent::__construct($message, $code, $previous);
    }
}
