<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\HAProxyBackend;
use RESTAPI\Models\HAProxyFrontend;
use RESTAPI\Models\HAProxyFrontendACL;
use RESTAPI\Models\HAProxyFrontendAction;
use RESTAPI\Models\HAProxyFrontendAddress;
use RESTAPI\Models\HAProxyFrontendCertificate;

class APIModelsHAProxyFrontendTestCase extends TestCase {
    private HAProxyBackend $backend;
    private Certificate $main_crt;
    private Certificate $alt_crt;
    public array $required_packages = ['pfSense-pkg-haproxy'];

    /**
     * Setup the test case.
     */
    public function setup(): void {
        $this->main_crt = new Certificate(
            descr: 'example_main_certificate',
            crt: '-----BEGIN CERTIFICATE-----\nMIIEETCCAvmgAwIBAgIUFj+2UXmP+7z2RqSU1NquTnaJu3owDQYJKoZIhvcNAQEL\nBQAwgZcxCzAJBgNVBAYTAlVTMREwDwYDVQQIDAhNaWNoaWdhbjEQMA4GA1UEBwwH\nTGFuc2luZzEcMBoGA1UECgwTRXhhbXBsZSBDb21wYW55IExMQzELMAkGA1UECwwC\nSVQxFDASBgNVBAMMC2V4YW1wbGUuY29tMSIwIAYJKoZIhvcNAQkBFhNleGFtcGxl\nQGV4YW1wbGUuY29tMB4XDTI1MDEyMTE3MjU0OFoXDTM1MDEyMTE3MjU0OFowgZcx\nCzAJBgNVBAYTAlVTMREwDwYDVQQIDAhNaWNoaWdhbjEQMA4GA1UEBwwHTGFuc2lu\nZzEcMBoGA1UECgwTRXhhbXBsZSBDb21wYW55IExMQzELMAkGA1UECwwCSVQxFDAS\nBgNVBAMMC2V4YW1wbGUuY29tMSIwIAYJKoZIhvcNAQkBFhNleGFtcGxlQGV4YW1w\nbGUuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt3Rc+FmQB/fk\n33faRhm9Gg9a67RHHCeEdB6OubfvSXN8/InYP6o+Ln5Wp8xmYqBtHcjnfO/iIYOH\nbl87pvKR7AiPSE1IfMSZxp8xiOhbUJwPt9QOXQA8mExW0M740+PNBQd8k+UXUb0g\n3ammsIo4NlNYvhle8l8Q2YBsK7KwsxLp6PjNLmnKOFm+BldALz1Sj3ER8XGuOfsZ\nALqxQiG87vTO2kSBSTqmorie5fgIcVVKkKhNmSfo9TWVKAr2rHBl2ep0L8o2QgeY\nuA/mPrlxc1V/JeybbENQqkiJVNz1F2120HKXGxHNv2VzWHN0rOAThG8Tmx4ej/FQ\nMRPsCKNqpwIDAQABo1MwUTAdBgNVHQ4EFgQUTlotGm0X/eXy7IpexMyOfHDtSPsw\nHwYDVR0jBBgwFoAUTlotGm0X/eXy7IpexMyOfHDtSPswDwYDVR0TAQH/BAUwAwEB\n/zANBgkqhkiG9w0BAQsFAAOCAQEAQKfpuM1jY86fd+oyfyEQJPkm4optyAdn/6FH\n+jsHejgAzwgMhLYldG8pEXR21zJZMIZuv9NqLhTJTbfbK7hZ4S3PhodYBU24xjyu\ndMzCqpW1nS8T/Hi1ULKoWQ/7fBaR1iLwA2gIvmKCWfTb0WrObxNZe3l95q1srli4\nN1XPMvmD53aBU4Yx4+pKQyPkS0tjUl2+sq/Ry1uh1Wz/2TIxn2zPfL0wnidXTwGQ\n33raRCxt+kJ0a7dmKo7ejh5dMx0o1yXeuttBiMddTJsK8GkHmX/ZJXZlSLAkJV3p\nEP/ZM3VAYeoj7iS5NC5XqXmOewiOor4ok75x2CoeoudAKENf1Q==\n-----END CERTIFICATE-----\n',
            prv: '-----BEGIN PRIVATE KEY-----\nMIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQC3dFz4WZAH9+Tf\nd9pGGb0aD1rrtEccJ4R0Ho65t+9Jc3z8idg/qj4uflanzGZioG0dyOd87+Ihg4du\nXzum8pHsCI9ITUh8xJnGnzGI6FtQnA+31A5dADyYTFbQzvjT480FB3yT5RdRvSDd\nqaawijg2U1i+GV7yXxDZgGwrsrCzEuno+M0uaco4Wb4GV0AvPVKPcRHxca45+xkA\nurFCIbzu9M7aRIFJOqaiuJ7l+AhxVUqQqE2ZJ+j1NZUoCvascGXZ6nQvyjZCB5i4\nD+Y+uXFzVX8l7JtsQ1CqSIlU3PUXbXbQcpcbEc2/ZXNYc3Ss4BOEbxObHh6P8VAx\nE+wIo2qnAgMBAAECggEACkAZtmoGsV30BK/P2zfdSDilwLgaJbloM2FxDHEkZnW/\nUjr/gWXT7fQdh72qK4ZBjTb9sY4+aSyjY2g1zzkMLI85Nksp1YgvUW5GrcV8iK9X\nn31dRMFadrtZOcyUEal6T0wg9VXebNuaRlhlhHAMqQMU2SEAxvPHNL1HY1UA04M5\neWlwgb+WN8ocQgAWT42L/Ys3bLWilA9HfTgotLcM2AonF7MhR8kKubRzCBupaRYI\nRLD+EPJpvz7hbvLjTi8OUc2Ksl8L2ZhuIVLqYljBWWHCEj+z97i5oaUSMo8ZujVo\nMPGQkM6Blh8viR9TSVXH/o+iGnlGgABn3yESNrUUTQKBgQD0Dq1EIXtiXtTOud6O\nI4l9tieb9y9hZXCgaemr2IiH5OpsnALf5aBqNA7WhRFvBN5Jby6F5XhSLPHthQLr\nWWouyJIFUuTxGMBQE3JFOTxn+mikH1nsiuZTqYvZtbfSyiuJA2qMpvegowX+Y0gX\n3HCIUBmNOZW44GsQyYxlzBZeDQKBgQDAboLRGnDTZF+Kg+XcrmTOchW3vrzhIaN+\n25wLxYBBuL8po0XchA2lR9DYl/z+HLl24Qv3x/kWDfqw3YMjeSuJDCiRW6k2nyOs\nBq9iIWJyG120retNeil/ee+Oi4Ot6heMBLhsiPokNiyQErFjFVDja0NWNI3RLT3O\nXl0NnC3ygwKBgQDPIWmyCgySl1NbQ+p7jfORqrgBKY+fxJ0Kt/jmrPZrB0Bh/yJ9\nbYBwwk/ZhsQeCe6yOlcssm0kqRqGcD3jClcQ747mT80YRd5p16dC0unOgXqv2B/U\nnqoOVTXpBs3Fa2n5ddnHF6nQPqvGk/JKtf8X4YteDYJhjq2vaK7scoKFPQKBgQCK\nz20IP6WBp9cPiUPH4/kNvtgRusJvOB/DQ9GV4Ds7rXQgKgocHxnkzyBFDaZO/BOB\nQKesbpEsqpra+H8/mImiC6Y77L3si/Fkh1H+XIyyOTFoQ0kIk4XZ0fDPh65ORx4D\nP+h1Sn3+nyGYMBPLoLW/x0FErxZZ/OOnuQQw7Cds1wJ/BRP+3rVtm8hof1hk7gRS\nUlhuViYUScnBfuuCGp5Fdj1tfuMdqDA0IB7u+7V4sIy4z8WBmeZFsjE67eMTv9uR\nC6tjrdA8DUjiUnSZTwBWzjPZ8u87BtD4y3TZqdhA/a2yd1u6L9Uw7g/JzDpAiMIG\n5Wj8TdEgyCArcswKTBRvZg==\n-----END PRIVATE KEY-----\n'
        );
        $this->alt_crt = new Certificate(
            descr: 'example_alt_certificate',
            crt: '-----BEGIN CERTIFICATE-----\nMIIEITCCAwmgAwIBAgIUEuRnc2GSFACHYusB06+IAjZ/XZ0wDQYJKoZIhvcNAQEL\nBQAwgZ8xCzAJBgNVBAYTAlVTMREwDwYDVQQIDAhJbGxpbm9pczEQMA4GA1UEBwwH\nQ2hpY2FnbzEcMBoGA1UECgwTRXhhbXBsZSBDb21wYW55IExMQzELMAkGA1UECwwC\nSVQxGDAWBgNVBAMMD2FsdC5leGFtcGxlLmNvbTEmMCQGCSqGSIb3DQEJARYXZXhh\nbXBsZUBhbHQuZXhhbXBsZS5jb20wHhcNMjUwMTIxMTcyODQxWhcNMzUwMTIxMTcy\nODQxWjCBnzELMAkGA1UEBhMCVVMxETAPBgNVBAgMCElsbGlub2lzMRAwDgYDVQQH\nDAdDaGljYWdvMRwwGgYDVQQKDBNFeGFtcGxlIENvbXBhbnkgTExDMQswCQYDVQQL\nDAJJVDEYMBYGA1UEAwwPYWx0LmV4YW1wbGUuY29tMSYwJAYJKoZIhvcNAQkBFhdl\neGFtcGxlQGFsdC5leGFtcGxlLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\nAQoCggEBAK/xDL+IK455MhTv86XU/PSAzrrPNbD1f03KxybPr/xX2AFTnIYkez53\nHWMnpizn5e1HoOqXHynptE/wGLsgE105H7aGZ8t52jbqifIIUhTi+HBRC2Yg4YRf\n6PbR+1lvMaCofotz4zMtZtpp/XiOXrSHqulGcV+bSgtu2uxu9wIfwxN2Mvg/8sk7\noqNimwLpuRU0ZqAYt6dFsxsvCZC8gCywYL45ML2KxociWAjFDp0obj7g/j5XSxEj\nAz5KfZdJhxl0NSCD+ewWUYYsnCRWHbojtqlQnGY4QXTYZ6TLGyOB5jPYK7cRCMXb\npsveo24c29UBStiJ1u87gu+PGqUhpVsCAwEAAaNTMFEwHQYDVR0OBBYEFGhql+aY\nSxnUlNLett0Oqb+O6OcqMB8GA1UdIwQYMBaAFGhql+aYSxnUlNLett0Oqb+O6Ocq\nMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAKQh880ZOtQeW2VH\nMqILfcF/qf/Ge5kCmS/8mTAAZinvSuCISblcBIMgei3L7LObav48u2BmNFor1o1o\naB4iSMLRyEGNrnYhCy4IrCWw7Y2YjzYJz5oEUy9jJ9S4zOZGV/qNVIhiBSJN274Z\nU8Q8kmF/Tpze1G65AipUgbaw3fx8VgvlOotemtDNsIwMA9WMDnXMqVOBIs5yWWp9\nT9f6Y8to/UcNm0JS7LExn9xTi5WAxstUzk5tWsvJ5pzEppUymSdkSpzObRcqRLZa\n5YeFbd+svKfAue8iukzwuukDsHl8lmwkSASFRwdh3391JC7lpxenAFjs0I1eXUBs\nBz371as=\n-----END CERTIFICATE-----\n',
            prv: '-----BEGIN PRIVATE KEY-----\nMIIEuwIBADANBgkqhkiG9w0BAQEFAASCBKUwggShAgEAAoIBAQCv8Qy/iCuOeTIU\n7/Ol1Pz0gM66zzWw9X9Nyscmz6/8V9gBU5yGJHs+dx1jJ6Ys5+XtR6Dqlx8p6bRP\n8Bi7IBNdOR+2hmfLedo26onyCFIU4vhwUQtmIOGEX+j20ftZbzGgqH6Lc+MzLWba\naf14jl60h6rpRnFfm0oLbtrsbvcCH8MTdjL4P/LJO6KjYpsC6bkVNGagGLenRbMb\nLwmQvIAssGC+OTC9isaHIlgIxQ6dKG4+4P4+V0sRIwM+Sn2XSYcZdDUgg/nsFlGG\nLJwkVh26I7apUJxmOEF02GekyxsjgeYz2Cu3EQjF26bL3qNuHNvVAUrYidbvO4Lv\njxqlIaVbAgMBAAECgf9u/iKMsdrIhqyRiM6sTzFFDf1c3FuCx10INmRsl5juGHys\nWZLlDR6yyXJAm0K6EZF7nzRkyfFe/5BA3ba8Vf/hT6gx/Zh9ROHkwxFDHvypMIaK\nVJZcV2HtoJPXIaDvSraEI8exMeqi3oGESFkfNLGKMgwgHdBoAA82Jzj07wvkyhjG\nuB2KZ0x8bDOuan9nM8EfJ6ncN8/biOolPfOv/ojUDjp0yN//gkPQTij50Dj8dxEU\nwobi9AE3diszC8gjj5i4mX14qWKC6/xxGcSHoxsPYUJt7nYGb5RRZciqChroob0o\nDP8VpQR/SnqY8OU97xExbjS0zjvlhipPgiMbGKECgYEA3SVdbR1niZrNEly2S9y1\nsL+pzl2t6KA6pd6XFvlMqgqAtCo9MOVDXkqfptKt9oT+MC8SkbwFFIYLWXHMBNcO\nqfVCIK1mp5scHRgiBdmjo0yEXIsJ5J1y+bm8mTlyGJHbefh7FQoHnrJ/7X9WXuQo\nZWetv196PQuOiLUL49NukGECgYEAy6vQBznsessCeetXOHOO7ap9dieoWzFK0YIX\nZaA9jX+REgIhhIxgWkBcsMQAlm5B74YskYcbsrUm510xn0stzzh91O6DyoM3PJJc\nrqeUJU+HEYELcuZCVy4RV2+XMjSLPc5Sivgbe0mOfHKz3szNIwGKZGuRrWIT6djA\nUyuuvzsCgYA7Qdzj9SIeTD2xMuiiMVT6NJFyu8Vy9SRh9+AyhsDVO5U7MapN5NEF\nfieDkyaMTRyzmpl2NZSC6Fw4LncFwP6r6g07JlvAg56n6SSMvsHLayB4j8Up3krF\nwdBYmn0JOmQ+LagFfjeGfbwrzpg9OxPcAEkdY58kpmkeFq1F8gzJYQKBgQCrb0d/\njaYcBd1julCEV+Cq9KL+XYs8l1Ue3J3NEQA+pm359ok4BODllt9ueszz0oL1lr7V\nECCkYA8LGEq4hpNcpXRlUw7j22cXOSuCZ9QvQ62xcims4Vxd/YzjSC7AN7IsqAqa\npxSaxeOFpRb6JvFU0esnc9P/WVrhr2zTlSVWGQKBgCFaRF3yP/Tc8EvYEhCPgRLv\nsM5FZUe/86gWoWUhSoXcPpW+CipjatcB3kLxXXcYMFYl4cVe5KwTemMmDgMMiMdP\nwn+R2SGvU89GTCTWmlUeD4sesNVFcHMe2Rw0pBf1BTh5KIsA9BXRe13bWHU+x8T2\nSIeb+boJuWOH4flIIBud\n-----END PRIVATE KEY-----\n'
        );
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
        $this->main_crt->create();
        $this->alt_crt->create();
    }

    /**
     * Tear down the test case.
     */
    public function teardown(): void {
        $this->backend->delete();
        $this->main_crt->delete();
        $this->alt_crt->delete();
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
            ssloffloadcert: $this->main_crt->refid->value,
            ha_certificates: [
                [
                    'ssl_certificate' => $this->alt_crt->refid->value
                ],
            ]
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
        $this->assert_equals($read_frontend->ssloffloadcert->value, $this->main_crt->refid->value);

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

        # Ensure the nested HAProxyFronendCertificate object was created successful
        $this->assert_is_not_empty($read_frontend->ha_certificates->value);
        $this->assert_is_true(HAProxyFrontendCertificate::query(parent_id: $frontend->id, id: 0)->exists());
        $this->assert_equals($read_frontend->ha_certificates->value[0]['ssl_certificate'], $this->alt_crt->refid->value);

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
