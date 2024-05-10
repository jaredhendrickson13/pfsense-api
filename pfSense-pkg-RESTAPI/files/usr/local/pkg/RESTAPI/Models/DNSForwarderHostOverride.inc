<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\IPAddressValidator;

class DNSForwarderHostOverride extends Model
{
    public StringField $host;
    public StringField $domain;
    public StringField $ip;
    public StringField $descr;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->config_path = 'dnsmasq/hosts';
        $this->many = true;
        $this->unique_together_fields = ['host', 'domain'];

        # Set model fields
        $this->host = new StringField(
            required: true,
            help_text: 'The hostname of this override.',
        );
        $this->domain = new StringField(
            required: true,
            help_text: 'The domain of this override.',
        );
        $this->ip = new StringField(
            required: true,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: 'The IP address of this override.',
        );
        $this->descr = new StringField(
            default: '',
            allow_empty: true,
            help_text: 'The description of this override.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }
}