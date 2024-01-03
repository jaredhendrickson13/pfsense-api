<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecPhase1;
use RESTAPI\Models\IPsecPhase1Encryption;

class APIModelsIPsecPhase1EncryptionTestCase extends TestCase
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
    
    /**
     * Checks that the `encryption_algorithm_keylen` field can only be assigned key lengths that are supported by the
     * `encryption_algorithm_name.
     */
    public function test_validate_encryption_algorithm_keylen(): void {
        # Create a p1 and p1 encryption entry to test with
        $p1 = new IPsecPhase1(data: self::BASE_P1_DATA);
        $p1->create();
        
        # Ensure valid encryption algorithm key lengths are accepted
        $this->assert_does_not_throw(
            callable: function () use ($p1) {
                $p1_enc = new IPsecPhase1Encryption(data: [
                    "parent_id" => $p1->id,
                    "encryption_algorithm_name" => "aes",
                    "encryption_algorithm_keylen" => 256,
                    "hash_algorithm" => "sha256",
                    "dhgroup" => 14
                ]);
                $p1_enc->validate();
            }
        );

        # Ensure invvalid encryption algorithm key lengths throw an error
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_ENCRYPTION_ALGORITHM_KEYLEN_INVALID_CHOICE",
            code: 400,
            callable: function () use ($p1) {
                $p1_enc = new IPsecPhase1Encryption(data: [
                    "parent_id" => $p1->id,
                    "encryption_algorithm_name" => "aes",
                    "encryption_algorithm_keylen" => 300,
                    "hash_algorithm" => "sha256",
                    "dhgroup" => 14
                ]);
                $p1_enc->validate();
            }
        );
        
        # Delete the p1
        $p1->delete();
    }
    
    /**
     * Checks that the `get_supported_keylens()` method throws a ValidationError if the provided $name is not a
     * supported encryption algorithm
     */
    public function test_get_support_keylens_name_must_support_key_lengths(): void {
        # Ensure an error is not thrown if `get_supported_keylens()` receives a valid `name`
        $this->assert_does_not_throw(
            callable: function () {
                $keylen_enabled_algo = IPsecPhase1Encryption::get_keylen_enabled_algos()[0];
                IPsecPhase1Encryption::get_supported_keylens(name: $keylen_enabled_algo);
            }
        );

        # Ensure an error is thrown if `get_supported_keylens()` receives an invalid `name`
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE1_ENCRYPTION_COULD_NOT_GET_KEYLENS_FOR_UNKNOWN_ALGO",
            code: 400,
            callable: function () {
                IPsecPhase1Encryption::get_supported_keylens(name: "not a valid encryption algorithm");
            }
        );
    }
    
    /**
     * Checks that the `get_supported_keylens()` correctly obtains each possible key length option for the provide
     * encryption algorithm name. 
     */
    public function test_get_supported_keylens(): void {
        global $p1_ealgos;
        
        # Loop each encryption algorithm that supported variable key lengths
        foreach (IPsecPhase1Encryption::get_keylen_enabled_algos() as $algo) {
            # Loop through each key length option available to this algorithm and ensure it meets the correct parameters
            foreach (IPsecPhase1Encryption::get_supported_keylens(name: $algo) as $keylen) {
                # Obtain the parameters for supported key lengths
                $min_keylen = $p1_ealgos[$algo]["keysel"]["lo"];
                $max_keylen = $p1_ealgos[$algo]["keysel"]["hi"];
                $keylen_increment = $p1_ealgos[$algo]["keysel"]["step"];
                
                # Ensure this key length option is greater than or equal to the minimum key length supported
                $this->assert_is_greater_than_or_equal($keylen, $min_keylen);
                
                # Ensure this key length option is less than or equal to the maximum key length supported
                $this->assert_is_less_than_or_equal($keylen, $max_keylen);
                
                # Ensure this key length is a valid increment by ensure it is wholly divisible by the increment value
                $this->assert_equals(($keylen % $keylen_increment), 0);
            }
        } 
    }
    
    /**
     * Checks that `get_keylen_enabled_algos()` only returns the name of encryption algorithms that support variable
     * key lengths.
     */
    public function test_get_keylen_enabled_algos(): void {
        global $p1_ealgos;
        
        # Loop through each algorithm name returned by get_keylen_enabled_algos()
        foreach (IPsecPhase1Encryption::get_keylen_enabled_algos() as $algo) {
            # Ensure the identified encryption algorithm supports variables key lengths
            $this->assert_is_true(array_key_exists("keysel", $p1_ealgos[$algo]));
        }
    }
    
    /**
     * Checks that the encryption algorithm is correctly represented in swanctl when the IPsecPhase1Encryption object
     * is created, updated, and deleted.
     */
    public function test_crud(): void {
        # Create a p1 and p1 encryption entry to test with
        $p1 = new IPsecPhase1(data: self::BASE_P1_DATA, async: false);
        $p1->encryption->value = [
            ["encryption_algorithm_name" => "aes", "encryption_algorithm_keylen" => 256, "hash_algorithm" => "sha256", "dhgroup" => 14],
            ["encryption_algorithm_name" => "chacha20poly1305", "hash_algorithm" => "sha512", "dhgroup" => 24]
        ];
        $p1->create(apply: true);
        
        # Ensure the expected `proposals` line is present in swanctl.conf
        $swanctl = file_get_contents("/var/etc/ipsec/swanctl.conf");
        $this->assert_str_contains($swanctl, $this->get_expected_proposals(p1: $p1));
        
        # Enable prf on the p1 and ensure the prf values are also included in swanctl
        $p1->prfselect_enable->value = true;
        $p1->update(apply: true);
        $swanctl = file_get_contents("/var/etc/ipsec/swanctl.conf");
        $this->assert_str_contains($swanctl, $this->get_expected_proposals(p1: $p1));
        
        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Generates the expected swanctl.conf `proposals` line for a given IPsecPhase1 object.
     * @param IPsecPhase1 $p1 The IPsecPhase1 object to use to generate the swanctl proposal
     * @return string The swanctl.conf `proposals` line expected from the IPsecPhase1 object
     */
    private function get_expected_proposals(IPsecPhase1 $p1): string {
        # Init an array to store the swanctl propoasl values (e.g. aes128-sha256-modp2048)
        $proposals = [];
        
        # Loop through each p1 'encryption' IPsecPhase1Encryption item and add its proposal
        foreach ($p1->encryption->value as $p1_enc) {
            # Obtain the relevant IPsecPhase1Encryption values from this object
            $enc_name = $p1_enc["encryption_algorithm_name"];
            $enc_keylen = $p1_enc["encryption_algorithm_keylen"];
            $hash = $p1_enc["hash_algorithm"];
            $dhgroup = ipsec_convert_to_modp($p1_enc["dhgroup"]);
            $prf_algorithm = $p1_enc["prf_algorithm"];
            
            # Piece together the swanctl proposal item for this IPsecPhase1Encryption object
            if ($enc_keylen and $p1->prfselect_enable->value) {
                $proposals[] = "$enc_name$enc_keylen-$hash-prf$prf_algorithm-$dhgroup";
            }
            elseif (!$enc_keylen and $p1->prfselect_enable->value) {
                $proposals[] = "$enc_name-$hash-prf$prf_algorithm-$dhgroup";
            }
            else {
                $proposals[] = "$enc_name$enc_keylen-$hash-$dhgroup";
            }
        }

        # Piece our proposal items together into the single `proposals` line
        return "proposals = ".implode(",", $proposals).PHP_EOL;
    }

}
