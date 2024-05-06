<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\DNSResolverApplyDispatcher;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\HostnameValidator;
use RESTAPI\Validators\IPAddressValidator;

class DNSResolverDomainOverride extends Model
{
    public StringField $domain;
    public StringField $ip;
    public StringField $descr;
    public BooleanField $forward_tls_upstream;
    public StringField $tls_hostname;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->config_path = 'unbound/domainoverrides';
        $this->subsystem = 'unbound';
        $this->many = true;

        # Set model fields
        $this->domain = new StringField(
            required: true,
            maximum_length: 255,
            validators: [new HostnameValidator(allow_hostname: true, allow_domain: true, allow_fqdn: true)],
            help_text: 'The domain to override.',
        );
        $this->ip = new StringField(
            required: true,
            maximum_length: 255,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: 'The IP address to which the domain should resolve.',
        );
        $this->forward_tls_upstream = new BooleanField(
            default: false,
            help_text: 'Enables or disables forwarding DNS queries to the upstream DNS server using TLS.',
        );
        $this->tls_hostname = new StringField(
            default: '',
            allow_empty: true,
            maximum_length: 255,
            validators: [new HostnameValidator(allow_hostname: true, allow_domain: true, allow_fqdn: true)],
            help_text: 'The hostname to use for the TLS connection to the upstream DNS server.',
        );
        $this->descr = new StringField(
            default: '',
            allow_empty: true,
            help_text: 'The description for this domain override.',
        );
        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Applies pending DNS Resolver changes.
     */
    public function apply(): void {
        (new DNSResolverApplyDispatcher(async: $this->async))->spawn_process();
    }
}