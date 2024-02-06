<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\NTPSettings;
use RESTAPI\Models\NTPTimeServer;
use RESTAPI\Models\Service;

class APIModelsNTPSettingsTestCase extends TestCase {
    /**
     * Ensure the NTP server is enabled before starting tests.
     */
    public function setup(): void {
        $ntp_settings = new NTPSettings(enable: true);
        $ntp_settings->update();
    }

    /**
     * Checks that `ntpminpoll` cannot exceed the `ntpmaxpoll` value when both are numeric.
     */
    public function test_ntpminpoll_cannot_exceed_ntpmaxpoll(): void {
        $this->assert_throws_response(
            response_id: 'NTP_SERVER_NTPMINPOLL_GREATER_THAN_NTPMAXPOLL',
            code: 400,
            callable: function () {
                $ntp_settings = new NTPSettings(ntpminpoll: '5', ntpmaxpoll: '3');
                $ntp_settings->validate();
            },
        );

        $this->assert_does_not_throw(
            callable: function () {
                $ntp_settings = new NTPSettings(ntpminpoll: 'omit', ntpmaxpoll: '3');
                $ntp_settings->validate();
                $ntp_settings = new NTPSettings(ntpminpoll: '', ntpmaxpoll: '3');
                $ntp_settings->validate();
                $ntp_settings = new NTPSettings(ntpminpoll: '', ntpmaxpoll: 'omit');
                $ntp_settings->validate();
            },
        );
    }

    /**
     * Checks that the `serverauthkey` must be 20 characters are less when `serverauthalgo` is `md5`
     */
    public function test_serverauthkey_maximum_length_with_md5(): void {
        $this->assert_throws_response(
            response_id: 'NTP_SERVER_SERVERAUTHKEY_EXCEEDS_MAXIMUM_LENGTH_FOR_MD5',
            code: 400,
            callable: function () {
                $ntp_settings = new NTPSettings(
                    serverauth: true,
                    serverauthalgo: 'md5',
                    serverauthkey: 'this value is too long for an md5 key',
                );
                $ntp_settings->validate();
            },
        );

        $this->assert_does_not_throw(
            callable: function () {
                $ntp_settings = new NTPSettings(
                    serverauth: true,
                    serverauthalgo: 'md5',
                    serverauthkey: 'this value is ok',
                );
                $ntp_settings->validate();
            },
        );
    }

    /**
     * Checks that the `serverauthkey` must be a 40 character hex encoded value when `serverauthalgo` is `sha1`
     */
    public function test_serverauthkey_40_char_hex_value_with_sha1(): void {
        # Ensure the value must be exactly 40 characters
        $this->assert_throws_response(
            response_id: 'NTP_SERVER_SERVERAUTHKEY_SHA1_CHARACTER_REQUIREMENT_UNMET',
            code: 400,
            callable: function () {
                $ntp_settings = new NTPSettings(serverauth: true, serverauthalgo: 'sha1', serverauthkey: '7d2aa74a407');
                $ntp_settings->validate();
            },
        );

        # Ensure the value must be a hex value
        $this->assert_throws_response(
            response_id: 'NTP_SERVER_SERVERAUTHKEY_MUST_BE_HEX_VALUE_SHA1',
            code: 400,
            callable: function () {
                $ntp_settings = new NTPSettings(
                    serverauth: true,
                    serverauthalgo: 'sha1',
                    serverauthkey: '!077d1f49d09af79d391b9887def5b966961698!',
                );
                $ntp_settings->validate();
            },
        );

        # Ensure a known good value is accepted
        $this->assert_does_not_throw(
            callable: function () {
                $ntp_settings = new NTPSettings(
                    serverauth: true,
                    serverauthalgo: 'sha1',
                    serverauthkey: '8077d1f49d09af79d391b9887def5b966961698a',
                );
                $ntp_settings->validate();
            },
        );
    }

    /**
     * Checks that the `serverauthkey` must be a 64 character hex encoded value when `serverauthalgo` is `sha256`
     */
    public function test_serverauthkey_64_char_hex_value_with_sha256(): void {
        # Ensure the value must be exactly 40 characters
        $this->assert_throws_response(
            response_id: 'NTP_SERVER_SERVERAUTHKEY_SHA256_CHARACTER_REQUIREMENT_UNMET',
            code: 400,
            callable: function () {
                $ntp_settings = new NTPSettings(
                    serverauth: true,
                    serverauthalgo: 'sha256',
                    serverauthkey: '7d2aa74a407',
                );
                $ntp_settings->validate();
            },
        );

        # Ensure the value must be a hex value
        $this->assert_throws_response(
            response_id: 'NTP_SERVER_SERVERAUTHKEY_MUST_BE_HEX_VALUE_SHA256',
            code: 400,
            callable: function () {
                $ntp_settings = new NTPSettings(
                    serverauth: true,
                    serverauthalgo: 'sha256',
                    serverauthkey: '!d2355f6fcdb9832eadb91f0ba14a78ba7d34d0988f9856ee935e843346927e!',
                );
                $ntp_settings->validate();
            },
        );

        # Ensure a known good value is accepted
        $this->assert_does_not_throw(
            callable: function () {
                $ntp_settings = new NTPSettings(
                    serverauth: true,
                    serverauthalgo: 'sha256',
                    serverauthkey: 'd2355f6fcdb9832eadb91f0ba14a78ba7d34d0988f9856ee935e843346927e6b',
                );
                $ntp_settings->validate();
            },
        );
    }

    /**
     * Checks that the `ntpd` service is not running when `enable` is set to `false`.
     */
    public function test_enable_controls_ntpd_service(): void {
        # Disable the NTP server and ensure the ntpd.conf is missing and ntpd is not running
        $ntp_settings = new NTPSettings(enable: false);
        $ntp_settings->update();
        $this->assert_is_false(is_file('/var/etc/ntpd.conf'));
        $this->assert_is_false(Service::query(name: 'ntpd', status: true)->exists());

        # Enable the NTP server and ensure the ntpd.conf is found and ntpd is running
        $ntp_settings = new NTPSettings(enable: true);
        $ntp_settings->update();
        $this->assert_is_true(is_file('/var/etc/ntpd.conf'));
        $this->assert_is_true(Service::query(name: 'ntpd', status: true)->exists());
    }

    /**
     * Checks that the `interface` field correctly controls which interfaces are configured to listen in ntpd.conf
     */
    public function test_interface_controls_listening_interfaces(): void {
        # Ensure specifying no interfaces allows any interface to listen
        $ntp_settings = new NTPSettings(interface: []);
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'interface listen');
        $this->assert_str_does_not_contain($ntpd_conf, 'interface ignore');

        # Ensure specifying specific interfaces allows only those interfaces to listen
        $ntp_settings = new NTPSettings(interface: ['lan']);
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $if = $ntp_settings->interface->get_related_models()->first()->if->value;
        $this->assert_str_contains($ntpd_conf, "interface listen $if");
        $this->assert_str_contains($ntpd_conf, 'interface ignore all');
        $this->assert_str_contains($ntpd_conf, 'interface ignore wildcard');
    }

    /**
     * Checks that the `orphan` and `ntpmaxpeers` correctly control the type of service line in ntpd.conf
     */
    public function test_orphan_and_ntpmaxpeers_control_tos(): void {
        # Update the orphan and ntpmaxpeers and ensure the tos line in ntpd.conf changes accordingly
        $ntp_settings = new NTPSettings(orphan: 13, ntpmaxpeers: 14);
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'tos orphan 13 maxclock 14');

        # One more time with different values for good measure
        $ntp_settings = new NTPSettings(orphan: 1, ntpmaxpeers: 4);
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'tos orphan 1 maxclock 4');
    }

    /**
     * Checks that the `ntpminpoll` and `ntpmaxpoll` are correctly represented in ntpd.conf
     */
    public function test_ntpminpoll_and_ntpmaxpoll_found_in_ntpd_conf(): void {
        # Update the orphan and ntpmaxpeers and ensure the tos line in ntpd.conf changes accordingly
        $ntp_settings = new NTPSettings(ntpminpoll: '5', ntpmaxpoll: '14');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'minpoll 5 maxpoll 14');

        # One more time with different values for good measure
        $ntp_settings = new NTPSettings(ntpminpoll: '3', ntpmaxpoll: '10');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'minpoll 3 maxpoll 10');

        # Ensure ntpminpoll omit complete omits the minpoll item
        $ntp_settings = new NTPSettings(ntpminpoll: 'omit', ntpmaxpoll: '10');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'minpoll');

        # Ensure ntpmaxpoll omit complete omits the maxpoll item
        $ntp_settings = new NTPSettings(ntpminpoll: '3', ntpmaxpoll: 'omit');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'maxpoll');
    }

    /**
     * Checks that the `dnsresolv` field correctly sets the IP type for timeservers
     */
    public function test_dnsresolv_found_in_ntpd_conf(): void {
        # dnsresolv does not work on pool timeservers, add a normal timeserver for this test
        $timeserver = new NTPTimeServer(timeserver: 'example.com', type: 'server');
        $timeserver->create();

        # Update `dnsresolv` so only IPv4 is used
        $ntp_settings = new NTPSettings(dnsresolv: 'inet');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'server -4 example.com');

        # Update `dnsresolv` so only IPv6 is used
        $ntp_settings = new NTPSettings(dnsresolv: 'inet6');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'server -6 example.com');

        # Update `dnsresolv` so it is automatically determined
        $ntp_settings = new NTPSettings(dnsresolv: 'auto');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'server example.com');

        # Delete the timeserver
        $timeserver->delete();
    }

    /**
     * Checks that the `logpeer` and `logsys` fields correctly set the logconfig ntpd.conf value
     */
    public function test_logconfig_fields(): void {
        # Set the field names and their expected value in the logconfig ntpd.conf item
        $logconfig_fields = ['logpeer' => '+peerall', 'logsys' => '+sysall'];

        # Loop through each logconfig field and ensure it correctly sets the logconfig value in ntpd.conf
        foreach ($logconfig_fields as $model_field => $ntpd_logconfig_value) {
            # Ensure setting this field to true makes the corresponding value become present in ntpd.conf
            $ntp_settings = new NTPSettings(data: [$model_field => true]);
            $ntp_settings->update();
            $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
            $this->assert_str_contains($ntpd_conf, $ntpd_logconfig_value);

            # Ensure setting this field to false removes the corresponding logconfig value from ntpd.conf
            $ntp_settings = new NTPSettings(data: [$model_field => false]);
            $ntp_settings->update();
            $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
            $this->assert_str_does_not_contain($ntpd_conf, $ntpd_logconfig_value);
        }
    }

    /**
     * Checks that the `clockstats`, `loopstats` and `peerstats` fields correctly set the statistics ntpd.conf value
     */
    public function test_statistics_fields(): void {
        # Set the field names and their expected value in the statistics ntpd.conf item
        $statistics_fields = ['clockstats', 'loopstats', 'peerstats'];

        # Loop through each logconfig field and ensure it correctly sets the statistics value in ntpd.conf
        foreach ($statistics_fields as $model_field) {
            # Ensure setting this field to true makes the corresponding value become present in ntpd.conf
            $ntp_settings = new NTPSettings(data: [$model_field => true]);
            $ntp_settings->update();
            $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
            $this->assert_str_contains($ntpd_conf, $model_field);

            # Ensure setting this field to false removes the corresponding statistics value from ntpd.conf
            $ntp_settings = new NTPSettings(data: [$model_field => false]);
            $ntp_settings->update();
            $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
            $this->assert_str_does_not_contain($ntpd_conf, $model_field);
        }
    }

    /**
     * Checks that the `leapsec` field value is correctly written to /var/db/leap-seconds after applying.
     */
    public function test_leap_seconds(): void {
        # Set the leapsec field and ensure the value set is present in /var/db/leap-seconds
        $ntp_settings = new NTPSettings(leapsec: 'test value');
        $ntp_settings->update();
        $this->assert_equals($ntp_settings->leapsec->value, file_get_contents('/var/db/leap-seconds'));

        # Ensure the leapfile is specified in ntpd.conf
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'leapfile /var/db/leap-seconds');

        # Again with new values for good measure
        $ntp_settings = new NTPSettings(leapsec: 'updated value');
        $ntp_settings->update();
        $this->assert_equals($ntp_settings->leapsec->value, file_get_contents('/var/db/leap-seconds'));

        # Ensure the leapfile value is removed from ntpd.conf if the leapsec value is empty
        $ntp_settings = new NTPSettings(leapsec: '');
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'leapfile /var/db/leap-seconds');
    }

    /**
     * Checks that the `serverauthkey` and `serverauthalgo` is correctly represented in ntpd.conf and/or ntp.keys
     */
    public function test_serverauth(): void {
        # Ensure MD5 server authentication is correctly set in ntp.keys
        $ntp_settings = new NTPSettings(serverauth: true, serverauthkey: 'testkey', serverauthalgo: 'md5');
        $ntp_settings->update();
        $ntp_keys = file_get_contents('/var/etc/ntp.keys');
        $this->assert_str_contains(
            $ntp_keys,
            strtoupper($ntp_settings->serverauthalgo->value) . ' ' . $ntp_settings->serverauthkey->value,
        );

        # Ensure SHA1 server authentication is correctly set in ntp.keys
        $ntp_settings = new NTPSettings(
            serverauth: true,
            serverauthkey: 'd2355f6fcdb9832eadb91f0ba14a78ba7d34d098',
            serverauthalgo: 'sha1',
        );
        $ntp_settings->update();
        $ntp_keys = file_get_contents('/var/etc/ntp.keys');
        $this->assert_str_contains(
            $ntp_keys,
            strtoupper($ntp_settings->serverauthalgo->value) . ' ' . $ntp_settings->serverauthkey->value,
        );

        # Ensure SHA256 server authentication is correctly set in ntp.keys
        $ntp_settings = new NTPSettings(
            serverauth: true,
            serverauthkey: 'd2355f6fcdb9832eadb91f0ba14a78ba7d34d0988f9856ee935e843346927e6b',
            serverauthalgo: 'sha256',
        );
        $ntp_settings->update();
        $ntp_keys = file_get_contents('/var/etc/ntp.keys');
        $this->assert_str_contains(
            $ntp_keys,
            strtoupper($ntp_settings->serverauthalgo->value) . ' ' . $ntp_settings->serverauthkey->value,
        );

        # Ensure the keys file is listed in ntpd.conf when serverauth is enabled
        $ntp_settings = new NTPSettings(serverauth: true);
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_contains($ntpd_conf, 'keys /var/etc/ntp.keys');

        # Ensure the keys file is not listed in ntpd.conf when serverauth is disabled
        $ntp_settings = new NTPSettings(serverauth: false);
        $ntp_settings->update();
        $ntpd_conf = file_get_contents('/var/etc/ntpd.conf');
        $this->assert_str_does_not_contain($ntpd_conf, 'keys /var/etc/ntp.keys');
    }
}
