<?php

namespace API\Models;

use API;
use API\Core\Model;
use API\Dispatchers\DNSResolverApplyDispatcher;
use API\Fields\StringField;
use API\Validators\HostnameValidator;
use API\Validators\IPAddressValidator;

class DNSResolverHostOverride extends Model
{
    public StringField $host;
    public StringField $domain;
    public StringField $ip;
    public StringField $descr;
    
    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null)
    {
        # Set model attributes
        $this->config_path = "unbound/hosts";
        $this->many = true;
        $this->sort_option = SORT_ASC;
        $this->sort_by_field = "host";
        $this->unique_together_fields = ["host", "domain"];
        
        # Set model fields
        $this->host = new StringField(
            required: true,
            allow_empty: true,
            maximum_length: 255,
            validators: [new HostnameValidator()],
            help_text: "The hostname portion of the host override."
        );
        $this->domain = new StringField(
            required: true,
            maximum_length: 255,
            validators: [new HostnameValidator()],
            help_text: "The hostname portion of the host override."
        );
        $this->ip = new StringField(
            required: true,
            many: true,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The IP addresses this host override will resolve."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "A detailed description for this host override."
        );
        parent::__construct(id: $id, data: $data, client: $client);
    }

    /**
     * Applies pending DNS Resolver changes.
     */
    public function apply()
    {
        (new DNSResolverApplyDispatcher())->spawn_process();
    }
}
