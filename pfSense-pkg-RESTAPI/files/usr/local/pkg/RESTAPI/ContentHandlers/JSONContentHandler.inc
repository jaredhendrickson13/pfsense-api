<?php

namespace RESTAPI\ContentHandlers;

use JsonException;
use RESTAPI\Core\ContentHandler;
use RESTAPI\Core\Response;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a ContentHandler for JSON data. This handler is responsible for encoding and decoding JSON data for the
 * REST API and corresponds with the 'application/json' MIME type.
 */
class JSONContentHandler extends ContentHandler {
    /**
     * The MIME type associated with this content handler.
     * @var string $mime_type
     */
    public string $mime_type = 'application/json';

    /**
     * Encodes the given content as JSON.
     * @param mixed|null $content The content to encode.
     * @param Response|null $context The Response object that contains the content to encode.
     * @return mixed The encoded content.
     */
    protected function _encode(mixed $content = null, ?Response $context = null): mixed {
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
