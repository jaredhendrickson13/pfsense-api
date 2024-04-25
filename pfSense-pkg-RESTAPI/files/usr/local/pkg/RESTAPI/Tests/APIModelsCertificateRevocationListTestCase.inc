<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\CertificateRevocationList;

class APIModelsCertificateRevocationListTestCase extends TestCase
{
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
    const EXAMPLE_CRL = "-----BEGIN X509 CRL-----
MIIB2jCBwwIBATANBgkqhkiG9w0BAQUFADAWMRQwEgYDVQQDEwtpbnRlcm5hbC1j
YRcNMjQwNDI0MjIwMjE1WhcNMjYwNDI0MjIwMjE1WjAiMCACAQIXDTI0MDQyNDIy
MDIxNVowDDAKBgNVHRUEAwoBCaBVMFMwRQYDVR0jBD4wPIAUJUzC5CblbCq8FU3k
hWe8eqZlPZehGqQYMBYxFDASBgNVBAMTC2ludGVybmFsLWNhgggtyC2VhQuYKjAK
BgNVHRQEAwIBAzANBgkqhkiG9w0BAQUFAAOCAQEAXqs/3gBtZFxt5sKVj/ek1N1J
tAFEc18IMOaWVpc+HasQ5Kvi04ygfE0BWq5oXT9xi7aQfLZLHV2kpk8thdo8DMRo
ezyZg2VnwovVPwmV/TnpXsRehBvo4pXXVq+REIhKLQW1SgfqfftEhEe0B2B5eK0U
IVN7BoJK2k0uypahNxUR1ob1LXtr74j1nfH5wLLF6nitwX6ujj2O9ge6zAQS8eBN
sJ2heugpAOP3tLQrMgGw6M9XBPjVBsnErVgRu11OJnUttL7kUgWcrTlaOYhPqvYx
O2KYIqqr1w7O0epy0oKKZSHlwQoIOy8S1CNCIrFiYx4BJ9Gq34cjZ/OpkQPijQ==
-----END X509 CRL-----";

    
    /*
     * Ensures CRLs can be created and deleted
     */
    public function test_create_and_delete(): void {
        # Create a Certificate Authority to test with
        $ca = new CertificateAuthority(descr: 'test_ca', crt: self::EXAMPLE_CRT, prv: self::EXAMPLE_PRV);
        $ca->create();
        
        # Create internal and existing CRLs
        $internal_crl = new CertificateRevocationList(
            descr: 'test_internal_ca',
            caref: $ca->refid->value,
            method: 'internal',
            lifetime: 730,
            serial: 1
        );
        $internal_crl->create();

        $existing_crl = new CertificateRevocationList(
            descr: 'test_existing_ca',
            caref: $ca->refid->value,
            method: 'existing',
            text: self::EXAMPLE_CRL
        );
        $existing_crl->create();
        
        # Ensure we can now read two different CRLs
        $this->assert_equals(count(CertificateRevocationList::read_all()->model_objects), 2);

        # Delete the CRLs
        $existing_crl->delete();
        $internal_crl->delete();
        $ca->delete();
        
        # Ensure there are no more CRLs
        $this->assert_equals(count(CertificateRevocationList::read_all()->model_objects), 0);
    }
}
