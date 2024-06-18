<?php

namespace RESTAPI\ContentHandlers;

use JsonException;
use RESTAPI\Core\ContentHandler;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a ContentHandler for JSON data. This handler is responsible for encoding and decoding JSON data for the
 * REST API and corresponds with the 'application/json' MIME type.
 */
class JSONContentHandler extends ContentHandler {
    public string $mime_type = 'application/json';

    /**
     * Obtains the client's raw request body content.
     *
     * @return string The raw content from the request body.
     */
    public function get_content(): string {
        return file_get_contents('php://input') ?: '';
    }

    /**
     * Encodes the given content as JSON.
     * @param mixed|null $content The content to encode.
     * @return mixed The encoded content.
     */
    protected function _encode(mixed $content = null): mixed {
        return $content ? json_encode($content) : [];
    }

    /**
     * Decodes the given content from JSON.
     * @param mixed|null $content The content to decode.
     * @return mixed The decoded content.
     * @throws ValidationError If the content could not be decoded.
     */
    protected function _decode(mixed $content = null): mixed {
        # Allow empty strings, but convert it to an empty array.
        if (empty($content)) {
            return [];
        }

        # Try to decode the JSON string, throw a not acceptable error if the string is not JSON encoded.
        try {
            return json_decode($content, associative: true, flags: JSON_THROW_ON_ERROR);
        } catch (JsonException) {
            throw new ValidationError(
                message: 'Request body could not be decoded as JSON.',
                response_id: 'JSON_CONTENT_HANDLER_COULD_NOT_DECODE_JSON',
            );
        }
    }
}
