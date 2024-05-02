<?php

namespace RESTAPI\Core;

use ReflectionException;
use ReflectionMethod;
use RESTAPI\Responses\MediaTypeError;
use RESTAPI\Responses\NotAcceptableError;
use RESTAPI\Responses\ServerError;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

require_once 'RESTAPI/autoloader.inc';

/**
 * Defines a class that is responsible for decoding request content from remote clients, and encoding response content to
 * be sent to the remote client. Children of this class must correspond with the valid HTTP MIME type this
 * ContentHandler is designed to interact with.
 * @link MIME-Types https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types.
 */
class ContentHandler {
    use BaseTraits;

    const DEFAULT_ENCODE_MIME_TYPE = 'application/json';

    /**
     * @var string $mime_type The unique name for this ContentHandler. This name MUST correspond to a valid HTTP Content-Type
     * and/or Accept type. This value is case-insensitive!
     */
    public string $mime_type;

    /**
     * Obtains the content from the remote client in its unprocessed form.
     * @return mixed The unprocessed content from the remote client.
     * @throws ServerError When this method is called but not overridden by the child class.
     */
    public function get_content(): mixed {
        throw new ServerError(
            message: 'Child ContentHanlder classes must override the get_content() method.',
            response_id: 'CONTENT_HANDLER_GET_CONTENT_NOT_OVERRIDDEN',
        );
    }

    /**
     * Defines the steps necessary to encode the given content into the correct format. This method is generally used
     * to encode the remote client's response in the format requested by the client.
     * @note This method MUST be overridden by the child class to be used in response to a client's Accept header
     * @param mixed $content The content to be encoded in this ContentHandler's format.
     * @return mixed The content in this ContentHandler's respective format.
     * @throws NotAcceptableError When this ContentHandler does not support encoding content.
     */
    protected function _encode(mixed $content = null): mixed {
        throw new NotAcceptableError(
            message: "Content handler `$this->mime_type` cannot format a response as it does not support content " .
                'encoding. Change the value of your `Accept` header to a valid option and try again.',
            response_id: 'CONTENT_HANDLER_CANNOT_ENCODE_CONTENT',
        );
    }

    /**
     * Encodes the provided content into the format corresponding with this ContentHandler's MIME type and sets the
     * Content-Type and Content-Length response headers accordingly.
     * @param mixed $content The content to be encoded in this ContentHandler's format.
     * @return mixed The content in this ContentHandler's respective format.
     */
    final public function encode(mixed $content): mixed {
        $encoded_content = $this->_encode($content);
        header("content-type: $this->mime_type");
        return $encoded_content;
    }

    /**
     * Defines the steps necessary to decode the given content into a PHP native format. This method is generally used
     * to decode the remote client's request from the format indicated in the remote client's request.
     * @note This method must be overridden by the child class to be used in response to a client's Content-Type header
     * @param mixed $content The content to be decoded in this ContentHandler's format.
     * @return mixed The content in this ContentHandler's respective format.
     * @throws MediaTypeError When this ContentHandler does not support decoding content.
     */
    protected function _decode(mixed $content = null): mixed {
        throw new MediaTypeError(
            message: "Content handler `$this->mime_type` does not support decoding content. Change the value of your " .
                '`Content-Type` header to a valid option and try again.',
            response_id: 'CONTENT_HANDLER_CANNOT_DECODE_CONTENT',
        );
    }

    /**
     * Obtains and decodes the content from the format corresponding with this ContentHandler's MIME type.
     * @param mixed $content The content to be decoded in this ContentHandler's format. If no value is specified,
     * the content will automatically be populated using the `get_content()` method.
     * @return mixed The content in this ContentHandler's respective format.
     */
    final public function decode(mixed $content = null): mixed {
        $content = $content ?: $this->get_content();
        return $this->_decode($content);
    }

    /**
     * Checks if this ContentHandler object is capable of encoding content.
     * @return bool Returns true if this ContentHandler can encode content, otherwise false.
     * @throws ReflectionException When the _encode() method is missing entirely.
     */
    final public function can_encode(): bool {
        $reflector = new ReflectionMethod($this, '_encode');
        return $reflector->getDeclaringClass()->getName() === get_class($this);
    }

    /**
     * Checks if this ContentHandler object is capable of decoding content.
     * @return bool Returns true if this ContentHandler can decode content, otherwise false.
     * @throws ReflectionException When the _decode() method is missing entirely.
     */
    final public function can_decode(): bool {
        $reflector = new ReflectionMethod($this, '_decode');
        return $reflector->getDeclaringClass()->getName() === get_class($this);
    }

    /**
     * Obtains an encode-capable ContentHandler for the MIME type provided via the HTTP Accept header.
     * @returns ContentHandler The encode-capable ContentHandler object with the requested $mime_type.
     * @throws NotAcceptableError When the ContentHandler with the requested $mime_type does not support encoding.
     * @throws NotAcceptableError When no ContentHandler exists with this $mime_type.
     */
    public static function get_encode_handler(): ContentHandler {
        # Variables
        $handler_classes = get_classes_from_namespace('\\RESTAPI\\ContentHandlers\\');
        $mime_type = self::get_encode_mime_type();

        # Loop through each ContentHandler child class and find the handler for this $mime_type
        foreach ($handler_classes as $handler_class) {
            # Loop variables
            $handler = new $handler_class();
            $hanlder_mime_type = strtolower($handler->mime_type);

            # Skip this handler if the $mime_type does not match
            if ($mime_type !== $hanlder_mime_type) {
                continue;
            }

            # When mime types match, but the handler doesn't support encoding, throw a not accepted error
            if (!$handler->can_encode()) {
                throw new NotAcceptableError(
                    message: "Content handler `$mime_type` cannot format a response as it does not support " .
                        'content encoding. Change the value of your `Accept` header to a valid option and try again.',
                    response_id: 'CONTENT_HANDLER_CANNOT_ENCODE_CONTENT',
                );
            }

            # Otherwise, this is the requested handler, return it
            return $handler;
        }

        # Throw a not acceptable error if there was no ContentHandler with the requested $mime_type
        throw new NotAcceptableError(
            message: "No content handler exists for `$mime_type`. Change the value of your `Accept` header " .
                'to a valid option and try again.',
            response_id: 'CONTENT_HANDLER_ENCODE_NOT_FOUND',
        );
    }

    /**
     * Obtains a decode-capable ContentHandler for the MIME type provided via the HTTP Content-Type header.
     * @returns ContentHandler The decode-capable ContentHandler object with the requested $mime_type.
     * @throws NotAcceptableError When the ContentHandler with the requested $mime_type does not support decoding.
     * @throws NotAcceptableError When no ContentHandler exists with this $mime_type.
     */
    public static function get_decode_handler(): ContentHandler {
        # Variables
        $handler_classes = get_classes_from_namespace('\\RESTAPI\\ContentHandlers\\');
        $mime_type = self::get_decode_mime_type();

        # Default to application/x-www-form-urlencoded if the Content-Type header is empty
        if (empty($mime_type)) {
            $mime_type = 'application/x-www-form-urlencoded';
        }

        # Loop through each ContentHandler child class and find the handler for this $mime_type
        foreach ($handler_classes as $handler_class) {
            # Loop variables
            $handler = new $handler_class();
            $hanlder_mime_type = strtolower($handler->mime_type);

            # Skip this handler if the $mime_type does not match
            if ($mime_type !== $hanlder_mime_type) {
                continue;
            }

            # When mime types match, but the handler doesn't support decoding, throw a media type error
            if (!$handler->can_decode()) {
                throw new MediaTypeError(
                    message: "Content handler `$mime_type` does not support decoding content. Change the value of your " .
                        '`Content-Type` header to a valid option and try again.',
                    response_id: 'CONTENT_HANDLER_CANNOT_DECODE_CONTENT',
                );
            }

            # Otherwise, this is the requested handler, return it
            return $handler;
        }

        # Throw a media type error if there was no ContentHandler with the requested $mime_type
        throw new MediaTypeError(
            message: "No content handler exists for `$mime_type`. Change the value of your `Content-Type` header " .
                'to a valid option and try again.',
            response_id: 'CONTENT_HANDLER_DECODE_NOT_FOUND',
        );
    }

    /**
     * Obtains the MIME type from the HTTP Accept header or assumes the default if no Accept header is specified. The
     * Accept header controls which ContentHandler will be used to encode API responses.
     * @return string The MIME type to be used as requested in the HTTP Accept header.
     */
    public static function get_encode_mime_type(): string {
        # Obtain the requested MIME type from the HTTP Accept header sent by the remote client.
        $mime_type = strtolower($_SERVER['HTTP_ACCEPT']);

        # Assume the default if the Accept header is empty or any MIME type was requested
        if (empty($mime_type) or $mime_type === '*/*') {
            return self::DEFAULT_ENCODE_MIME_TYPE;
        }

        # Ignore media type parameters if present, they are unsupported at this time.
        if (str_contains($mime_type, ';')) {
            $mime_type = strstr($mime_type, ';', before_needle: true);
        }

        return $mime_type;
    }

    /**
     * Obtains the MIME type from the HTTP Content-Type header. The Content-Type header controls which ContentHandler
     * will be used to decode request data sent by the remote client. If no content-header was specified, the API
     * will assume a default of application/x-www-form-urlencoded for GET and DELETE requests and a default of
     * application/json for POST, PUT and PATCH requests.
     * @return string The MIME type to be used as requested in the HTTP Content-Header header.
     */
    public static function get_decode_mime_type(): string {
        # Obtain the requested MIME type from the HTTP Accept header sent by the remote client.
        $mime_type = strtolower($_SERVER['HTTP_CONTENT_TYPE']);
        $request_method = strtoupper($_SERVER['REQUEST_METHOD']);

        # Assume the default content-type based on the request method if no content-type was provided
        if (empty($mime_type)) {
            $mime_type = match ($request_method) {
                'DELETE', 'GET' => 'application/x-www-form-urlencoded',
                'POST', 'PUT', 'PATCH' => 'application/json',
                default => $mime_type,
            };
        }

        # Ignore media type parameters if present, they are unsupported at this time.
        if (str_contains($mime_type, ';')) {
            $mime_type = strstr($mime_type, ';', before_needle: true);
        }

        return $mime_type;
    }
}
