<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDSettings;

class APIModelsBINDSettingsTestCase extends TestCase {
    const BIND_CONFIG_PATH = '/var/etc/named/etc/namedb/named.conf';
    public array $required_packages = ['pfSense-pkg-bind'];

    /**
     * Checks that the 'listenon' field correctly controls the 'listen-on' settings in the BIND configuration.
     */
    public function test_listenon(): void {
        # Ensure 'All' is accepted and represents 'any' in the BIND configuration
        $bind_settings = new BINDSettings(listenon: ['All'], async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'listen-on port 53 { any; };',
        );

        # Change 'listenon' to 'lan' and ensure it is correctly represented in the BIND configuration
        $bind_settings->listenon->value = ['lan'];
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'listen-on port 53 { 192.168.1.1; };',
        );
    }

    /**
     * Checks that the bind_notify field correctly controls the 'notify' settings in the BIND configuration.
     */
    public function test_bind_notify(): void {
        $bind_settings = new BINDSettings(bind_notify: true, async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'notify yes;',
        );

        $bind_settings->bind_notify->value = false;
        $bind_settings->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'notify yes;',
        );
    }

    /**
     * Checks that the 'bind_hide_version' field correctly controls the 'version' settings in the BIND configuration.
     */
    public function test_bind_hide_version(): void {
        $bind_settings = new BINDSettings(bind_hide_version: true, async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'version none;',
        );

        $bind_settings->bind_hide_version->value = false;
        $bind_settings->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'version none;',
        );
    }

    /**
     * Checks that the 'bind_ram_limit' field correctly controls the 'max-cache-size' settings in the BIND configuration.
     */
    public function test_bind_ram_limit(): void {
        $bind_settings = new BINDSettings(bind_ram_limit: '128M', async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'max-cache-size 128M;',
        );

        $bind_settings->bind_ram_limit->value = '256M';
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'max-cache-size 256M;',
        );
    }

    /**
     * Checks that the rate limit fields correctly control the 'rate-limit' settings in the BIND configuration.
     */
    public function test_rate_limit(): void {
        # Ensure we can disabled rate limiting
        $bind_settings = new BINDSettings(rate_enabled: false, async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'rate-limit',
        );

        # Ensure we can enable rate limiting
        $bind_settings->rate_enabled->value = true;
        $bind_settings->rate_limit->value = 100;
        $bind_settings->update(apply: true);
        $bind_conf = (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output;
        $this->assert_str_contains($bind_conf, 'rate-limit');
        $this->assert_str_contains($bind_conf, 'responses-per-second 100;');

        # Ensure we can enable log only rate limiting
        $bind_settings->log_only->value = true;
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'log-only yes;',
        );

        # Ensure we can disable log only rate limiting
        $bind_settings->log_only->value = false;
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'log-only no;',
        );
    }

    /**
     * Check that the forwarder fields correctly control the 'forwarders' settings in the BIND configuration.
     */
    public function test_forwarder(): void {
        # Ensure we can disable forwarding
        $bind_settings = new BINDSettings(bind_forwarder: false, async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'forwarders',
        );

        # Ensure we can enable forwarding
        $bind_settings->bind_forwarder->value = true;
        $bind_settings->bind_forwarder_ips->value = ['127.0.0.1', '127.0.0.2'];
        $bind_settings->update(apply: true);
        $bind_conf = (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output;
        $this->assert_str_contains($bind_conf, 'forwarders { 127.0.0.1;127.0.0.2; };');
    }

    /**
     * Checks that the 'bind_dnssec_validation' field correctly controls the 'dnssec-validation' settings in the BIND configuration.
     */
    public function test_bind_dnssec_validation(): void {
        $bind_settings = new BINDSettings(bind_dnssec_validation: 'auto', async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'dnssec-validation auto;',
        );

        $bind_settings->bind_dnssec_validation->value = 'on';
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'dnssec-validation yes;',
        );

        $bind_settings->bind_dnssec_validation->value = 'off';
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'dnssec-validation no;',
        );
    }

    /**
     * Checks that the 'listenport' field correctly controls the 'listen-on' settings in the BIND configuration.
     */
    public function test_listenport(): void {
        $bind_settings = new BINDSettings(listenport: '5353', async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'listen-on port 5353',
        );

        $bind_settings->listenport->value = '53';
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'listen-on port 53',
        );
    }

    /**
     * Checks that the 'controlport' field correctly controls the 'controls' settings in the BIND configuration.
     */
    public function test_controlport(): void {
        $bind_settings = new BINDSettings(controlport: '953', async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'inet 127.0.0.1 port 953',
        );

        $bind_settings->controlport->value = '8953';
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'inet 127.0.0.1 port 8953',
        );
    }

    /**
     * Checks that the 'bind_custom_options' field correctly controls the 'options' settings in the BIND configuration.
     */
    public function test_bind_custom_options(): void {
        $bind_settings = new BINDSettings(bind_custom_options: 'example custom option;', async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'example custom option;',
        );

        $bind_settings->bind_custom_options->value = '';
        $bind_settings->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'example custom option;',
        );
    }

    /**
     * Checks that the 'bind_global_settings' field correctly controls the 'options' settings in the BIND configuration.
     */
    public function test_bind_global_settings(): void {
        $bind_settings = new BINDSettings(bind_global_settings: 'example global setting;', async: false);
        $bind_settings->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'example global setting;',
        );

        $bind_settings->bind_global_settings->value = '';
        $bind_settings->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat ' . self::BIND_CONFIG_PATH, trim_whitespace: true))->output,
            'example global setting;',
        );
    }
}
