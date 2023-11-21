<?php

namespace api\models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Dispatchers\DNSResolverApplyDispatcher;
use API\Fields\StringField;
use API\Validators\HostnameValidator;
use API\Validators\IPAddressValidator;

class DNSResolverHostOverrideAlias extends Model
{
    public StringField $host;
    public StringField $domain;
    public StringField $descr;
    
    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], Auth $client = null)
    {
        # Set model attributes
        $this->parent_model_class = "DNSResolverHostOverride";
        $this->config_path = "aliases/item";
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
            help_text: "The hostname portion of the host override alias."
        );
        $this->domain = new StringField(
            required: true,
            maximum_length: 255,
            validators: [new HostnameValidator()],
            help_text: "The hostname portion of the host override alias."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            internal_name: "description",
            help_text: "A detailed description for this host override alias."
        );
        parent::__construct(id: $id, parent_id: $parent_id, data: $data, client: $client);
    }

    /**
     * Applies pending DNS Resolver changes.
     */
    public function apply()
    {
        (new DNSResolverApplyDispatcher())->spawn_process();
    }
}
