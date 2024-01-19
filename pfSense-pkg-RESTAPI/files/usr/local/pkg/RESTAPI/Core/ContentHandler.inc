<?php

namespace RESTAPI\Core;

use RESTAPI\Responses\MediaTypeError;
use RESTAPI\Responses\NotAcceptableError;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

require_once("RESTAPI/autoloader.inc");

/**
 * Defines a class that is responsible for decoding request content from remote clients, and encoding response content to
 * be sent to the remote client. Children of this class must correspond with the valid HTTP MIME type this 
 * ContentHandler is designed to interact with. Note: ContentHandlers should not interact with the remote client 
 * directly. That is the Endpoint's job. Only use ContentHandlers to encode and decode content in the associated format.
 * @link MIME-Types https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types.
 */
class ContentHandler
{
    /**
     * @var string $mime_type The unique name for this ContentHandler. This name MUST correspond to a valid HTTP Content-Type
     * and/or Accept type. This value is case-insensitive!
     */
    public string $mime_type;

    /**
     * Defines the steps necessary to encode the given content into the correct format. This method is generally used
     * to encode the remote client's response in the format requested by the client.
     * @note This method must be overridden by the child class to be used in response to a client's Accept header
     * @param mixed $content The content to be encoded in this ContentHandler's format.
     * @return mixed The content in this ContentHandler's respective format. 
     * @throws NotAcceptableError When this ContentHandler does not support encoding content.
     */
    public function encode_content(mixed $content = null): mixed {
        throw new NotAcceptableError(
            message: "Content handler `$this->mime_type` cannot format a response as it does not support content ".
                "encoding. Change the value of your `Accept` header to a valid option and try again.",
            response_id: "CONTENT_HANDLER_CANNOT_ENCODE_CONTENT"
        );
    }

    /**
     * Defines the steps necessary to decode the given content into a PHP native format. This method is generally used
     * to decode the remote client's request from the format indicated in the remote client's request.
     * @note This method must be overridden by the child class to be used in response to a client's Content-Type header
     * @param mixed $content The content to be decoded in this ContentHandler's format.
     * @return mixed The content in this ContentHandler's respective format.
     * @throws MediaTypeError When this ContentHandler does not support decoding content.
     */
    public function decode_content(mixed $content = null): mixed {
        throw new MediaTypeError(
            message: "Content handler `$this->mime_type` does not support decoding content. Change the value of your ".
                "`Content-Type` header to a valid option and try again.",
            response_id: "CONTENT_HANDLER_CANNOT_DECODE_CONTENT"
        );
    }

    /**
     * Checks if this ContentHandler object is capable of encoding content.
     * @return bool Returns true if this ContentHandler can encode content, otherwise false.
     */
    final public function can_encode_content(): bool {
        $content_handler = get_called_class();
        return method_exists($content_handler, "encode_content");
    }

    /**
     * Checks if this ContentHandler object is capable of deocding content.
     * @return bool Returns true if this ContentHandler can decode content, otherwise false.
     */
    final public function can_decode_content(): bool {
        $content_handler = get_called_class();
        return method_exists($content_handler, "decode_content");
    }
    
    /**
     * Obtains an encode-capapble ContentHandler for a given MIME type.
     * @param string $mime_type The MIME type of the ContentHandler to look for.
     * @returns ContentHandler The encode-capable ContentHandler object with the requested $mime_type.
     * @throws NotAcceptableError When the ContentHandler with the requested $mime_type does not support encoding.
     * @throws NotAcceptableError When no ContentHandler exists with this $mime_type. 
     */
    public static function get_encode_handler(string $mime_type): ContentHandler {
        # Variables
        $handler_classes = get_classes_from_namespace("\\RESTAPI\\ContentHandlers\\");
        $mime_type = strtolower($mime_type);
        
        # Loop through each ContentHandler child class and find the handler for this $mime_type
        foreach ($handler_classes as $handler_class) {
            # Loop variables
            $handler = new $handler_class();
            $hanlder_mime_type = strtolower($handler->mime_type);
            
            # Skip this handler if the $mime_type does not match
            if ($mime_type !== $hanlder_mime_type) {
                continue;
            }
            
            # When mime types match, but the handler doesn't support encoding, run encode_content() to throw an error
            if (!$handler->encode_content()) {
                
            }
            
        }
    }

}
