<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\NTPTimeServer;

class APIModelsNTPTimeServerTestCase extends TestCase {
    /**
     * Checks that the get_timeservers_from_internal() method correctly reads timeserver values from their internal
     * config values
     */
    public function test_get_timeservers_from_internal(): void {
        # Create a timeserver and ensure we can read it correctly from internal
        $timeserver = new NTPTimeServer(timeserver: 'example.com', type: 'pool', prefer: true, noselect: false);
        $timeserver->create();
        $this->assert_equals($timeserver->get_timeservers_from_internal()[$timeserver->id], [
            'timeserver' => 'example.com',
            'type' => 'pool',
            'prefer' => true,
            'noselect' => false,
        ]);

        # Another for good measure
        $timeserver->from_representation(timeserver: 'ntp.example.com', type: 'server', prefer: false, noselect: true);
        $timeserver->update();
        $this->assert_equals($timeserver->get_timeservers_from_internal()[$timeserver->id], [
            'timeserver' => 'ntp.example.com',
            'type' => 'server',
            'prefer' => false,
            'noselect' => true,
        ]);

        # Delete the timeserver
        $timeserver->delete();
    }

    /**
     * Checks that `timeserver` values that end with pool.ntp.org have their `type` force set as `pool`
     */
    public function test_time_server_with_auto_pool_suffix_forced_as_pool_type(): void {
        # Try to create a timeserver that uses the auto pool suffix and ensure its type is automatically changed to pool
        $timeserver = new NTPTimeServer(timeserver: NTPTimeServer::AUTO_POOL_SUFFIX, type: 'server');
        $timeserver->validate();
        $this->assert_equals($timeserver->type->value, 'pool');

        $timeserver = new NTPTimeServer(timeserver: 'sub.' . NTPTimeServer::AUTO_POOL_SUFFIX, type: 'server');
        $timeserver->validate();
        $this->assert_equals($timeserver->type->value, 'pool');
    }

    /**
     * Check that the get_next_id() method correctly determines the next available ID.
     */
    public function test_get_next_id(): void {
        # Manually set 3 time servers and ensure the next available ID is 3 (starts at 0)
        $timeserver = new NTPTimeServer();
        $timeserver->set_config(
            path: 'system/timeservers',
            value: 'ntp0.example.com ntp1.example.com ntp2.example.com',
        );
        $this->assert_equals($timeserver->get_next_id(), 3);

        # Manually set 2 time servers and ensure the next available ID is 2 (starts at 0)
        $timeserver->set_config(path: 'system/timeservers', value: 'ntp0.example.com ntp1.example.com');
        $this->assert_equals($timeserver->get_next_id(), 2);

        # Manually set no time servers and ensure the next available ID is 0
        $timeserver->set_config(path: 'system/timeservers', value: '');
        $this->assert_equals($timeserver->get_next_id(), 0);
    }

    /**
     * Checks that the _create() method correctly adds a new timeserver to configuration
     */
    public function test_crud(): void {
        # Create a timeserver and ensure it is stored correctly in config
        $timeserver = new NTPTimeServer(timeserver: 'example.com', type: 'peer', prefer: true, noselect: true);
        $timeserver->create();
        $this->assert_str_contains($timeserver->get_config('system/timeservers', ''), $timeserver->timeserver->value);
        $this->assert_str_contains($timeserver->get_config('ntpd/ispeer', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/ispool', ''), $timeserver->timeserver->value);
        $this->assert_str_contains($timeserver->get_config('ntpd/prefer', ''), $timeserver->timeserver->value);
        $this->assert_str_contains($timeserver->get_config('ntpd/noselect', ''), $timeserver->timeserver->value);

        # Update the timeserver with pool values and ensure they are found in config
        $timeserver->from_representation(timeserver: 'new.example.com', type: 'pool', prefer: false, noselect: false);
        $timeserver->update();
        $this->assert_str_contains($timeserver->get_config('system/timeservers', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/ispeer', ''), $timeserver->timeserver->value);
        $this->assert_str_contains($timeserver->get_config('ntpd/ispool', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/prefer', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain(
            $timeserver->get_config('ntpd/noselect', ''),
            $timeserver->timeserver->value,
        );

        # Update the timeserver with server values and ensure they are found in config
        $timeserver->from_representation(timeserver: 'new.example.com', type: 'server', prefer: true, noselect: false);
        $timeserver->update();
        $this->assert_str_contains($timeserver->get_config('system/timeservers', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/ispeer', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/ispool', ''), $timeserver->timeserver->value);
        $this->assert_str_contains($timeserver->get_config('ntpd/prefer', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain(
            $timeserver->get_config('ntpd/noselect', ''),
            $timeserver->timeserver->value,
        );

        # Delete the timeserver and ensure it is no longer present in config
        $timeserver->delete();
        $this->assert_str_does_not_contain(
            $timeserver->get_config('system/timeservers', ''),
            $timeserver->timeserver->value,
        );
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/ispeer', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/ispool', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain($timeserver->get_config('ntpd/prefer', ''), $timeserver->timeserver->value);
        $this->assert_str_does_not_contain(
            $timeserver->get_config('ntpd/noselect', ''),
            $timeserver->timeserver->value,
        );
    }

    /**
     * Checks that timeservers are correctly represented in ntpd.conf
     */
    public function test_timeservers_found_in_ntpd_conf(): void {
        # Create timeservers for each type and ensure they are correctly seen in ntpd.conf
        $pool_ts = new NTPTimeServer(timeserver: 'pool.example.com', type: 'pool', prefer: true, noselect: false);
        $pool_ts->create();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'pool pool.example.com iburst maxpoll 9 prefer');
        $pool_ts->delete();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'pool.example.com');

        $peer_ts = new NTPTimeServer(timeserver: 'peer.example.com', type: 'peer', prefer: false, noselect: true);
        $peer_ts->create();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'peer peer.example.com maxpoll 9 noselect');
        $peer_ts->delete();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'peer.example.com');

        $server_ts = new NTPTimeServer(timeserver: 'server.example.com', type: 'server', prefer: true, noselect: true);
        $server_ts->create();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'server server.example.com iburst maxpoll 9 prefer noselect');
        $server_ts->delete();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'server.example.com');
    }
}
