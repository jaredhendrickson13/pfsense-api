<?php

namespace RESTAPI\ContentHandlers;

use RESTAPI\Core\ContentHandler;

/**
 * Defines a ContentHandler for URL encoded data. This handler is responsible for decoding URL encoded data for the
 * REST API and corresponds with the 'application/x-www-form-urlencoded' MIME type.
 */
class URLContentHandler extends ContentHandler {
    public string $mime_type = 'application/x-www-form-urlencoded';

    /**
     * Obtains the raw query string data from the request if present, otherwise obtains the raw request body.
     * @note In the event that both the request body and query string contain data, the query string data will take precedence.
     * @return string|null The request data parsed from the query string.
     */
    public function get_content(): ?string {
        return $_SERVER['QUERY_STRING'] ?: file_get_contents('php://input');
    }

    /**
     * Decodes the given content from URL encoded data and makes basic type interference.
     * @param mixed|null $content The content to decode.
     * @return mixed The decode content.
     */
    protected function _decode(mixed $content = null): mixed {
        # Parse the request data into an array
        parse_str($content, $content);

        # Loop through each query string item and check for expected data types
        foreach ($content as $key => $value) {
            # Infer the data type of the value
            $content[$key] = $this->infer_type($value);
        }

        return $content;
    }

    /**
     * Infers the data type of given string values.
     * @param string $value The value to infer the data type of.
     * @return mixed The inferred value converted to the appropriate data type.
     */
    public function infer_type(mixed $value): mixed {
        # If value is an array, recursively infer the type of each element
        if (is_array($value)) {
            return array_map([$this, 'infer_type'], $value);
        }

        # Convert primitive data types
        if ($value === 'true') {
            return true;
        } elseif ($value === 'false') {
            return false;
        } elseif ($value === 'null') {
            return null;
        } elseif (ctype_digit($value)) {
            return intval($value);
        } elseif (is_numeric($value) and str_contains($value, '.')) {
            return floatval($value);
        }

        # If a data type could not be inferred, return the original string value
        return $value;
    }
}
