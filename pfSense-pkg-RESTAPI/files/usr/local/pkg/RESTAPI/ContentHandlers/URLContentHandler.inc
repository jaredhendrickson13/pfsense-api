<?php

namespace RESTAPI\ContentHandlers;

use JsonException;
use RESTAPI\Core\ContentHandler;
use RESTAPI\Responses\ValidationError;

class URLContentHandler extends ContentHandler {
    public string $mime_type = 'application/x-www-form-urlencoded';

    public function get_content(): array {
        return $_GET;
    }

    protected function _decode(mixed $content = null): mixed {
        # Loop through each query string item and check for expected data types
        foreach ($content as $key => $value) {
            # Check for boolean type query strings
            if ($value === 'true') {
                $content[$key] = true;
            } elseif ($value === 'false') {
                $content[$key] = false;
            }

            # Check for integer type query strings
            if (is_numeric($value)) {
                $content[$key] = intval($value);
            }
        }

        return $content;
    }
}
