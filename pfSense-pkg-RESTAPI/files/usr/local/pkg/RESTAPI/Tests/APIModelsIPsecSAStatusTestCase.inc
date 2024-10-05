<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecSAStatus;

class APIModelsIPsecSAStatusTestCase extends TestCase {
    const MOCK_DATA = [
        [
            'con-id' => 'con2',
            'uniqueid' => '851',
            'version' => '2',
            'state' => 'ESTABLISHED',
            'local-host' => '1.2.3.4',
            'local-port' => '4500',
            'local-id' => '1.2.3.4',
            'remote-host' => '4.3.2.1',
            'remote-port' => '4501',
            'remote-id' => '4.3.2.1',
            'initiator-spi' => '1122334455aabbccdd',
            'responder-spi' => 'aabbccdd1122334455',
            'nat-remote' => 'yes',
            'nat-any' => 'yes',
            'encr-alg' => 'AES_CBC',
            'encr-keysize' => '256',
            'integ-alg' => 'HMAC_SHA2_256_128',
            'prf-alg' => 'PRF_HMAC_SHA2_256',
            'dh-group' => 'MODP_2048',
            'established' => '21133',
            'rekey-time' => '3014',
            'child-sas' => [
                [
                    'name' => 'con2',
                    'uniqueid' => '9261',
                    'reqid' => '4',
                    'state' => 'INSTALLED',
                    'mode' => 'TUNNEL',
                    'protocol' => 'ESP',
                    'encap' => 'yes',
                    'spi-in' => 'a9sd8f9as8d9f',
                    'spi-out' => 'asd9fa8sd9f8',
                    'encr-alg' => 'AES_CBC',
                    'encr-keysize' => '256',
                    'integ-alg' => 'HMAC_SHA2_256_128',
                    'dh-group' => 'MODP_2048',
                    'bytes-in' => '1952934',
                    'packets-in' => '3116',
                    'use-in' => '1',
                    'bytes-out' => '1397428',
                    'packets-out' => '2343',
                    'use-out' => '1',
                    'rekey-time' => '860',
                    'life-time' => '1361',
                    'install-time' => '2239',
                    'local-ts' => ['10.0.0.0/9|/0'],
                    'remote-ts' => ['192.100.0.0/16|/0'],
                ],
            ],
        ],
    ];

    /**
     * Ensure we can read the IPsecSAStatus model data.
     */
    public function test_read(): void {
        global $mock_internal_objects;
        $mock_internal_objects = self::MOCK_DATA;
        $status = new IPsecSAStatus(id: 0);

        # Ensure all values are expected
        $this->assert_equals($status->con_id->value, 'con2');
        $this->assert_equals($status->uniqueid->value, 851);
        $this->assert_equals($status->version->value, 2);
        $this->assert_equals($status->state->value, 'ESTABLISHED');
        $this->assert_equals($status->local_host->value, '1.2.3.4');
        $this->assert_equals($status->local_port->value, '4500');
        $this->assert_equals($status->local_id->value, '1.2.3.4');
        $this->assert_equals($status->remote_host->value, '4.3.2.1');
        $this->assert_equals($status->remote_port->value, '4501');
        $this->assert_equals($status->remote_id->value, '4.3.2.1');
        $this->assert_equals($status->initiator_spi->value, '1122334455aabbccdd');
        $this->assert_equals($status->responder_spi->value, 'aabbccdd1122334455');
        $this->assert_equals($status->nat_remote->value, true);
        $this->assert_equals($status->nat_any->value, true);
        $this->assert_equals($status->encr_alg->value, 'AES_CBC');
        $this->assert_equals($status->encr_keysize->value, 256);
        $this->assert_equals($status->integ_alg->value, 'HMAC_SHA2_256_128');
        $this->assert_equals($status->prf_alg->value, 'PRF_HMAC_SHA2_256');
        $this->assert_equals($status->dh_group->value, 'MODP_2048');
        $this->assert_equals($status->established->value, 21133);
        $this->assert_equals($status->rekey_time->value, 3014);

        # Ensure child SA values are present
        $this->assert_equals(count($status->child_sas->value), 1);
        $this->assert_equals($status->child_sas->value[0]['name'], 'con2');
        $this->assert_equals($status->child_sas->value[0]['uniqueid'], 9261);
        $this->assert_equals($status->child_sas->value[0]['reqid'], 4);
        $this->assert_equals($status->child_sas->value[0]['state'], 'INSTALLED');
        $this->assert_equals($status->child_sas->value[0]['mode'], 'TUNNEL');
        $this->assert_equals($status->child_sas->value[0]['protocol'], 'ESP');
        $this->assert_equals($status->child_sas->value[0]['encap'], true);
        $this->assert_equals($status->child_sas->value[0]['spi_in'], 'a9sd8f9as8d9f');
        $this->assert_equals($status->child_sas->value[0]['spi_out'], 'asd9fa8sd9f8');
        $this->assert_equals($status->child_sas->value[0]['encr_alg'], 'AES_CBC');
        $this->assert_equals($status->child_sas->value[0]['encr_keysize'], 256);
        $this->assert_equals($status->child_sas->value[0]['integ_alg'], 'HMAC_SHA2_256_128');
        $this->assert_equals($status->child_sas->value[0]['dh_group'], 'MODP_2048');
        $this->assert_equals($status->child_sas->value[0]['bytes_in'], 1952934);
        $this->assert_equals($status->child_sas->value[0]['packets_in'], 3116);
        $this->assert_equals($status->child_sas->value[0]['use_in'], 1);
        $this->assert_equals($status->child_sas->value[0]['bytes_out'], 1397428);
        $this->assert_equals($status->child_sas->value[0]['packets_out'], 2343);
        $this->assert_equals($status->child_sas->value[0]['use_out'], 1);
        $this->assert_equals($status->child_sas->value[0]['rekey_time'], 860);
        $this->assert_equals($status->child_sas->value[0]['life_time'], 1361);
        $this->assert_equals($status->child_sas->value[0]['install_time'], 2239);
        $this->assert_equals($status->child_sas->value[0]['local_ts'], ['10.0.0.0/9|/0']);
        $this->assert_equals($status->child_sas->value[0]['remote_ts'], ['192.100.0.0/16|/0']);

        # Unset mock data
        $mock_internal_objects = null;
    }
}
