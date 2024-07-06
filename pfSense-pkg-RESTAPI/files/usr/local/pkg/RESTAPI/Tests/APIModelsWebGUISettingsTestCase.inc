<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Dispatchers\WebGUIRestartDispatcher;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\WebGUISettings;

class APIModelsWebGUISettingsTestCase extends TestCase {
    /**
     * Ensure the Web GUI is restarted after we're done testing so it can restore the original config
     */
    public function teardown(): void {
        (new WebGUIRestartDispatcher(async: false))->spawn_process();
    }

    /**
     * Checks that we can successfully update the web GUI's port.
     */
    public function test_can_update_webgui_port(): void {
        # Update the webgui port
        $webgui = new WebGUISettings(port: '8443', async: false);
        $webgui->update();

        # Ensure nginx is now listening on the new port
        $nginx_conf = file_get_contents('/var/etc/nginx-webConfigurator.conf');
        $this->assert_str_contains($nginx_conf, 'listen 8443');

        # Update the webgui port
        $webgui = new WebGUISettings(port: '443', async: false);
        $webgui->update();

        # Ensure nginx is now listening on the new port
        $nginx_conf = file_get_contents('/var/etc/nginx-webConfigurator.conf');
        $this->assert_str_contains($nginx_conf, 'listen 443');
    }

    /**
     * Checks that we can successfully update the web GUI's protocol
     */
    public function test_can_update_webgui_protocol(): void {
        # Update the webgui protocol
        $webgui = new WebGUISettings(protocol: 'http', port: '8080', async: false);
        $webgui->update();

        # Ensure nginx is now listening for the new protocol
        $nginx_conf = file_get_contents('/var/etc/nginx-webConfigurator.conf');
        $this->assert_str_contains($nginx_conf, 'listen 8080;');

        # Update the webgui protocol
        $webgui = new WebGUISettings(protocol: 'https', port: '443', async: false);
        $webgui->update();

        # Ensure nginx is now listening for the new protocol
        $nginx_conf = file_get_contents('/var/etc/nginx-webConfigurator.conf');
        $this->assert_str_contains($nginx_conf, 'listen 443 ssl');
    }

    /**
     * Checks that we can successfully update the web GUI's SSL/TLS certificate.
     */
    public function test_can_update_webgui_sslcertref(): void {
        # Create a new certificate to use
        $cert = new Certificate(
            descr: 'WebGUISettingsTestCaseCert',
            crt: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt'),
            prv: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key'),
        );
        $cert->create();

        # Update the web GUI's SSL/TLS certificate
        $webgui = new WebGUISettings(sslcertref: $cert->refid->value, async: false);
        $webgui->update();

        # Ensure the SSL/TLS certificate referenced by nginx matches our cert
        $nginx_cert = file_get_contents('/var/etc/cert.crt');
        $nginx_key = file_get_contents('/var/etc/cert.key');
        $this->assert_equals($nginx_cert, $cert->crt->value);
        $this->assert_equals($nginx_key, $cert->prv->value);
    }
}
