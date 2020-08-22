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
        ],
        
        4000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward interface required"
        ],
        4001=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward rule protocol required"
        ],
        4002 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward redirect IP required"
        ],
        4003 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward redirect port required"
        ],
        4004 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward source required"
        ],
        4005 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward destination required"
        ],
        4006=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown port forward interface"
        ],
        4007 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown port forward protocol"
        ],
        4008=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown port forward NAT reflection value"
        ],
        4009 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward redirect IP"
        ],
        4010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward redirect port"
        ],
        4011 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward source address"
        ],
        4012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward destination address"
        ],
        4013 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward source port"
        ],
        4014 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward destination port"
        ],
        4015 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward rule ID required"
        ],
        4016 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward rule ID does not exist"
        ],
        4017 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP configuration ID required"
        ],
        4018 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP configuration ID does not exist"
        ],
        4019 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP mode required"
        ],
        4020 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP interface required"
        ],
        4021 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP subnet required"
        ],
        4022 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP CARP password required"
        ],
        4023 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown virtual IP mode"
        ],
        4024 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown virtual IP interface"
        ],
        4025 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP subnet"
        ],
        4026 =>[
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP subnet already in use"
        ],
        4027 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP VHID already in use"
        ],
        4028 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP VHID"
        ],
        4029 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP CARP advertisement base"
        ],
        4030 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP CARP advertisement skew"
        ],
    ];

    $response = $responses[$id];
    $response["data"] = $data;
    return $response;
}
