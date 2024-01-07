<?php
//   Copyright 2023 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

namespace RESTAPI\Core;

require_once("RESTAPI/autoloader.inc");

use Error;
use ReflectionClass;
use RESTAPI\Models\RESTAPISettings;

const HTTP_STATUS_MESSAGES = [
    100 => "continue",
    101 => "switching protocols",
    102 => "processing",
    103 => "checkpoint",
    200 => "ok",
    201 => "created",
    202 => "accepted",
    203 => "non-authoritative information",
    204 => "no content",
    205 => "reset content",
    206 => "partial content",
    207 => "multi-status",
    300 => "multiple choices",
    301 => "moved permanently",
    302 => "found",
    303 => "see other",
    304 => "not modified",
    305 => "use proxy",
    306 => "switch proxy",
    307 => "temporary redirect",
    400 => "bad request",
    401 => "unauthorized",
    402 => "payment required",
    403 => "forbidden",
    404 => "not found",
    405 => "method not allowed",
    406 => "not acceptable",
    407 => "proxy authentication required",
    408 => "request timeout",
    409 => "conflict",
    410 => "gone",
    411 => "length required",
    412 => "precondition failed",
    413 => "request entity too large",
    414 => "request-uri too long",
    415 => "unsupported media type",
    416 => "requested range not satisfiable",
    417 => "expectation failed",
    418 => "i'm a teapot",
    422 => "unprocessable entity",
    423 => "locked",
    424 => "failed dependency",
    425 => "unordered collection",
    426 => "upgrade required",
    449 => "retry with",
    450 => "blocked by windows parental controls",
    500 => "internal server error",
    501 => "not implemented",
    502 => "bad gateway",
    503 => "service unavailable",
    504 => "gateway timeout",
    505 => "http version not supported",
    506 => "variant also negotiates",
    507 => "insufficient storage",
    509 => "bandwidth limit exceeded",
    510 => "not extended"
];

/**
 * Defines the API response object. Response objects are throwable objects that contain response information such as
 * HTTP status code, HTTP status message, API return code, error messages, and API response data. All API Error classes
 * will originate as Response objects.
 */
class Response extends Error {
    public $code = 500;
    public string $status;
    public string $help_text = "";

    /**
     * Constructs a new Response object.
     * @param mixed $message The verbose message to include in the Response.
     * @param string $response_id A unique response ID that can be used to identify this specific response. This should
     * be unique from any other Response object in the codebase!
     * @param Model|ModelSet $data A Model or ModelSet this Response relates to. This is typically only included
     * for successful Responses.
     * @param ResourceLinkSet $_links A ResourceLinkSet containing applicable ResourceLinks for this response.
     */
    public function __construct(
        public $message,
        public string $response_id,
        public Model|ModelSet $data = new ModelSet(),
        public ResourceLinkSet $_links = new ResourceLinkSet()
    ) {
        # Normalize values so they are consistent.
        $this->message = str_replace(["\"", "'"], "`", $message);
        $this->status = strtolower($this->get_http_status_message($this->code));
        $this->response_id = strtoupper($response_id);
        
        # Construct the Parent Error object to allow this object to become throwable.
        parent::__construct($this->message, $this->code);

    }

    /**
     * Converts this Response object to an array representation of its contents.
     * @return array An array containing the response details.
     */
    final public function to_representation() : array {
        # Set the standard representation for the Response
        $representation = [
            "code" => $this->code,
            "status" => $this->status,
            "response_id" => $this->response_id,
            "message" => $this->message,
            "data" => $this->data->to_representation()
        ];

        # If HATEOAS is enabled, include related response _links
        if (RESTAPISettings::get_api_config()["config"]["hateoas"] === "enabled") {
            $representation += $this->_links->to_representation();
        }

        return $representation;
    }

    /**
     * Fetch HTTP status message associated with a given HTTP status code.
     * @param int $http_status The HTTP status code to fetch the descriptive message for.
     * @return string The HTTP status message associated with the give HTTP response code OR `unknown` if not found.
     */
    final public static function get_http_status_message(int $http_status) : string {
        # Return the message associated with this HTTP status code if it exists
        if (array_key_exists($http_status,HTTP_STATUS_MESSAGES)) {
            return HTTP_STATUS_MESSAGES[$http_status];
        }
        # Otherwise, return the message 'unknown'
        else {
            return "unknown";
        }
    }

    /**
     * Fetches the shortname for this class.
     * @return string The shortname of the instantiating class.
     */
    final public function get_shortname() : string {
        return (new ReflectionClass($this))->getShortName();
    }

    /**
     * Represents this Response object as an OpenAPI response component.
     * @return array This Response object as an OpenAPI response component
     */
    final public function to_openapi_component() : array {
        return [
            "description" => $this->help_text,
            "content" => [
                "application/json" => [
                    "schema" => ['$ref' => '#/components/schemas/Response']
                ]
            ]
        ];
    }

    /**
     * Represents the Response object as an OpenAPI schema.
     * @return array This Response object as an OpenAPI schema.
     */
    final public static function to_openapi_schema() : array {
        return [
            "type" => "object",
            "properties" => [
                "code" => [
                    "description" => "The HTTP status code that corresponds with the API response.",
                    "type" => "integer"
                ],
                "status" => [
                    "description" => "The HTTP status message that corresponds with the HTTP status code.",
                    "type" => "string"
                ],
                "response_id" => [
                    "description" => "The unique response ID that corresponds with the result of the API".
                        "call. In most situations, this will contain an error code.",
                    "type" => "string"
                ],
                "message" => [
                    "description" => "The descriptive message detailing the results of the API call.",
                    "type" => "string"
                ],
                "data" => [
                    "description" => "The data requested from the API. In the event that many objects have".
                        "been requested, this field will be an array of objects. Otherwise, it will only return".
                        "the single object requested.",
                    "example" => [],
                    "oneOf" => [
                        ["type" => "array"],
                        ["type" => "object"]
                    ]
                ],
                "_links" => [
                    "description" => "An array of links to resources that are related to this API response.",
                    "example" => [],
                    "type" => "object"
                ]
            ]
        ];
    }
}
