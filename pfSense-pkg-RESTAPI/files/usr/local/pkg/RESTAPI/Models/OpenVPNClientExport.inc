<?php

namespace RESTAPI\Models;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;
use RESTAPI\Fields\Base64Field;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\ObjectField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ConflictError;
use RESTAPI\Responses\ValidationError;
use RESTAPI\Responses\ServerError;
use RESTAPI\Responses\NotFoundError;
use RESTAPI\Validators\HostnameValidator;
use RESTAPI\Validators\IPAddressValidator;

/**
 * Defines a Model that represents OpenVPN Client config Export.
 *
 * This Model doesn't (yet?) load default values from config "installedpackages/vpn_openvpn_export/serverconfig/item[]"
 *
 * Many logic/code copied from /usr/local/www/vpn_openvpn_export.php then adopted for RESTAPI.
 * Some cases (like 'Legacy Client') not tested yet.
 *
 */
class OpenVPNClientExport extends Model {
    public ForeignModelField $vpnid;
    public StringField $useaddr;
    public StringField $verifyservercn;
    public BooleanField $blockoutsidedns;
    public BooleanField $legacy;
    public BooleanField $silent;
    public StringField $bindmode;
    public StringField $usepkcs11;
    public StringField $pkcs11providers;
    public StringField $pkcs11id;
    public StringField $usetoken;
    public StringField $usepass;
    public StringField $pass;
    public StringField $p12encryption;
    public StringField $useproxy;
    public StringField $useproxytype;
    public StringField $proxyaddr;
    public PortField $proxyport;
    public StringField $useproxypass;
    public StringField $proxyuser;
    public StringField $proxypass;
    public StringField $advancedoptions;
    public ForeignModelField $uid;
    public ForeignModelField $crtref;

    private $want_cert;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # Set model attributes
        $this->packages = ['pfSense-pkg-openvpn-client-export'];
        $this->package_includes = ['openvpn-client-export.inc'];
        $this->always_apply = true;
        $this->many = false;

        #
        # some internal vars
        #
        $this->want_cert = false;
        $this->nokeys = false;

        #
        # Set model fields
        #
        $this->act = new StringField(required: true, choices: ['confinline', 'confzip'], help_text: 'Export format');

        $this->vpnid = new ForeignModelField(
            model_name: 'OpenVPNServer',
            model_field: 'vpnid',
            required: true,
            help_text: 'The VPN ID of the OpenVPN server this client export corresponds to.',
        );

        $this->uid = new ForeignModelField(
            model_name: 'User',
            model_field: 'uid',
            required: false,
            default: null,
            allow_null: true,
            help_text: 'User ID',
        );
        $this->crtref = new ForeignModelField(
            model_name: 'Certificate',
            model_field: 'refid',
            required: false,
            default: null,
            allow_null: true,
            help_text: 'Certificate refid',
        );

        #
        # Client Connection Behavior
        #
        $this->useaddr = new StringField(required: false, default: 'serveraddr', help_text: 'Host Name Resolution');
        $this->verifyservercn = new StringField(
            default: 'auto',
            choices: ['auto', 'none'],
            help_text: 'Verify Server CN',
        );
        $this->blockoutsidedns = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Block Outside DNS',
        );
        $this->legacy = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Legacy Client',
        );
        $this->silent = new BooleanField(
            default: false,
            indicates_true: 'yes',
            indicates_false: '',
            help_text: 'Silent Installer',
        );
        $this->bindmode = new StringField(
            default: 'nobind',
            choices: ['nobind', 'lport0', 'bind'],
            help_text: 'Bind Mode',
        );

        #
        # Certificate Export Options
        #
        $this->usepkcs11 = new StringField(
            choices: ['yes', 'no'],
            default: 'no',
            help_text: 'Use PKCS#11 storage device (cryptographic token, HSM, smart card) instead of local files.',
        );
        $this->pkcs11providers = new StringField(default: '', conditions: ['usepkcs11' => 'yes']);
        $this->pkcs11id = new StringField(default: '', conditions: ['usepkcs11' => 'yes']);
        $this->usetoken = new StringField(
            choices: ['yes', 'no'],
            default: 'no',
            help_text: 'Use Microsoft Certificate Storage instead of local files.',
        );
        $this->usepass = new StringField(
            default: '',
            choices: ['yes', 'no'],
            help_text: 'Password Protect Certificate',
        );
        $this->pass = new StringField(
            default: '',
            sensitive: true,
            conditions: ['usepass' => 'yes'],
            help_text: 'Certificate Password',
        );
        $this->p12encryption = new StringField(
            default: 'high',
            choices: ['high', 'low', 'legacy'],
            help_text: 'PKCS#12 Encryption',
        );

        #
        # Proxy Options
        #
        $this->useproxy = new StringField(default: 'no', choices: ['yes', 'no'], help_text: 'Use A Proxy');
        $this->useproxytype = new StringField(
            default: 'http',
            choices: ['http', 'socks'],
            conditions: ['useproxy' => 'yes'],
            help_text: 'Proxy Type',
        );
        $this->proxyaddr = new StringField(
            required: true,
            allow_empty: false,
            default: null,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            conditions: ['useproxy' => 'yes'],
            help_text: 'Proxy IP Address',
        );
        $this->proxyport = new PortField(
            required: true,
            unique: true,
            default: null,
            allow_alias: false,
            allow_range: false,
            conditions: ['useproxy' => 'yes'],
            help_text: 'Proxy Port',
        );
        $this->useproxypass = new StringField(
            required: true,
            default: null,
            choices: ['none', 'basic', 'ntlm'],
            conditions: ['useproxy' => true],
            help_text: 'Proxy Authentication',
        );
        $this->proxyuser = new StringField(
            required: true,
            default: null,
            conditions: ['useproxy' => 'yes', 'useproxypass' => ['basic', 'ntlm']],
            help_text: 'Proxy Username',
        );
        $this->proxypass = new StringField(
            required: true,
            conditions: ['useproxy' => 'yes', 'useproxypass' => ['basic', 'ntlm']],
            help_text: 'Proxy Password',
        );

        #
        # Advanced
        #
        $this->advancedoptions = new StringField(
            required: false,
            default: '',
            allow_empty: true,
            help_text: 'Additional options to add to the OpenVPN client export configuration.',
        );

        #
        $this->clientconfig = new Base64Field(default: null, allow_null: true, read_only: true);

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * 	many logic/code copied from /usr/local/www/vpn_openvpn_export.php.
     */
    public function _create(): void {
        global $config, $input_errors;

        $act = $this->act->value;

        $srvid = $this->vpnid->value;

        $useaddr = $this->useaddr->value;

        $verifyservercn = $this->verifyservercn->value;
        $blockoutsidedns = $this->blockoutsidedns->value;
        $legacy = $this->legacy->value;
        $silent = $this->silent->value;
        $bindmode = $this->bindmode->value;

        $usepkcs11 = $this->usepkcs11->value;
        if ($usepkcs11 === 'yes') {
            $usepkcs11 = true;
        } else {
            $usepkcs11 = false;
        }
        $pkcs11providers = $this->pkcs11providers->value;
        $pkcs11id = $this->pkcs11id->value;
        $usetoken = $this->usetoken->value;
        if ($usetoken === 'yes') {
            $usetoken = true;
        } else {
            $usetoken = false;
        }

        $password = $this->pass->value;
        $p12encryption = $this->p12encryption->value;

        $advancedoptions = $this->advancedoptions->value;

        #
        #
        #
        $srvcfg = get_openvpnserver_by_id($srvid);

        $want_cert = $this->want_cert;
        $nokeys = $this->nokeys;

        $cert = $this->cert;
        $crtid = $this->crtid;
        $usrid = $this->usrid;

        if ($srvcfg['mode'] != 'server_user' && !$usepkcs11 && !$usetoken && empty($cert['prv'])) {
            throw new ServerError(
                message: 'A private key cannot be empty if PKCS#11 or Microsoft Certificate Storage is not used.',
                response_id: 'FIELD_INVALID_CHOICE',
            );
        }

        $proxy = '';
        $useproxy = $this->useproxy->value;
        if ($useproxy == 'yes') {
            $proxy = [];
            $proxy['ip'] = $this->proxy_addr->value;
            $proxy['port'] = $this->proxy_port->value;
            $proxy['proxy_type'] = $this->proxy_type->value;
            $proxy['proxy_authtype'] = $this->proxy_authtype->value;
            if ($proxy['proxy_authtype'] != 'none') {
                $proxy['user'] = $this->proxy_user->value;
                $proxy['password'] = $this->proxy_password->value;
            }
        }

        $exp_name = openvpn_client_export_prefix($srvid, $usrid, $crtid);

        if (substr($act, 0, 4) == 'conf') {
            switch ($act) {
                case 'confzip':
                    $exp_name = urlencode($exp_name . '-config.zip');
                    $expformat = 'zip';
                    break;
                case 'conf_yealink_t28':
                    $exp_name = urlencode('client.tar');
                    $expformat = 'yealink_t28';
                    break;
                case 'conf_yealink_t38g':
                    $exp_name = urlencode('client.tar');
                    $expformat = 'yealink_t38g';
                    break;
                case 'conf_yealink_t38g2':
                    $exp_name = urlencode('client.tar');
                    $expformat = 'yealink_t38g2';
                    break;
                case 'conf_snom':
                    $exp_name = urlencode('vpnclient.tar');
                    $expformat = 'snom';
                    break;
                case 'confinline':
                    $exp_name = urlencode($exp_name . '-config.ovpn');
                    $expformat = 'inline';
                    break;
                case 'confinlinedroid':
                    $exp_name = urlencode($exp_name . '-android-config.ovpn');
                    $expformat = 'inlinedroid';
                    break;
                case 'confinlineconnect':
                    $exp_name = urlencode($exp_name . '-connect-config.ovpn');
                    $expformat = 'inlineconnect';
                    break;
                case 'confinlinevisc':
                    $exp_name = urlencode($exp_name . '-viscosity-config.ovpn');
                    $expformat = 'inlinevisc';
                    break;
                default:
                    $exp_name = urlencode($exp_name . '-config.ovpn');
                    $expformat = 'baseconf';
            }
            $exp_path = openvpn_client_export_config(
                $srvid,
                $usrid,
                $crtid,
                $useaddr,
                $verifyservercn,
                $blockoutsidedns,
                $legacy,
                $bindmode,
                $usetoken,
                $nokeys,
                $proxy,
                $expformat,
                $password,
                $p12encryption,
                false,
                false,
                $advancedoptions,
                $usepkcs11,
                $pkcs11providers,
                $pkcs11id,
            );
        }

        if ($act == 'visc') {
            $exp_name = urlencode($exp_name . '-Viscosity.visc.zip');
            $exp_path = viscosity_openvpn_client_config_exporter(
                $srvid,
                $usrid,
                $crtid,
                $useaddr,
                $verifyservercn,
                $blockoutsidedns,
                $legacy,
                $bindmode,
                $usetoken,
                $password,
                $p12encryption,
                $proxy,
                $advancedoptions,
                $usepkcs11,
                $pkcs11providers,
                $pkcs11id,
            );
        }

        if (substr($act, 0, 4) == 'inst') {
            $openvpn_version = substr($act, 5);
            $exp_name = "openvpn-{$exp_name}-install-";
            switch ($openvpn_version) {
                case 'Win7':
                    $legacy = true;
                    $exp_name .= "{$legacy_openvpn_version}-I{$legacy_openvpn_version_rev}-Win7.exe";
                    break;
                case 'Win10':
                    $legacy = true;
                    $exp_name .= "{$legacy_openvpn_version}-I{$legacy_openvpn_version_rev}-Win10.exe";
                    break;
                case 'x86-previous':
                    $exp_name .= "{$previous_openvpn_version}-I{$previous_openvpn_version_rev}-x86.exe";
                    break;
                case 'x64-previous':
                    $exp_name .= "{$previous_openvpn_version}-I{$previous_openvpn_version_rev}-amd64.exe";
                    break;
                case 'x86-current':
                    $exp_name .= "{$current_openvpn_version}-I{$current_openvpn_version_rev}-x86.exe";
                    break;
                case 'x64-current':
                default:
                    $exp_name .= "{$current_openvpn_version}-I{$current_openvpn_version_rev}-amd64.exe";
                    break;
            }

            $exp_name = urlencode($exp_name);
            $exp_path = openvpn_client_export_installer(
                $srvid,
                $usrid,
                $crtid,
                $useaddr,
                $verifyservercn,
                $blockoutsidedns,
                $legacy,
                $bindmode,
                $usetoken,
                $password,
                $p12encryption,
                $proxy,
                $advancedoptions,
                substr($act, 5),
                $usepkcs11,
                $pkcs11providers,
                $pkcs11id,
                $silent,
            );
        }

        /* pfSense >= 2.5.0 with OpenVPN >= 2.5.0 has ciphers not compatible with
         * legacy clients, check for those and warn */
        if ($legacy) {
            global $legacy_incompatible_ciphers;
            $settings = get_openvpnserver_by_id($srvid);
            if (in_array($settings['data_ciphers_fallback'], $legacy_incompatible_ciphers)) {
                $input_errors[] = gettext(
                    'The Fallback Data Encryption Algorithm for the selected server is not compatible with Legacy clients.',
                );
            }
        }

        if (!$exp_path) {
            $input_errors[] = 'Failed to export config files!';
        }

        if (empty($input_errors)) {
            if ($act == 'conf' || substr($act, 0, 10) == 'confinline') {
                $exp_size = strlen($exp_path);
            } else {
                $exp_size = filesize($exp_path);
            }
            header('Pragma: ');
            header('Cache-Control: ');
            header('Content-Type: application/octet-stream');
            header("Content-Disposition: attachment; filename={$exp_name}");
            header("Content-Length: $exp_size");
            if ($act == 'conf' || substr($act, 0, 10) == 'confinline') {
                echo $exp_path;
            } else {
                readfile($exp_path);
                @unlink($exp_path);
            }
            exit();
        } else {
            throw new ServerError(message: 'Some errors occured: ' . $input_errors[0], response_id: 'UNKNONW_ERROR');
        }
    }

    /**
     *
     */
    private function find_usrid_by_uid($uid) {
        global $config, $input_errors;

        foreach ($config['system']['user'] as $idx => $u) {
            if (intval($u['uid']) == intval($uid)) {
                return $idx;
            }
        }

        return -1;
    }

    /**
     *
     */
    private function find_crtid_by_crtref($crtref) {
        global $config, $input_errors;

        foreach ($config['cert'] as $idx => $cert) {
            if ($cert['refid'] == $crtref && $cert['type'] == 'user') {
                return $idx;
            }
        }

        return -1;
    }

    /**
     *
     */
    public function validate_useaddr(string $useaddr): string {
        if (
            !(
                is_ipaddr($useaddr) ||
                is_hostname($useaddr) ||
                in_array($useaddr, ['serveraddr', 'servermagic', 'servermagichost', 'serverhostname'])
            )
        ) {
            throw new ValidationError(
                message: 'An IP address or hostname must be specified.',
                response_id: 'useaddr error',
            );
        }

        return $useaddr;
    }

    /**
     *
     */
    public function validate_extra(): void {
        $srv = $this->vpnid->get_related_model();
        $u = $this->uid->value;
        $crtref = $this->crtref->value;

        #
        #	check if Certificate required for this VPN-instance
        #
        if ($srv->mode->value != 'server_user') {
            $this->want_cert = true;

            if (!isset($crtref) || empty($crtref) || is_null($crtref)) {
                throw new ValidationError(
                    message: 'certref must be specified for this vpnid',
                    response_id: 'CERTREF_REQUIRED_FOR_THIS_SERVER',
                );
            }

            $cert_model = $this->crtref->get_related_model();
            if ($cert_model->type->value != 'user') {
                throw new ServerError(message: 'Bad cert type for this crtref', response_id: 'FIELD_INVALID_CHOICE');
            }

            $this->cert = [
                'crt' => $cert_model->crt->value,
                'prv' => $cert_model->prv->value,
            ];

            $this->crtid = $this->find_crtid_by_crtref($crtref);
        }

        if ($srv->mode->value != 'server_user') {
            $this->nokeys = true;
        }

        #
        #	check if User required for this VPN-instance
        #
        if (isset($srv->authmode->value) && in_array('Local Database', $srv->authmode->value)) {
            if (!isset($u) || empty($u) || is_null($u)) {
                throw new ValidationError(
                    message: 'uid must be specified for this vpnid',
                    response_id: 'UID_REQUIRED_FOR_THIS_SERVER',
                );
            }

            $user_model = $this->uid->get_related_model();

            $this->usrid = $this->find_usrid_by_uid($user_model->uid->value);

            #
            #	check if this Crtref velongs to this User
            #
            $crtref_founded = 0;
            foreach ($user_model->cert->value as $idx => $c) {
                if ($c == $this->crtref->value) {
                    $crtref_founded = 1;
                    $this->crtid = $idx; // openvpn-client-export.inc require crtid as index of user certificates not all certificates
                    break;
                }
            }
            if ($crtref_founded != 1) {
                throw new ValidationError(
                    message: "Can't find this crtref for this uid",
                    response_id: 'CRTREF_UID_MISMATCH',
                );
            }
        }

        $usetoken = $this->usetoken->value;
        $act = $this->act->value;
        if ($usetoken == 'yes' && $act == 'confinline') {
            throw new ValidationError(
                message: 'Microsoft Certificate Storage cannot be used with an Inline configuration.',
                response_id: 'FIELD_INVALID_CHOICE',
            );
        }
        if (
            $usetoken == 'yes' &&
            ($act == 'conf_yealink_t28' ||
                $act == 'conf_yealink_t38g' ||
                $act == 'conf_yealink_t38g2' ||
                $act == 'conf_snom')
        ) {
            throw new ValidationError(
                message: 'Microsoft Certificate Storage cannot be used with a Yealink or SNOM configuration.',
                response_id: 'FIELD_INVALID_CHOICE',
            );
        }
    }
}
