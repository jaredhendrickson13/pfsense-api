<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;

class APIModelsIPsecPhase2TestCase extends TestCase
{
    const BASE_P1_DATA = [
        "iketype" => "ikev2",
        "protocol" => "inet",
        "interface" => "wan",
        "remote_gateway" => "1.2.3.4",
        "authentication_method" => "pre_shared_key",
        "myid_type" => "myaddress",
        "peerid_type" => "peeraddress",
        "pre_shared_key" => "testkey",
        "encryption" => [
            [
                "encryption_algorithm_name" => "aes",
                "encryption_algorithm_keylen" => 256,
                "hash_algorithm" => "sha256",
                "dhgroup" => 14
            ]
        ]
    ];
    const BASE_P2_DATA = [
        "iketype" => "ikev2",
        "protocol" => "inet",
        "interface" => "wan",
        "remote_gateway" => "1.2.3.4",
        "authentication_method" => "pre_shared_key",
        "myid_type" => "myaddress",
        "peerid_type" => "peeraddress",
        "pre_shared_key" => "testkey",
        "encryption" => [
            [
                "encryption_algorithm_name" => "aes",
                "encryption_algorithm_keylen" => 256,
                "hash_algorithm" => "sha256",
                "dhgroup" => 14
            ]
        ]
    ];
}