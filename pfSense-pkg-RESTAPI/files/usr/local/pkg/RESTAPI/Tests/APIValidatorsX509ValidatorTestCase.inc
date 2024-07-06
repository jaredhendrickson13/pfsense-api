<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\X509Validator;

class APIValidatorsX509ValidatorTestCase extends TestCase {
    const EXAMPLE_CRT = "-----BEGIN CERTIFICATE-----
MIIDnzCCAoegAwIBAgIUd9R9uoNKE2GoxgJPwzRn67WV4bgwDQYJKoZIhvcNAQEL
BQAwXzELMAkGA1UEBhMCVVMxDTALBgNVBAgMBFV0YWgxFzAVBgNVBAcMDlNhbHQg
TGFrZSBDaXR5MRIwEAYDVQQKDAlBY21lIEluYy4xFDASBgNVBAMMC2V4YW1wbGUu
Y29tMB4XDTIzMTIyMzAxNTIzMFoXDTMzMTIyMDAxNTIzMFowXzELMAkGA1UEBhMC
VVMxDTALBgNVBAgMBFV0YWgxFzAVBgNVBAcMDlNhbHQgTGFrZSBDaXR5MRIwEAYD
VQQKDAlBY21lIEluYy4xFDASBgNVBAMMC2V4YW1wbGUuY29tMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsOO3ABmSowO9LxYb4nN6QSaj2wrEJLyyXW0s
w3wPYkskQ+LFSCK1nBfJJXzPL3JqeBYksIppjyVCBWHSjbDN4DCy4zWheBWiNX0N
MMEyRgCefcmyJhw0GCcG3LQdB4lwlNOQiNQ7cd7H7Tl4ufIWaq4GZeJlF0VDvwb7
suDvhEB1UgtXIaAy2+JAFp+hV/x5PcaKnppu2OgWMEGXY0pMC3cRq/cfPvDolrQI
rlOxE1+XTlL7xylu/SVg/aHCHjgRKa9G4mpMQuDMnvn6TaJZRwF0K+jmCpjk6qTY
Uqxuc+Ua1OMCqUHO3fVEY0s+ULkVL5nFItaaUGdKcrPn8hEvOwIDAQABo1MwUTAd
BgNVHQ4EFgQUR5oK7sWvKs8QOsNnnRXvVGxvLkgwHwYDVR0jBBgwFoAUR5oK7sWv
Ks8QOsNnnRXvVGxvLkgwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOC
AQEAYCfLDfnHTKWipng64tNWAvwgiIj2IsdfPibypgJiuWAY1kDL6Vz328N6Jr2T
Ljs2M8HlP+Xo8CxSx2I2NHSWhad8eFKOX4AhOjTk0fDMDBe1AlRuLTNlyBDdWAMI
7B5RbeCuIlHqwy7DefTtzVkHHoG8vB/hY7jukOIlxnyjmwTbFNBHmeo7+BmWhxQ2
5JPfqJelyprR6OA+CpZhu4bLjwS1J6iOaVMOgnvzGVHLD5o7cFv+bLoc/VyfTn2v
cU/K9lj+3VxX+W3Vj7L7xSY9UnGfF2trcokXIzNirVHEPDLUQDWZ1YyqQBHPBYVV
BL1l1UjlU4Vb8ubjQ1/ISq95dw==
-----END CERTIFICATE-----";
    const EXAMPLE_PRV = "-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCw47cAGZKjA70v
Fhvic3pBJqPbCsQkvLJdbSzDfA9iSyRD4sVIIrWcF8klfM8vcmp4FiSwimmPJUIF
YdKNsM3gMLLjNaF4FaI1fQ0wwTJGAJ59ybImHDQYJwbctB0HiXCU05CI1Dtx3sft
OXi58hZqrgZl4mUXRUO/Bvuy4O+EQHVSC1choDLb4kAWn6FX/Hk9xoqemm7Y6BYw
QZdjSkwLdxGr9x8+8OiWtAiuU7ETX5dOUvvHKW79JWD9ocIeOBEpr0biakxC4Mye
+fpNollHAXQr6OYKmOTqpNhSrG5z5RrU4wKpQc7d9URjSz5QuRUvmcUi1ppQZ0py
s+fyES87AgMBAAECggEAAeQ3wDxO6VDnRsNDQlr+wgbf9mKU31mxe22NpXwNn3zz
NUCgKZCpczLqxTXHO/OT5gSwT+PQuG7GfL0++Q+cktGifcYyQFD5MLUWr4T96o62
IyZXxGazO56Y7a/zE817pTPCOtMwb0B+PcuBLFWOlM68ATVTm0Lhj6RLQpMNmlMN
2w/AKGTzK4e4mzsgH/8N2b9uwgCtrDeB1HWQ8nKC9ZGeL0ZYmNvVKo5Umlsh+Y/1
PRwmaGciaap9Rrki4fHM+tHU33roIC3B6MRLdma0sLE4UoJ6IGgApL2ox4gX+qJy
qvitGzu9Oq3f0ZVuXM/Tk9zAIiZhFiY3ISNRvDppgQKBgQDfji4HO8/4geKhLf8K
UZxfpkVxTLj4J8b6T0olPsfe4Cj3vJu3PCjn3bT8+n6Px72Ub6HMbomSRZAnJUW2
i5VNWRT6NQhcLN566n9O4HJLoT7OkXoJ1SRAT6X3hsxcyZtjDNGgIhsOJ64dHce/
uu161aB1cqj0Jk1yoJiTfcEOuwKBgQDKj75jzlenbaJduCp3dbpOyjRHUdNjNmZR
Ja6GC/iO54NRKUXLt18gEzpFdPCxrQl9t7/lHi+6m6WU1Sfss3+Xxx8fIp8Pwggh
w1R2uZrzHzRhtAaEG9ZRTjJ7pEk/I/YBQ2OTdaWoUZKdl+B1O7sgWWJj16cUHFFf
MYWnVcGZgQKBgQCqFD2oezTqV8bX4WqbqyNREerbHAfNpFWZsiEy3OSrB9ejQvcl
HLvyS5aasOkA8nGlMJjkM2SuyAW4An9bNi7Mve1bvmUx22EYGrfohpHuQCrMdZln
SaVe2Dnoa8GnBisV3HbPYaM7zayFslZ54/kw7r45HJa1yT466zupWo86aQKBgQC7
t2ZbG9gDYwh7DbbdKg7UAXNdPKlHYhyDywXnV6Sq2kXJqxPxbCgNk4QG5g5Aufj3
tpdw/v6AUsmqGbd8FgSCZZejxPPfKJNcpqXZuLut3xcuDCWVvC5q/qquBluS1gKe
vRLEjtxMFfxna14phoctvWf+2tUNQAjwzPXtUv2tAQKBgAUsNWjqUE4WnI/Gu0II
bSIrTCXwoMS91HTKG5P1OhjHP7WH5hhKOlWc8SFPMn1ivu760n7F1qBkj4BOiDvy
4qYNpDCL82eUEfRtCiFmfsERhOnXRcaud8zQj/fNIOcvx5qADI1ErAJBzgp5ZGJp
E0InZI+5q1RzueYy8+j0FX/o
-----END PRIVATE KEY-----";
    const EXAMPLE_ECPRV = "-----BEGIN EC PRIVATE KEY-----
MHcCAQEEIM52UGyF/b3kF1wcPGeEf0/npV7YpgUJBhU4zsOfRowvoAoGCCqGSM49
AwEHoUQDQgAE1RtITpFIce3jjv2AkBqYaVRU0swODWO+DREC/xsYnaNSyVGT5hy6
R02Pul8ulWQ8Kl3Q3pou8As7W1mMzA2DxQ==
-----END EC PRIVATE KEY-----";
    const EXAMPLE_CSR = "-----BEGIN CERTIFICATE REQUEST-----
MIICpDCCAYwCAQAwXzELMAkGA1UEBhMCVVMxDTALBgNVBAgMBFV0YWgxFzAVBgNV
BAcMDlNhbHQgTGFrZSBDaXR5MRIwEAYDVQQKDAlBY21lIEluYy4xFDASBgNVBAMM
C2V4YW1wbGUuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvn2p
JfcUDhmnGOZcD1TYaTyAPFCwE11Nt93sKADQPrL5i4V8aWppOM4Rk4czu0/AN0hC
5xdind5JFcPMsTCnmRL3fmnwQzrA76MrsL7pEhdN17m/6WXEW8VPycO+POX4dBt/
7vk2G4oVsOVZCzWe0y4sdA58zFI6sDszo2oZr25rY8CYi1MQ7ThiXTGxXgzKVi5v
jGtqe3rBgKAGv1ofKCO/KtCvJglJsN2I5GJ08irLCYj19eT2u7j7CAW9x7VTB0s2
0XxM7PL0KN7NQmHG5tS+190LobBe1C9IgBid7IKlPof14mTWaEE6I0kZ8Yx1H2XF
bwZeGJWjXvqQsSlwnwIDAQABoAAwDQYJKoZIhvcNAQELBQADggEBACyHKyMKkEiW
6wB0D5ymsmdetGKKx8NG4LaW8yBHUkm8k9DIEDeZrVo4BvLmMHQKjJTEzLlXZ2ed
P3yxJiM9YmbrSQYpuX0tOSKKgrYfJl80Oo34PxjIxUdNuGxocRJUm7Uxr4G8EpEp
p57b2YDRMJU5rJCv5evkYbUDMOjyZ07s0hw3W7U+6CrQWaSuTNq8tWEHhnh0wnIT
CJbowbf6nJjngLWXgKviZsIO/dFeO0lNb+4Qup8nOEX7TvrmLaV/YvKOdiTj6bOy
H0t+b8M07WqCw6FuSkg/UcMgMMshSooVSIemL+YhK7tYQ9oQKcVdy34Xt69NDh2T
+tDO4ISrm0s=
-----END CERTIFICATE REQUEST-----";

    /**
     * Checks that the `allow_crt` parameter allows X509 encoded certificate strings.
     */
    public function test_allow_crt() {
        # Ensure valid X509 certificate strings are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $validator = new X509Validator(allow_crt: true);
                $validator->validate(self::EXAMPLE_CRT);
            },
        );

        # Ensure non X509 certificate strings are rejected
        $this->assert_throws_response(
            response_id: 'X509_VALIDATOR_INVALID_VALUE',
            code: 400,
            callable: function () {
                $validator = new X509Validator(allow_crt: true);
                $validator->validate('not a certificate!');
            },
        );
    }

    /**
     * Checks that the `allow_prv` parameter allows X509 encoded private key strings.
     */
    public function test_allow_prv() {
        # Ensure valid X509 private key strings are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $validator = new X509Validator(allow_crt: false, allow_prv: true);
                $validator->validate(self::EXAMPLE_PRV);
            },
        );

        # Ensure non X509 private key strings are rejected
        $this->assert_throws_response(
            response_id: 'X509_VALIDATOR_INVALID_VALUE',
            code: 400,
            callable: function () {
                $validator = new X509Validator(allow_prv: true);
                $validator->validate('not a private key!');
            },
        );
    }

    /**
     * Checks that the `allow_ecprv` parameter allows X509 encoded EC private key strings.
     */
    public function test_allow_ecprv() {
        # Ensure valid X509 EC private key strings are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $validator = new X509Validator(allow_crt: false, allow_ecprv: true);
                $validator->validate(self::EXAMPLE_ECPRV);
            },
        );

        # Ensure non X509 EC private key strings are rejected
        $this->assert_throws_response(
            response_id: 'X509_VALIDATOR_INVALID_VALUE',
            code: 400,
            callable: function () {
                $validator = new X509Validator(allow_ecprv: true);
                $validator->validate('not an EC private key!');
            },
        );
    }

    /**
     * Checks that the `allow_csr` parameter allows X509 encoded CSR strings.
     */
    public function test_allow_csr() {
        # Ensure valid X509 CSRs are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $validator = new X509Validator(allow_crt: false, allow_csr: true);
                $validator->validate(self::EXAMPLE_CSR);
            },
        );

        # Ensure non X509 EC CSR strings are rejected
        $this->assert_throws_response(
            response_id: 'X509_VALIDATOR_INVALID_VALUE',
            code: 400,
            callable: function () {
                $validator = new X509Validator(allow_csr: true);
                $validator->validate('not a CSR!');
            },
        );
    }

    /**
     * Checks that the `is_matching_keypair()` correctly identifies matching keypairs.
     */
    public function test_is_matching_key_pair() {
        # Ensure matching key pairs return `true`
        $this->assert_is_true(
            X509Validator::is_matching_keypair(public_key: self::EXAMPLE_CRT, private_key: self::EXAMPLE_PRV),
        );

        # Ensure non matching key pairs return `false`
        $this->assert_is_false(
            X509Validator::is_matching_keypair(public_key: self::EXAMPLE_CRT, private_key: self::EXAMPLE_ECPRV),
        );
    }
}
