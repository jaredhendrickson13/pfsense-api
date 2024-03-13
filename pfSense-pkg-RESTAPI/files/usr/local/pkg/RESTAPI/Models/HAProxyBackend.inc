<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\HostnameValidator;
use RESTAPI\Validators\RegexValidator;

class HAProxyBackend extends Model
{
    public StringField $name;
    public NestedModelField $servers;
    public StringField $balance;
    public IntegerField $balance_urilen;
    public IntegerField $balance_uridepth;
    public BooleanField $balance_uriwhole;
    public NestedModelField $acls;
    public IntegerField $connection_timeout;
    public IntegerField $server_timeout;
    public IntegerField $retries;
    public StringField $check_type;
    public IntegerField $checkinter;
    public BooleanField $log_health_checks;
    public StringField $httpcheck_method;
    public StringField $monitor_uri;
    public StringField $monitor_httpversion;
    public StringField $monitor_username;
    public StringField $monitor_domain;

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
        $this->balance = new StringField(
            default: "",
            choices: ["", "roundrobin", "static-rr", "leastconn", "source", "uri"],
            help_text: "The load balancing option to use for servers assigned to this backend."
        );
        $this->balance_urilen = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ["balance" => "uri"],
            help_text: "The number of URI characters the algorithm should consider when hashing."
        );
        $this->balance_uridepth = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ["balance" => "uri"],
            help_text: "The maximum directory depth to be used to compute the hash. One level is counted for each ".
                "slash in the request."
        );
        $this->balance_uriwhole = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["balance" => "uri"],
            help_text: "Enables or disables allowing the use of whole URIs, including URL parameters."
        );
        $this->acls = new NestedModelField(
            model_class: "HAProxyBackendACL",
            default: [],
            allow_empty: true,
            help_text: "The ACLs to apply to this backend."
        );
        $this->connection_timeout = new IntegerField(
            default: 30000,
            allow_null: true,
            help_text: "The amount of time (in milliseconds) to wait before giving up on connections."
        );
        $this->server_timeout = new IntegerField(
            default: 30000,
            allow_null: true,
            help_text: "The amount of time (in milliseconds) to wait for data transferred to or from the server."
        );
        $this->retries = new IntegerField(
            default: null,
            allow_null: true,
            help_text: "The number of retry attempts to allow after a connection failure to the server."
        );
        $this->check_type = new StringField(
            default: "none",
            choices: ['none', 'Basic', 'HTTP', 'LDAP', 'MySQL', 'PostgreSQL', 'Redis', 'SMTP', 'ESMTP', 'SSL'],
            help_text: "The health check method to use when checking the health of backend servers."
        );
        $this->checkinter = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ["!check_type" => "none"],
            help_text: "The interval (in milliseconds) in which health checks will be performed."
        );
        $this->log_health_checks = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["!check_type" => "none"],
            help_text: "Enables or disables logging changes to the health check status"
        );
        $this->httpcheck_method = new StringField(
            default: "OPTIONS",
            choices: ["OPTIONS", "HEAD", "GET", "POST", "PUT", "DELETE", "TRACE"],
            conditions: ["check_type" => "HTTP"],
            help_text: "The HTTP method to use for HTTP health checks."
        );
        $this->monitor_uri = new StringField(
            default: "/",
            allow_empty: true,
            conditions: ["check_type" => "HTTP"],
            help_text: "The URL to use for HTTP health checks."
        );
        $this->monitor_httpversion = new StringField(
            default: "HTTP/1.0",
            allow_empty: true,
            conditions: ["check_type" => "HTTP"],
            help_text: "The HTTP version to use for HTTP health checks."
        );
        $this->monitor_username = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["check_type" => ["MySQL", "PostgreSQL"]],
            help_text: "The username to use for MySQL or PostgreSQL health checks."
        );
        $this->monitor_domain = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["check_type" => ["SMTP", "ESMTP"]],
            validators: [new HostnameValidator(allow_hostname: true, allow_domain: true, allow_fqdn: true)],
            help_text: "The domain to use for SMTP or ESMTP health checks."
        );
        
        parent::__construct($id, $parent_id, $data, ...$options);
    }
}
