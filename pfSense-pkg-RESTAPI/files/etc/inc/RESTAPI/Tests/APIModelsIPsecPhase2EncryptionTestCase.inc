<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecPhase1Encryption;
use RESTAPI\Models\IPsecPhase2Encryption;

class APIModelsIPsecPhase2EncryptionTestCase extends TestCase
{
    # NOTE: This model is partially tested by /RESTAPI/Tests/APIModelsIPsecPhase2TestCase
    
    /**
     * Checks that the `keylen` must be valid for the given encryption algorithm `name`.
     */
    public function test_keylen_choices(): void {
        # Ensure an error is thrown if the requested key length is not supported for the given encryption algorithm
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_2_ENCRYPTION_ALGORITHM_KEYLEN_INVALID_CHOICE",
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2Encryption();
                $p2->name->value = "aes";
                $p2->validate_keylen(257);
            }
        );

        # Ensure no error is thrown when the key length is supported for the given encryption algorithm
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2Encryption();
                $p2->name->value = "aes";
                $p2->validate_keylen(256);
            }
        );
    }

    /**
     * Checks that the `get_supported_keylens()` method throws a ValidationError if the provided $name is not a
     * supported encryption algorithm
     */
    public function test_get_support_keylens_name_must_support_key_lengths(): void {
        # Ensure an error is not thrown if `get_supported_keylens()` receives a valid `name`
        $this->assert_does_not_throw(
            callable: function () {
                $keylen_enabled_algo = IPsecPhase2Encryption::get_keylen_enabled_algos()[0];
                IPsecPhase2Encryption::get_supported_keylens(name: $keylen_enabled_algo);
            }
        );

        # Ensure an error is thrown if `get_supported_keylens()` receives an invalid `name`
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_2_ENCRYPTION_COULD_NOT_GET_KEYLENS_FOR_UNKNOWN_ALGO",
            code: 400,
            callable: function () {
                IPsecPhase2Encryption::get_supported_keylens(name: "not a valid encryption algorithm");
            }
        );
    }

    /**
     * Checks that the `get_supported_keylens()` correctly obtains each possible key length option for the provide
     * encryption algorithm name.
     */
    public function test_get_supported_keylens(): void {
        global $p2_ealgos;

        # Loop each encryption algorithm that supported variable key lengths
        foreach (IPsecPhase2Encryption::get_keylen_enabled_algos() as $algo) {
            # Loop through each key length option available to this algorithm and ensure it meets the correct parameters
            foreach (IPsecPhase2Encryption::get_supported_keylens(name: $algo) as $keylen) {
                # Obtain the parameters for supported key lengths
                $min_keylen = $p2_ealgos[$algo]["keysel"]["lo"];
                $max_keylen = $p2_ealgos[$algo]["keysel"]["hi"];
                $keylen_increment = $p2_ealgos[$algo]["keysel"]["step"];

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
        global $p2_ealgos;

        # Loop through each algorithm name returned by get_keylen_enabled_algos()
        foreach (IPsecPhase2Encryption::get_keylen_enabled_algos() as $algo) {
            # Ensure the identified encryption algorithm supports variables key lengths
            $this->assert_is_true(array_key_exists("keysel", $p2_ealgos[$algo]));
        }
    }

}
