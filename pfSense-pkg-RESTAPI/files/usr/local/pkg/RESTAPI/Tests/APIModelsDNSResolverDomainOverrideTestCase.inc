<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSResolverDomainOverride;

class APIModelsDNSResolverDomainOverrideTestCase extends TestCase {
    /**
     * Checks that we are able to create, update and delete DNSResolverDomainOverride objects.
     */
    public function test_crud(): void {
        # Create a new domain override
        $domain_override = new DNSResolverDomainOverride(
            domain: 'test.example.com',
            ip: '8.8.8.8',
            descr: 'Test domain override',
            forward_tls_upstream: true,
            tls_hostname: 'example.com',
            async: false,
        );
        $domain_override->create(apply: true);

        # Ensure it is found in /var/unbound/domainoverrides.conf
        $domainoverrides_conf = file_get_contents('/var/unbound/domainoverrides.conf');
        $this->assert_str_contains($domainoverrides_conf, 'name: "test.example.com"');
        $this->assert_str_contains($domainoverrides_conf, 'forward-tls-upstream: yes');
        $this->assert_str_contains($domainoverrides_conf, 'forward-addr: 8.8.8.8@853#example.com');

        # Update the domain override
        $domain_override->from_representation(
            domain: 'newtest.example.com',
            ip: '1.1.1.1',
            descr: 'New test domain override',
            forward_tls_upstream: false,
            async: false,
        );
        $domain_override->update(apply: true);

        # Ensure it is found in /var/unbound/domainoverrides.conf
        $domainoverrides_conf = file_get_contents('/var/unbound/domainoverrides.conf');
        $this->assert_str_does_not_contain($domainoverrides_conf, 'name: "test.example.com');
        $this->assert_str_contains($domainoverrides_conf, 'name: "newtest.example.com');
        $this->assert_str_does_not_contain($domainoverrides_conf, 'forward-tls-upstream: yes');
        $this->assert_str_contains($domainoverrides_conf, 'forward-addr: 1.1.1.1');

        # Delete the domain override
        $domain_override->delete(apply: true);
        $domainoverrides_conf = file_get_contents('/var/unbound/domainoverrides.conf');
        $this->assert_str_does_not_contain($domainoverrides_conf, 'name: "test.example.com"');
        $this->assert_str_does_not_contain($domainoverrides_conf, 'name: "newtest.example.com"');
    }
}
