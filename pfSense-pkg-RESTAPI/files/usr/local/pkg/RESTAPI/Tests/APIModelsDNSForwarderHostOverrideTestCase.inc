<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\Model;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSForwarderHostOverride;

class APIModelsDNSForwarderHostOverrideTestCase extends TestCase {
    public function test_crud(): void {
        # Ensure the DNS Forwarder is enabled before starting
        Model::set_config(path: 'dnsmasq/enable', value: '');

        # Create a new DNSForwarderHostOverride object and ensure it is present in /etc/hosts after
        $dns_forwarder_host_override = new DNSForwarderHostOverride(
            host: 'test',
            domain: 'example.com',
            ip: '1.2.3.4',
            description: 'Test DNSForwarderHostOverride object',
            aliases: [
                [
                    'host' => 'test-alias',
                    'domain' => 'example.com',
                    'description' => 'Test DNSForwarderHostOverrideAlias object',
                ],
            ],
            async: false,
        );
        $dns_forwarder_host_override->create(apply: true);
        $etc_hosts = new Command('cat /etc/hosts', trim_whitespace: true);
        $this->assert_str_contains($etc_hosts->output, '1.2.3.4 test.example.com');
        $this->assert_str_contains($etc_hosts->output, '1.2.3.4 test-alias.example.com');

        # Update the DNSForwarderHostOverride object and ensure the changes are present in /etc/hosts after
        $dns_forwarder_host_override->from_representation(
            host: 'newtest',
            domain: 'example.com',
            ip: '4.3.2.1',
            description: 'New Test DNSForwarderHostOverride object',
            aliases: [
                [
                    'host' => 'newtest-alias',
                    'domain' => 'example.com',
                    'description' => 'New Test DNSForwarderHostOverrideAlias object',
                ],
            ],
        );
        $dns_forwarder_host_override->update(apply: true);
        $etc_hosts = new Command('cat /etc/hosts', trim_whitespace: true);
        $this->assert_str_contains($etc_hosts->output, '4.3.2.1 newtest.example.com');
        $this->assert_str_contains($etc_hosts->output, '4.3.2.1 newtest-alias.example.com');

        # Delete the DNSForwarderHostOverride object and ensure it is not present in /etc/hosts after
        $dns_forwarder_host_override->delete(apply: true);
        $etc_hosts = new Command('cat /etc/hosts', trim_whitespace: true);
        $this->assert_str_does_not_contain($etc_hosts->output, '4.3.2.1 newtest.example.com');
        $this->assert_str_does_not_contain($etc_hosts->output, '4.3.2.1 newtest-alias.example.com');
    }
}
