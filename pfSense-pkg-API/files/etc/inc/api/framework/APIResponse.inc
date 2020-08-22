<?php
namespace APIResponse;

# Pulls a assoc array API response from our response library. Optionally formats descriptive data into messages.
function get($id, $data=[]) {
    $responses = [
        // 0-999 reserved for system API level responses
        0 => [
            "status" => "ok",
            "code" => 200,
            "return" => $id,
            "message" => "Success",
        ],
        1 => [
            "status" => "server error",
            "code" => 500,
            "return" => $id,
            "message" => "Process encountered unexpected error",
        ],
        2 => [
            "status" => "method not allowed",
            "code" => 405,
            "return" => $id,
            "message" => "Invalid HTTP method",
        ],
        3 => [
            "status" => "unauthorized",
            "code" => 401,
            "return" => $id,
            "message" => "Authentication failed",
        ],
        4 => [
            "status" => "forbidden",
            "code" => 401,
            "return" => $id,
            "message" => "Authorization failed",
        ],
        5 => [
            "status" => "not implemented",
            "code" => 501,
            "return" => $id,
            "message" => "Incompatible pfSense version",
        ],
        6 => [
            "status" => "forbidden",
            "code" => 403,
            "return" => $id,
            "message" => "Requested action is not allowed",
        ],
        7 => [
            "status" => "not found",
            "code" => 404,
            "return" => $id,
            "message" => "Search attribute not found",
        ],
        8 => [
            "status" => "not found",
            "code" => 404,
            "return" => $id,
            "message" => "Could not identify pfSense version",
        ],
        9 => [
            "status" => "forbidden",
            "code" => 403,
            "return" => $id,
            "message" => "Authentication mode must be set to JWT to enable access token authentication",
        ],
        10 => [
            "status" => "server error",
            "code" => "500",
            "return" => $id,
            "message" => "Your API request was valid but no actions were specified for this endpoint",
        ]
    ];

    $response = $responses[$id];
    $response["data"] = $data;
    return $response;
}
