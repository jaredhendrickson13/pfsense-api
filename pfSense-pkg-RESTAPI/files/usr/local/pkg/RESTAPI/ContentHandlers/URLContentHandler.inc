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
     * Obtains the client's request data from PHP's $_GET superglobal.
     *
     * @return array The request data parsed from the query string.
     */
    public function get_content(): array {
        return $_GET;
    }

    /**
     * Decodes the given content from URL encoded data and makes basic type interference.
     * @param mixed|null $content The content to decode.
     * @return mixed The decode content.
     */
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
