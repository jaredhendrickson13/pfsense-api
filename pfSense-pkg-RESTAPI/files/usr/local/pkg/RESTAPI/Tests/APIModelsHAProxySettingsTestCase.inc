<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\HAProxySettings;
use RESTAPI\Models\Package;

class APIModelsHAProxySettingsTestCase extends TestCase {
    /**
     * Installs the HAProxy package before running tests
     */
    public function setup(): void {
        # Install HAProxy
        $pkg = new Package(name: 'pfSense-pkg-haproxy');
        $pkg->create();
    }

    /**
     * Removes the HAProxy package after running tests
     */
    public function teardown(): void {
        $pkg = Package::query(name: 'pfSense-pkg-haproxy')->first();
        $pkg->delete();
    }

    /**
     * Checks that the `enable` field correctly controls the HAProxy service's enablement.
     */
    public function test_enable(): void {
        # Enable HAProxy and ensure the service is enabled/running
        $haproxy_settings = new HAProxySettings(enable: true, async: false);
        $haproxy_settings->update(apply: true);
        $pid = (int) file_get_contents('/var/run/haproxy.pid');
        $this->assert_is_true(posix_kill($pid, signal: 0)); // signal 0 does not actually kill the process

        # Disable HAProxy and ensure the service is no longer enabled/running
        $haproxy_settings = new HAProxySettings(enable: false, async: false);
        $haproxy_settings->update(apply: true);
        $pid = (int) file_get_contents('/var/run/haproxy.pid');
        $this->assert_is_false(posix_kill($pid, signal: 0)); // signal 0 does not actually kill the process
    }

    /**
     * Checks that the `maxconn` field correctly sets the `maxconn` option in haproxy.cfg
     */
    public function test_maxconn(): void {
        $haproxy_settings = new HAProxySettings(enable: true, maxconn: 100, async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'maxconn 100');

        $haproxy_settings = new HAProxySettings(enable: true, maxconn: 120, async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'maxconn 120');
    }

    /**
     * Checks that the `nbthread` field correctly sets the `nbthread` option in haproxy.cfg
     */
    public function test_nbthread(): void {
        $haproxy_settings = new HAProxySettings(enable: true, nbthread: 2, async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'nbthread 2');

        $haproxy_settings = new HAProxySettings(enable: true, nbthread: 3, async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'nbthread 3');
    }

    /**
     * Checks that the `hard_stop_after` field correctly sets the `hard_stop_after` option in haproxy.cfg
     */
    public function test_hard_stop_after(): void {
        $haproxy_settings = new HAProxySettings(enable: true, hard_stop_after: '10m', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'hard-stop-after 10m');

        $haproxy_settings = new HAProxySettings(enable: true, hard_stop_after: '15m', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'hard-stop-after 15m');
    }

    /**
     * Checks that the `localstatsport` field correctly sets the `bind` stats option in haproxy.cfg
     */
    public function test_localstatsport(): void {
        $haproxy_settings = new HAProxySettings(enable: true, localstatsport: '45454', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'bind 127.0.0.1:45454 name localstats');

        $haproxy_settings = new HAProxySettings(enable: true, localstatsport: '45632', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'bind 127.0.0.1:45632 name localstats');
    }

    /**
     * Checks that the `localstats_refreshtime` field correctly sets the `stats refresh` option in haproxy.cfg
     */
    public function test_localstats_refreshtime(): void {
        $haproxy_settings = new HAProxySettings(
            enable: true, 
            localstats_refreshtime: 20, 
            localstatsport: "45454", 
            async: false
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'stats refresh 20');

        $haproxy_settings = new HAProxySettings(
            enable: true,
            localstats_refreshtime: 10,
            localstatsport: "45454",
            async: false
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'stats refresh 10');
    }

    /**
     * Checks that the `remotesyslog` fields correctly sets the `log` option in haproxy.cfg
     */
    public function test_remotesyslog(): void {
        $haproxy_settings = new HAProxySettings(
            enable: true,
            remotesyslog: '127.0.0.50',
            logfacility: 'local1',
            loglevel: 'warning',
            async: false,
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'log 127.0.0.50 local1 warning');

        $haproxy_settings = new HAProxySettings(
            enable: true,
            remotesyslog: '127.0.0.51',
            logfacility: 'syslog',
            loglevel: 'info',
            async: false,
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'log 127.0.0.51 syslog info');
    }

    /**
     * Checks that the `log_send_hostname` field correctly sets the `log-send-hostname` option in haproxy.cfg
     */
    public function test_log_send_hostname(): void {
        $haproxy_settings = new HAProxySettings(enable: true, log_send_hostname: 'example', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'log-send-hostname example');

        $haproxy_settings = new HAProxySettings(enable: true, log_send_hostname: 'updatedexample', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'log-send-hostname updatedexample');
    }

    /**
     * Checks that the resolver fields correctly set the resolver haproxy.cfg options.
     */
    public function test_resolver_fields(): void {
        $haproxy_settings = new HAProxySettings(
            enable: true,
            resolver_retries: 2,
            resolver_timeoutretry: '12s',
            resolver_holdvalid: '10s',
            dns_resolvers: [['name' => 'test', 'server' => '127.0.0.53', 'port' => '5353']],
            async: false,
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'nameserver test 127.0.0.53:5353');
        $this->assert_str_contains($haproxy_cfg->output, 'resolve_retries 2');
        $this->assert_str_contains($haproxy_cfg->output, 'timeout retry 12s');
        $this->assert_str_contains($haproxy_cfg->output, 'timeout resolve 10s');

        $haproxy_settings = new HAProxySettings(
            enable: true,
            resolver_retries: 4,
            resolver_timeoutretry: '15s',
            resolver_holdvalid: '12s',
            dns_resolvers: [['name' => 'updatedtest', 'server' => '127.0.1.53', 'port' => '53']],
            async: false,
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'nameserver updatedtest 127.0.1.53:53');
        $this->assert_str_contains($haproxy_cfg->output, 'resolve_retries 4');
        $this->assert_str_contains($haproxy_cfg->output, 'timeout retry 15s');
        $this->assert_str_contains($haproxy_cfg->output, 'timeout resolve 12s');
    }

    /**
     * Checks that the email_mailers correctly set the email haproxy.cfg options.
     */
    public function test_email_mailers(): void {
        $haproxy_settings = new HAProxySettings(
            enable: true,
            email_mailers: [['name' => 'test', 'mailserver' => '127.0.0.25', 'mailserverport' => '2525']],
            async: false,
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'mailer test 127.0.0.25:2525');

        $haproxy_settings = new HAProxySettings(
            enable: true,
            email_mailers: [['name' => 'updatedtest', 'mailserver' => '127.0.1.25', 'mailserverport' => '25']],
            async: false,
        );
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'mailer updatedtest 127.0.1.25:25');
    }

    /**
     * Checks that the `ssldefaultdhparam` field correctly sets the `tune.ssl.default-dh-param` option in haproxy.cfg
     */
    public function test_ssldefaultdhparam(): void {
        $haproxy_settings = new HAProxySettings(enable: true, ssldefaultdhparam: 4096, async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'tune.ssl.default-dh-param 4096');

        $haproxy_settings = new HAProxySettings(enable: true, ssldefaultdhparam: 2048, async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'tune.ssl.default-dh-param 2048');
    }

    /**
     * Checks that the `sslcompatibilitymode` field correctly sets the `ssl-default-server-options` option in haproxy.cfg
     */
    public function test_sslcompatibilitymode(): void {
        $haproxy_settings = new HAProxySettings(enable: true, sslcompatibilitymode: 'modern', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'ssl-default-server-options ssl-min-ver TLSv1.3');

        $haproxy_settings = new HAProxySettings(enable: true, sslcompatibilitymode: 'intermediate', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'ssl-default-server-options ssl-min-ver TLSv1.2');

        $haproxy_settings = new HAProxySettings(enable: true, sslcompatibilitymode: 'old', async: false);
        $haproxy_settings->update(apply: true);
        $haproxy_cfg = new Command('cat /var/etc/haproxy/haproxy.cfg', trim_whitespace: true);
        $this->assert_str_contains($haproxy_cfg->output, 'ssl-default-server-options ssl-min-ver TLSv1.0');
    }
}
