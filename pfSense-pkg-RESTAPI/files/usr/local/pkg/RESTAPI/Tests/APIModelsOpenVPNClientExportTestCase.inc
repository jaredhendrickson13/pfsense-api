<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\AuthServer;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\CertificateGenerate;
use RESTAPI\Models\OpenVPNClientExport;
use RESTAPI\Models\OpenVPNClientExportConfig;
use RESTAPI\Models\OpenVPNServer;
use RESTAPI\Models\User;

class APIModelsOpenVPNClientExportTestCase extends TestCase {
    private CertificateAuthority $server_ca;
    private CertificateGenerate $server_cert;
    private CertificateGenerate $user_cert;
    private User $user;
    private AuthServer $authserver;
    private OpenVPNServer $ovpns;
    private OpenVPNClientExportConfig $ovpnce;

    public array $required_packages = ['pfSense-pkg-openvpn-client-export'];

    /**
     * Setup the test environment before starting tests.
     */
    public function setup(): void {
        # Create a CA we can test with
        $this->server_ca = new CertificateAuthority(
            descr: 'test_ca',
            crt: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt'),
            prv: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key'),
        );
        $this->server_ca->create();

        # Create a new server certificate we can test with
        $this->server_cert = new CertificateGenerate(
            descr: 'test_user_cert',
            caref: $this->server_ca->refid->value,
            keytype: 'RSA',
            keylen: 2048,
            digest_alg: 'sha256',
            dn_commonname: 'ovpns',
            type: 'server',
        );
        $this->server_cert->create();

        # Create a new user certificate we can test with
        $this->user_cert = new CertificateGenerate(
            descr: 'test_user_cert',
            caref: $this->server_ca->refid->value,
            keytype: 'RSA',
            keylen: 2048,
            digest_alg: 'sha256',
            dn_commonname: 'ovpntest',
            type: 'user',
        );
        $this->user_cert->create();

        # Create a user we can use for testing
        $this->user = new User(name: 'ovpntest', password: 'ovpntest', cert: [$this->user_cert->refid->value]);
        $this->user->create();

        # Create a remote auth server to use for testing
        $this->authserver = new AuthServer(
            type: 'radius',
            name: 'TEST_RADIUS',
            host: 'radius.example.com',
            radius_auth_port: '1812',
            radius_acct_port: '1813',
            radius_secret: 'secret',
            radius_protocol: 'MSCHAPv2',
            radius_timeout: 30,
            radius_nasip_attribute: 'wan',
        );
        $this->authserver->create();

        # Create a new OpenVPNServer model object
        $this->ovpns = new OpenVPNServer(
            mode: 'server_tls',
            dev_mode: 'tun',
            protocol: 'UDP4',
            interface: 'wan',
            use_tls: true,
            tls: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_openvpn_tls.key'),
            tls_type: 'auth',
            dh_length: '2048',
            ecdh_curve: 'none',
            data_ciphers: ['AES-256-GCM'],
            data_ciphers_fallback: 'AES-256-GCM',
            digest: 'SHA256',
            caref: $this->server_ca->refid->value,
            certref: $this->server_cert->refid->value,
            async: true,
        );
        $this->ovpns->create(apply: true);
        $this->ovpns->reload_config();

        # Create a client export config for the server
        $this->ovpnce = new OpenVPNClientExportConfig(server: $this->ovpns->vpnid->value);
        $this->ovpnce->create();
    }

    /**
     * Remove the CA and cert used for testing after tests complete.
     */
    public function teardown(): void {
        $this->user->delete();
        $this->authserver->delete();
        $this->ovpns->delete(apply: true);
        $this->user_cert->delete();
        $this->server_cert->delete();
        $this->server_ca->delete();
    }

    /**
     * Ensure the 'certref' field is required for OpenVPNServers using the 'server_tls' mode
     */
    public function test_certref_required_for_server_tls(): void {
        # Ensure the server_tls server mode requires a certref for export
        $this->assert_throws_response(
            response_id: 'OPENVPN_CLIENT_EXPORT_CERTREF_REQUIRED',
            code: 400,
            callable: function () {
                # Update the OpenVPNServer mode
                $this->ovpns->mode->value = 'server_tls';
                $this->ovpns->update();

                # Ensure the validate_server method throws an error for no certref
                $export = new OpenVPNClientExport(id: $this->ovpnce->id);
                $export->certref->value = null;
                $export->validate_server($this->ovpns->vpnid->value);
            },
        );
    }

    /**
     * Ensure the 'certref' field is required for OpenVPNServers using the 'server_tls_user' mode
     */
    public function test_certref_required_for_server_tls_user(): void {
        # Ensure the server_tls_user server mode requires a certref for export
        $this->assert_throws_response(
            response_id: 'OPENVPN_CLIENT_EXPORT_CERTREF_REQUIRED',
            code: 400,
            callable: function () {
                # Update the OpenVPNServer mode
                $this->ovpns->mode->value = 'server_tls_user';
                $this->ovpns->authmode->value = $this->ovpns->authmode->default;
                $this->ovpns->update();

                # Ensure the validate_server method throws an error for no certref
                $export = new OpenVPNClientExport(id: $this->ovpnce->id);
                $export->certref->value = null;
                $export->validate_server($this->ovpns->vpnid->value);
            },
        );
    }

    /**
     * Ensure the 'certref' field is not required for OpenVPNServers using the 'server_user' mode
     */
    public function test_certref_not_required_for_server_user(): void {
        # Ensure the server_user server mode does not require a certref for export
        $this->assert_does_not_throw(
            callable: function () {
                # Update the OpenVPNServer mode
                $this->ovpns->mode->value = 'server_user';
                $this->ovpns->authmode->value = $this->ovpns->authmode->default;
                $this->ovpns->update();

                # Ensure the validate_server method does not throw an error for no certref
                $export = new OpenVPNClientExport(id: $this->ovpnce->id);
                $export->certref->value = null;
                $export->username->value = $this->user->name->value; # username is required for server_user mode
                $export->validate_server($this->ovpns->vpnid->value);
            },
        );
    }

    /**
     * Ensure the username field is required for OpenVPN servers using the 'Local Database' authmode
     */
    public function test_username_required_for_local_database_authmode(): void {
        # Ensure a username is required when using the local database authmode
        $this->assert_throws_response(
            response_id: 'OPENVPN_CLIENT_EXPORT_USERNAME_REQUIRED',
            code: 400,
            callable: function () {
                # Update the OpenVPNServer mode
                $this->ovpns->mode->value = 'server_tls_user';
                $this->ovpns->authmode->value = $this->ovpns->authmode->default;
                $this->ovpns->update();

                # Ensure the validate_server method throws an error for no certref
                $export = new OpenVPNClientExport(id: $this->ovpnce->id, certref: $this->user_cert->refid->value);
                $export->username->value = null;
                $export->validate_server($this->ovpns->vpnid->value);
            },
        );
    }

    /**
     * Ensure the username field is not required for OpenVPN servers using a remote authmode
     */
    public function test_username_not_required_for_remote_authmode(): void {
        # Ensure a username is not required when using a remote authmode
        $this->assert_does_not_throw(
            callable: function () {
                # Update the OpenVPNServer mode
                $this->ovpns->mode->value = 'server_tls_user';
                $this->ovpns->authmode->value = [$this->authserver->name->value];
                $this->ovpns->update();

                # Ensure the validate_server method does not throw an error for no username
                $export = new OpenVPNClientExport(id: $this->ovpnce->id, certref: $this->user_cert->refid->value);
                $export->username->value = null;
                $export->validate_server($this->ovpns->vpnid->value);
            },
        );
    }

    /**
     * Ensure an error is thrown when 'usetoken' is enabled with a forbidden export 'type'
     */
    public function test_usetoken_forbidden_types(): void {
        foreach (OpenVPNClientExport::USETOKEN_FORBIDDEN_TYPES as $forbidden_type) {
            $this->assert_throws_response(
                response_id: 'OPENVPN_CLIENT_EXPORT_TYPE_USETOKEN_NOT_ALLOWED',
                code: 409,
                callable: function () use ($forbidden_type) {
                    # Create a new OpenVPNClientExport with 'usetoken' enabled and a forbidden export 'type'
                    $export = new OpenVPNClientExport(usetoken: true);
                    $export->validate_type($forbidden_type);
                },
            );
        }
    }

    /**
     * Ensure no error is thrown when 'usetoken' is disabled, and a usetoken forbidden type is used.
     */
    public function test_usetoken_forbidden_types_disabled(): void {
        foreach (OpenVPNClientExport::USETOKEN_FORBIDDEN_TYPES as $forbidden_type) {
            $this->assert_does_not_throw(
                callable: function () use ($forbidden_type) {
                    # Create a new OpenVPNClientExport with 'usetoken' disabled and a forbidden export 'type'
                    $export = new OpenVPNClientExport(usetoken: false);
                    $export->validate_type($forbidden_type);
                },
            );
        }
    }

    /**
     * Ensure no error is thrown when 'usetoken' is enabled, and a non usetoken forbidden type is used.
     */
    public function test_usetoken_non_forbidden_types(): void {
        $this->assert_does_not_throw(
            callable: function () {
                # Create a new OpenVPNClientExport with 'usetoken' enabled and a non forbidden export 'type'
                $export = new OpenVPNClientExport(usetoken: true);
                $export->validate_type('inst-Win10');
            },
        );
    }

    /**
     * Ensure the certref validation for user certs is skipped if there is no username specified.
     */
    public function test_certref_validation_skipped_for_user_certs_without_username(): void {
        # Create a new OpenVPNClientExport with a user cert and no username
        $export = new OpenVPNClientExport();
        $export->username->value = null;
        $this->assert_equals($export->validate_certref('anything'), 'anything');
        $this->assert_equals($export->validate_certref($this->user_cert->refid->value), $this->user_cert->refid->value);
    }

    /**
     * Ensure an error is thrown if the provided certref is not assigned to the provided user.
     */
    public function test_certref_validation_user_cert_not_assigned_to_user(): void {
        # Ensure an error is thrown if the certref is not assigned to the user
        $this->assert_throws_response(
            response_id: 'OPENVPN_CLIENT_EXPORT_USER_CERT_NOT_FOUND',
            code: 404,
            callable: function () {
                # Create a new OpenVPNClientExport with a user cert and a username
                $export = new OpenVPNClientExport(username: $this->user->name->value);

                # Try to use the server cert (which isn't assigned to the user), this should throw an error
                $export->validate_certref($this->server_cert->refid->value);
            },
        );
    }

    /**
     * Ensure an error is thrown if the requested user cert does not have a private key
     * and usepkcs11 or usetoken is not used
     */
    public function test_certref_validation_user_cert_no_private_key(): void {
        # Temporarily unset the prv value for the user cert
        Certificate::set_config("{$this->user_cert->get_config_path()}/prv", null);

        # Ensure an error is thrown if the user cert does not have a private key
        $this->assert_throws_response(
            response_id: 'OPENVPN_CLIENT_EXPORT_CERT_NO_PRIVATE_KEY',
            code: 400,
            callable: function () {
                # Create a new OpenVPNClientExport with a user cert and a username
                $export = new OpenVPNClientExport(
                    username: $this->user->name->value,
                    usepkcs11: false,
                    usetoken: false,
                );

                # Ensure the validate_certref method throws an error for no private key
                $export->validate_certref($this->user_cert->refid->value);
            },
        );
    }

    /**
     * Checks that the export config correctly generates an inline client export
     */
    public function test_export_config_inline(): void {
        $exporter = new OpenVPNClientExport(
            id: $this->ovpnce->id,
            type: 'confinline',
            username: $this->user->name->value,
            certref: $this->user_cert->refid->value,
        );

        $config = $exporter->export_config();

        # Ensure the user ca, cert and key are present in the config
        $this->assert_str_contains($config, $this->user_cert->crt->value);
        $this->assert_str_contains($config, $this->user_cert->prv->value);
        $this->assert_str_contains($config, $this->server_ca->crt->value);
    }

    /**
     * Checks that the export config correctly generates a zip client export
     */
    public function test_export_config_zip(): void {
        $exporter = new OpenVPNClientExport(
            id: $this->ovpnce->id,
            type: 'confzip',
            username: $this->user->name->value,
            certref: $this->user_cert->refid->value,
        );
        $filepath = $exporter->export_config();

        # Ensure the generated file exists and is a zip
        $this->assert_is_true(file_exists($filepath));
        $this->assert_str_contains($filepath, '.zip');
        @unlink($filepath); // Clean up the generated file
    }

    /**
     * Checks that the export installer correctly generates a exe client export
     */
    public function test_export_installer(): void {
        $exporter = new OpenVPNClientExport(
            id: $this->ovpnce->id,
            type: 'inst-Win10',
            username: $this->user->name->value,
            certref: $this->user_cert->refid->value,
        );
        $filepath = $exporter->export_installer();

        # Ensure the generated file exists and is a exe
        $this->assert_is_true(file_exists($filepath));
        $this->assert_str_contains($filepath, '.exe');
        @unlink($filepath); // Clean up the generated file
    }

    /**
     * Checks that the export viscosity correctly generates a viscosity client export
     */
    public function test_export_viscosity(): void {
        $exporter = new OpenVPNClientExport(
            id: $this->ovpnce->id,
            type: 'visc',
            username: $this->user->name->value,
            certref: $this->user_cert->refid->value,
        );
        $filepath = $exporter->export_installer();

        # Ensure the generated file exists and is a exe
        $this->assert_is_true(file_exists($filepath));
        $this->assert_str_contains($filepath, '.exe');
        @unlink($filepath); // Clean up the generated file
    }

    /**
     * Checks that the create methods behaves as intended
     */
    public function test_create(): void {
        # Create a new OpenVPNClientExport with the required fields
        $export = new OpenVPNClientExport(
            id: $this->ovpnce->id,
            type: 'confinline',
            blockoutsidedns: true,
            useaddr: 'other',
            useaddr_hostname: 'ovpn.example.com',
            username: $this->user->name->value,
            certref: $this->user_cert->refid->value,
        );

        # Ensure the export can be created without errors
        $export->create();

        # Ensure the binary_data field is populated with the correct config
        $this->assert_is_not_empty($export->binary_data->value);
        $this->assert_str_contains($export->binary_data->value, $this->user_cert->crt->value);
        $this->assert_str_contains($export->binary_data->value, $this->user_cert->prv->value);
        $this->assert_str_contains($export->binary_data->value, $this->server_ca->crt->value);
        $this->assert_str_contains($export->binary_data->value, 'remote ovpn.example.com');
        $this->assert_str_contains($export->binary_data->value, 'block-outside-dns');

        # Change the export config and ensure the config is regenerated
        $export->useaddr->value = 'other';
        $export->useaddr_hostname->value = 'new.ovpn.example.com';
        $export->blockoutsidedns->value = false;
        $export->create();

        # Ensure the binary_data field is updated with the new config
        $this->assert_str_contains($export->binary_data->value, 'remote new.ovpn.example.com');
        $this->assert_str_does_not_contain($export->binary_data->value, 'block-outside-dns');

        # Export a file based type
        $export->type->value = 'confzip';
        $export->create();

        # Ensure a filename was set, the binary_data is populated (as actual binary data), and the actual file has
        # already been removed
        $this->assert_str_contains($export->filename->value, '.zip');
        $this->assert_is_not_empty($export->binary_data->value);
        $this->assert_is_true(!ctype_print($export->binary_data->value));
        $this->assert_is_false(file_exists("/tmp/{$export->filename->value}"));
    }

    /**
     * Ensures we can exporta config for a server using the 'server_tls_user' mode, a user cert, and local database
     * auth. This is a regression test for issue #756. This is a necessary test distinction as pfSense will change the
     * meaning of the crtid passed into certain pfSense functions based on the server mode and authmode.
     */
    public function test_create_with_server_tls_user_and_local_database(): void {
        # First, update the OpenVPNServer to use the 'server_tls_user' mode and local database auth
        $this->ovpns->mode->value = 'server_tls_user';
        $this->ovpns->authmode->value = ['Local Database'];
        $this->ovpns->update();

        # Setup the export
        $export = new OpenVPNClientExport(
            id: $this->ovpnce->id,
            type: 'confinline',
            username: $this->user->name->value,
            certref: $this->user_cert->refid->value,
        );

        # Ensure the certref's object ID does NOT match the determined crtid. These should not match because
        # pfSense now refers to the crtid as the index of the cert in the system/user config, NOT the cert config
        # like usual.
        $this->assert_not_equals($this->user_cert->refid->get_related_model()->id, $export->locate_crtid());

        # Ensure we can complete the export as intended and that the embedded cert is correct
        $export->create();
        $this->assert_is_not_empty($export->binary_data->value);
        $this->assert_str_contains($export->binary_data->value, $this->user_cert->crt->value);

        # Reset the server mode and authmode for other tests
        $this->ovpns->mode->value = 'server_tls';
        $this->ovpns->update();
    }
}
