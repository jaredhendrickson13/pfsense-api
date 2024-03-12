<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\RegexValidator;

class HAProxyBackend extends Model
{
    public StringField $name;
    public NestedModelField $servers;
    public StringField $balance;
    
    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->config_path = "installedpackages/haproxy/ha_pools/item";
        $this->packages = ['pfSense-pkg-haproxy'];
        $this->package_includes = ['haproxy/haproxy.inc', 'haproxy/haproxy_utils.inc'];
        $this->many = true;

        # Set model fields
        $this->name = new StringField(
            required: true,
            unique: true,
            validators: [new RegexValidator(pattern: "/[^a-zA-Z0-9\.\-_]/", invert: true)],
            help_text: "The unique name for this backend."
        );
        $this->servers = new NestedModelField(
            model_class: "HAProxyBackendServer",
            default: [],
            allow_empty: true,
            help_text: "The pool of servers this backend will use."
        );
        
        
        parent::__construct($id, $parent_id, $data, ...$options);
    }
}
