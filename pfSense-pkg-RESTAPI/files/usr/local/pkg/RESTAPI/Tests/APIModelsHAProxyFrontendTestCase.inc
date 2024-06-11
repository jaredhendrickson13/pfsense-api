<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\HAProxyBackend;
use RESTAPI\Models\HAProxyBackendAction;
use RESTAPI\Models\HAProxyFrontend;
use RESTAPI\Models\HAProxyFrontendACL;
use RESTAPI\Models\HAProxyFrontendAction;
use RESTAPI\Models\HAProxyFrontendAddress;

class APIModelsHAProxyFrontendTestCase extends TestCase {
    private HAProxyBackend $backend;
    public array $required_packages = ['pfSense-pkg-haproxy'];

    /**
     * Setup the test case.
     */
    public function setup(): void {
        $this->backend = new HAProxyBackend(
            name: 'example_backend',
            balance: '',
            acls: [
                [
                    'name' => 'example_acl',
                    'expression' => 'host_starts_with',
                    'value' => 'example',
                    'casesensitive' => false,
                    'not' => false,
                ],
            ],
            actions: [
                [
                    'action' => 'http-response_lua',
                    'acl' => 'example_acl',
                    'lua_function' => 'example function',
                ],
            ],
            check_type: 'HTTP',
            log_health_checks: false,
            httpcheck_method: 'OPTIONS',
            monitor_uri: '',
            monitor_httpversion: '',
            monitor_username: '',
            monitor_domain: '',
            agent_checks: false,
            agent_port: '',
            persist_cookie_enabled: false,
            persist_cookie_name: '',
            persist_cookie_mode: 'passive',
            haproxy_cookie_domains: [],
            haproxy_cookie_dynamic_cookie_key: '',
            persist_sticky_type: 'none',
            persist_stick_expire: '',
            persist_stick_tablesize: '',
            persist_stick_cookiename: '',
            email_level: '',
            email_to: '',
            stats_enabled: true,
            stats_uri: '/',
            stats_scope: ['.'],
            stats_realm: '',
            stats_username: '',
            stats_admin: '',
            stats_node: 'NODE1',
            stats_desc: '',
            stats_refresh: 10,
            cookie_attribute_secure: false,
            advanced: '',
            advanced_backend: '',
            transparent_clientip: false,
            transparent_interface: 'lan',
        );
        $this->backend->create();
    }

    /**
     * Tear down the test case.
     */
    public function teardown(): void {
        $this->backend->delete();
    }

    /**
     * Check that we can create, update and deleted HAProxy Frontend objects.
     */
    public function test_crud(): void {
        $frontend = new HAProxyFrontend(
            name: 'example_multipledomains',
            status: 'active',
            a_extaddr: [
                [
                    'extaddr' => 'any_ipv4',
                    'extaddr_port' => '80',
                    'extaddr_ssl' => false,
                ],
            ],
            type: 'http',
            ha_acls: [
                [
                    'name' => 'mail_acl',
                    'expression' => 'host_matches',
                    'value' => 'mail.domain.tld',
                    'casesensitive' => false,
                    'not' => false,
                ],
            ],
            a_actionitems: [
                [
                    'action' => 'tcp-request_content_lua',
                    'acl' => 'mail_acl',
                    'lua_function' => 'example function',
                ],
            ],
            backend_serverpool: 'example_backend',
            socket_stats: false,
            dontlognull: false,
            dontlog_normal: false,
            log_separate_errors: false,
            log_detailed: false,
            client_timeout: 30000,
            forwardfor: false,
            httpclose: 'http-keep-alive',
            advanced: '',
        );
        $frontend->create();

        # Ensure the object was created successful and we can read it and its child objects
        $read_frontend = HAProxyFrontend::query(id: $frontend->id);
        $this->assert_is_true($read_frontend->exists());
        $read_frontend = $read_frontend->first();
        $this->assert_equals($read_frontend->name->value, 'example_multipledomains');
        $this->assert_equals($read_frontend->status->value, 'active');
        $this->assert_equals($read_frontend->type->value, 'http');
        $this->assert_equals($read_frontend->backend_serverpool->value, 'example_backend');
        $this->assert_equals($read_frontend->client_timeout->value, 30000);
        $this->assert_equals($read_frontend->httpclose->value, 'http-keep-alive');

        # Ensure the nested HAProxyBackend object linked up with the foreign object correctly
        $this->assert_is_true($read_frontend->backend_serverpool->get_related_model() instanceof HAProxyBackend);

        # Ensure the nested HAProxyFrontendAddress object was created successful
        $this->assert_is_not_empty($read_frontend->a_extaddr->value);
        $this->assert_is_true(HAProxyFrontendAddress::query(parent_id: $frontend->id, id: 0)->exists());
        $this->assert_equals($read_frontend->a_extaddr->value[0]['extaddr'], 'any_ipv4');
        $this->assert_equals($read_frontend->a_extaddr->value[0]['extaddr_port'], '80');
        $this->assert_equals($read_frontend->a_extaddr->value[0]['extaddr_ssl'], false);

        # Ensure the nested HAProxyFrontendACL object was created successful
        $this->assert_is_true(HAProxyFrontendACL::query(parent_id: $frontend->id, id: 0)->exists());
        $this->assert_is_not_empty($read_frontend->ha_acls->value);
        $this->assert_equals($read_frontend->ha_acls->value[0]['name'], 'mail_acl');
        $this->assert_equals($read_frontend->ha_acls->value[0]['expression'], 'host_matches');
        $this->assert_equals($read_frontend->ha_acls->value[0]['value'], 'mail.domain.tld');
        $this->assert_equals($read_frontend->ha_acls->value[0]['casesensitive'], false);
        $this->assert_equals($read_frontend->ha_acls->value[0]['not'], false);

        # Ensure the nested HAProxyFrontendAction object was created successful, and the dynamic internal_name was set
        $this->assert_is_not_empty($read_frontend->a_actionitems->value);
        $action = HAProxyFrontendAction::query(parent_id: $frontend->id, id: 0);
        $this->assert_is_true($action->exists());
        $this->assert_equals($action->first()->lua_function->internal_name, 'tcp-request_content_lualua-function');
        $this->assert_equals($read_frontend->a_actionitems->value[0]['action'], 'tcp-request_content_lua');
        $this->assert_equals($read_frontend->a_actionitems->value[0]['acl'], 'mail_acl');
        $this->assert_equals($read_frontend->a_actionitems->value[0]['lua_function'], 'example function');

        # Update the object and ensure the changes were successful
        $frontend->status->value = 'disabled';
        $frontend->update();
        $read_frontend = HAProxyFrontend::query(id: $frontend->id)->first();
        $this->assert_equals($read_frontend->status->value, 'disabled');

        # Delete the object and ensure it was deleted
        $frontend->delete();
        $this->assert_is_false(HAProxyFrontend::query(id: $frontend)->exists());
    }
}
