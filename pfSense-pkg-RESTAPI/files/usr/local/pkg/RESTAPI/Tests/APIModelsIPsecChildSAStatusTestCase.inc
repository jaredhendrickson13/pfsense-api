<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecChildSAStatus;
use RESTAPI\Models\IPsecSAStatus;

class APIModelsIPsecChildSAStatusTestCase extends TestCase {
    /**
     * Ensure we can read the IPsecSAStatus model data.
     */
    public function test_read(): void {
        global $mock_internal_objects;
        $mock_internal_objects = APIModelsIPsecSAStatusTestCase::MOCK_DATA[0]['child-sas'];
        $status = new IPsecChildSAStatus(id: 0, parent_id: 0);

        # Ensure child SA values are present
        $this->assert_equals($status->name->value, 'con2');
        $this->assert_equals($status->uniqueid->value, 9261);
        $this->assert_equals($status->reqid->value, 4);
        $this->assert_equals($status->state->value, 'INSTALLED');
        $this->assert_equals($status->mode->value, 'TUNNEL');
        $this->assert_equals($status->protocol->value, 'ESP');
        $this->assert_equals($status->encap->value, true);
        $this->assert_equals($status->spi_in->value, 'a9sd8f9as8d9f');
        $this->assert_equals($status->spi_out->value, 'asd9fa8sd9f8');
        $this->assert_equals($status->encr_alg->value, 'AES_CBC');
        $this->assert_equals($status->encr_keysize->value, 256);
        $this->assert_equals($status->integ_alg->value, 'HMAC_SHA2_256_128');
        $this->assert_equals($status->dh_group->value, 'MODP_2048');
        $this->assert_equals($status->bytes_in->value, 1952934);
        $this->assert_equals($status->packets_in->value, 3116);
        $this->assert_equals($status->use_in->value, 1);
        $this->assert_equals($status->bytes_out->value, 1397428);
        $this->assert_equals($status->packets_out->value, 2343);
        $this->assert_equals($status->use_out->value, 1);
        $this->assert_equals($status->rekey_time->value, 860);
        $this->assert_equals($status->life_time->value, 1361);
        $this->assert_equals($status->install_time->value, 2239);
        $this->assert_equals($status->local_ts->value, ['10.0.0.0/9|/0']);
        $this->assert_equals($status->remote_ts->value, ['192.100.0.0/16|/0']);

        # Unset mock data
        $mock_internal_objects = null;
    }
}
