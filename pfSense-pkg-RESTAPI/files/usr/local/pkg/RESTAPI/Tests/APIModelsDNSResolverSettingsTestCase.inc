<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\DNSResolverSettings;

class APIModelsDNSResolverSettingsTestCase extends TestCase {
    /**
     * Checks that the `enable` field correctly controls the unbound (DNS Resolver) service.
     */
    public function test_enable(): void {
        $settings = new DNSResolverSettings(enable: false, async: false);
        $settings->update(apply: true);
        $pid = (int) file_get_contents('/var/run/unbound.pid');
        $this->assert_is_false(posix_kill($pid, signal: 0)); // signal 0 does not actually kill the process

        $settings->enable->value = true;
        $settings->update(apply: true);
        $pid = (int) file_get_contents('/var/run/unbound.pid');
        $this->assert_is_true(posix_kill($pid, signal: 0)); // signal 0 does not actually kill the process
    }

    /**
     * Checks that the `port` field correctly controls the port on which the unbound (DNS Resolver) service listens.
     */
    public function test_port(): void {
        $settings = new DNSResolverSettings(port: '5353', async: false);
        $settings->update(apply: true);
        $unbound_config = file_get_contents('/var/unbound/unbound.conf');
        $this->assert_str_contains($unbound_config, 'port: 5353');

        $settings = new DNSResolverSettings(port: '53', async: false);
        $settings->update(apply: true);
        $unbound_config = file_get_contents('/var/unbound/unbound.conf');
        $this->assert_str_contains($unbound_config, 'port: 53');
    }

    /**
     * Checks that the `tlsport` field correctly controls the tlsport on which the unbound (DNS Resolver) service listens.
     */
    public function test_tlsport(): void {
        # Obtain the current webGUI cert
        $cert = new Certificate(id: 0);

        $settings = new DNSResolverSettings(
            tlsport: '8353',
            sslcertref: $cert->refid->value,
            enablessl: true,
            async: false,
        );
        $settings->update(apply: true);
        $unbound_config = file_get_contents('/var/unbound/unbound.conf');
        $this->assert_str_contains($unbound_config, 'tls-port: 8353');

        $settings = new DNSResolverSettings(
            tlsport: '853',
            enablessl: true,
            sslcertref: $cert->refid->value,
            async: false,
        );
        $settings->update(apply: true);
        $unbound_config = file_get_contents('/var/unbound/unbound.conf');
        $this->assert_str_contains($unbound_config, 'tls-port: 853');
    }

    /**
     * Checks that the `custom_options` field correctly includes custom configuration in unbound.conf.
     */
    public function test_custom_options(): void {
        $settings = new DNSResolverSettings(custom_options: 'test: test', async: false);
        $settings->update(apply: true);
        $unbound_config = file_get_contents('/var/unbound/unbound.conf');
        $this->assert_str_contains($unbound_config, 'test: test');
    }

    /**
     * Checks that the `get_python_script_options` method correctly retrieves the available Python scripts.
     */
    public function test_get_python_script_options(): void {
        # Add a mock script to /var/unbound/ to populate an option
        touch('/var/unbound/test_option1.py');
        $settings = new DNSResolverSettings(async: false);
        $this->assert_equals($settings->get_python_script_options(), ['test_option1']);
    }
}
