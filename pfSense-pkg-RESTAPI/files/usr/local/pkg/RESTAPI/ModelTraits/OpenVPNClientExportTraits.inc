<?php

namespace RESTAPI\ModelTraits;

use RESTAPI\Core\Model;
use RESTAPI\Fields\Base64Field;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ConflictError;
use RESTAPI\Validators\HostnameValidator;
use RESTAPI\Validators\IPAddressValidator;

trait OpenVPNClientExportTraits {
    public ForeignModelField $server;
    public StringField $useaddr;
    public StringField $useaddr_hostname;
    public StringField $verifyservercn;
    public BooleanField $blockoutsidedns;
    public BooleanField $legacy;
    public BooleanField $silent;
    public StringField $bindmode;
    public BooleanField $usepkcs11;
    public StringField $pkcs11providers;
    public StringField $pkcs11id;
    public BooleanField $usetoken;
    public BooleanField $usepass;
    public StringField $pass;
    public StringField $p12encryption;
    public BooleanField $useproxy;
    public StringField $useproxytype;
    public StringField $proxyaddr;
    public PortField $proxyport;
    public StringField $useproxypass;
    public StringField $proxyuser;
    public StringField $proxypass;
    public Base64Field $advancedoptions;

    /**
     * Sets up all Field properties for classes using this trait. This is primarily used to allow the
     * OpenVPNClientExportConfig and OpenVPNClientExport Model classes to share the same fields since
     * they are identical in structure, but serve two distinct purposes.
     */
    private function __setup_fields(): void {
        $this->server = new ForeignModelField(
            model_name: 'OpenVPNServer',
            model_field: 'vpnid',
            model_query: ['disable' => false, 'mode__startswith' => 'server'],
            required: true,
            help_text: 'The VPN ID of the OpenVPN server this client export corresponds to.',
        );
        $this->useaddr = new StringField(
            required: false,
            default: 'serveraddr',
            choices: ['serveraddr', 'servermagic', 'servermagichost', 'serverhostname', 'other'],
            help_text: 'The method to use for the OpenVPN server address listed in the config export.',
        );
        $this->useaddr_hostname = new StringField(
            required: false,
            default: '',
            allow_empty: true,
            conditions: ['useaddr' => 'other'],
            validators: [new HostnameValidator(allow_hostname: true, allow_domain: true, allow_fqdn: true)],
            help_text: 'The hostname to use for the OpenVPN server address.',
        );
        $this->verifyservercn = new StringField(
            default: 'auto',
            choices: ['auto', 'none'],
            help_text: 'Verify the server certificate Common Name (CN) when the client connects.',
        );
        $this->blockoutsidedns = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Block access to DNS servers except across OpenVPN while connected, forcing clients to ' .
            'use only VPN DNS servers.',
        );
        $this->legacy = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Do not include OpenVPN 2.5 and later settings in the client configuration.',
        );
        $this->silent = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Create Windows installer for unattended deploy.',
        );
        $this->bindmode = new StringField(
            default: 'nobind',
            choices: ['nobind', 'lport0', 'bind'],
            help_text: 'The port binding mode to use. If OpenVPN client binds to the default OpenVPN port (1194), ' .
            'two clients may not run concurrently.',
        );
        $this->usepkcs11 = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Use PKCS#11 storage device (cryptographic token, HSM, smart card) instead of local files.',
        );
        $this->pkcs11providers = new StringField(
            required: true,
            many: true,
            delimiter: ' ',
            conditions: ['usepkcs11' => true],
            help_text: 'The client local path to the PKCS#11 provider(s) (DLL, module)',
        );
        $this->pkcs11id = new StringField(
            required: true,
            conditions: ['usepkcs11' => true],
            help_text: 'The object\'s ID on the PKCS#11 device.',
        );
        $this->usetoken = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Use Microsoft Certificate Storage instead of local files.',
        );
        $this->usepass = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Use a password to protect the PKCS#12 file contents or key in Viscosity bundles.',
        );
        $this->pass = new StringField(
            required: true,
            sensitive: true,
            conditions: ['usepass' => true],
            help_text: 'Password used to protect the certificate file contents.',
        );
        $this->p12encryption = new StringField(
            default: 'high',
            choices: ['high', 'low', 'legacy'],
            help_text: 'The level of encryption to use when exporting a PKCS#12 archive. Encryption support varies ' .
            'by Operating System and program',
        );
        $this->useproxy = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Use proxy to communicate with the OpenVPN server.',
        );
        $this->useproxytype = new StringField(
            default: 'http',
            choices: ['http', 'socks'],
            conditions: ['useproxy' => true],
            help_text: 'The proxy type to use.',
        );
        $this->proxyaddr = new StringField(
            required: true,
            conditions: ['useproxy' => true],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true, allow_fqdn: true)],
            help_text: 'The IP address or hostname of the proxy server to use.',
        );
        $this->proxyport = new PortField(
            required: true,
            allow_alias: false,
            allow_range: false,
            conditions: ['useproxy' => true],
            help_text: 'The port where the proxy server is listening.',
        );
        $this->useproxypass = new StringField(
            required: true,
            choices: ['none', 'basic', 'ntlm'],
            conditions: ['useproxy' => true],
            help_text: 'The type of authentication to use for the proxy server.',
        );
        $this->proxyuser = new StringField(
            required: true,
            conditions: ['useproxy' => true, 'useproxypass' => ['basic', 'ntlm']],
            help_text: 'The username to use to authenticate with the proxy server.',
        );
        $this->proxypass = new StringField(
            required: true,
            sensitive: true,
            conditions: ['useproxy' => true, 'useproxypass' => ['basic', 'ntlm']],
            help_text: 'The password to use to authenticate with the proxy server.',
        );
        $this->advancedoptions = new Base64Field(
            required: false,
            default: '',
            allow_empty: true,
            help_text: 'Additional options to add to the OpenVPN client export configuration.',
        );
    }

    /**
     * Add extra validation to the 'legacy' field. This is used to ensure that legacy ciphers are even
     * supported by the OpenVPN server.
     */
    public function validate_legacy(bool $legacy): bool {
        if ($legacy) {
            global $legacy_incompatible_ciphers;
            $settings = get_openvpnserver_by_id($this->server->value);
            if (in_array($settings['data_ciphers_fallback'], $legacy_incompatible_ciphers)) {
                throw new ConflictError(
                    message: 'The Fallback Data Encryption Algorithm for the selected server is not compatible with ' .
                    'Legacy clients.',
                    response_id: 'OPENVPN_CLIENT_EXPORT_CONFIG_LEGACY_CIPHER_NOT_COMPATIBLE',
                );
            }
        }

        return $legacy;
    }
}
