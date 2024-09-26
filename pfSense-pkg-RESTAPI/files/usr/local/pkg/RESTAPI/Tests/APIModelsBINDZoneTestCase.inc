<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDView;
use RESTAPI\Models\BINDZone;

class APIModelsBINDZoneTestCase extends TestCase {
    const NAMED_PATH = '/var/etc/named/etc/namedb/';
    public array $required_packages = ['pfSense-pkg-bind'];
    public BINDView $view;

    /**
     * Setup the test case by creating a view for our zone.
     */
    public function setup(): void {
        $this->view = new BINDView(
            name: 'test_view',
            recursion: true,
            match_clients: ['any'],
            allow_recursion: ['any'],
            async: false,
        );
        $this->view->create(apply: true);
    }

    /**
     * Tear down the test case by deleting the view.
     */
    public function teardown(): void {
        $this->view->delete(apply: true);
    }

    /**
     * Ensure we can create, update, read and delete a zone
     */
    public function test_crud(): void {
        # Create a new master zone
        $zone = new BINDZone(
            async: false,
            name: 'test.zone.example.com',
            description: 'A test zone.',
            type: 'master',
            view: [$this->view->name->value],
            ttl: 321,
            baseip: '1.2.3.4',
            nameserver: 'ns1.example.com',
            mail: 'admin.example.com',
            serial: 123456,
            refresh: 3605,
            retry: 605,
            expire: 86405,
            minimum: 2605,
            records: [
                ['name' => 'a.example.com.', 'type' => 'A', 'rdata' => '4.3.2.1'],
                ['name' => 'mx.example.com.', 'type' => 'MX', 'rdata' => 'mail.example.com.', 'priority' => 5],
            ],
        );
        $zone->create(apply: true);

        # Read the named.conf and zone file
        $named_conf = file_get_contents(self::NAMED_PATH . 'named.conf');
        $zone_db = new Command(
            'cat ' . self::NAMED_PATH . 'master/test_view/test.zone.example.com.DB',
            trim_whitespace: true,
        );

        # Ensure the zone was created in named.conf and the zone file has expected values
        $this->assert_str_contains($named_conf, 'zone "test.zone.example.com" {');
        $this->assert_str_contains($named_conf, 'type master;');
        $this->assert_str_contains($zone_db->output, '$TTL 321');
        $this->assert_str_contains($zone_db->output, '123456 ; serial');
        $this->assert_str_contains($zone_db->output, '3605 ; refresh');
        $this->assert_str_contains($zone_db->output, '605 ; retry');
        $this->assert_str_contains($zone_db->output, '86405 ; expire');
        $this->assert_str_contains($zone_db->output, '2605 ; default_ttl');
        $this->assert_str_contains($zone_db->output, 'a.example.com. IN A 4.3.2.1');
        $this->assert_str_contains($zone_db->output, 'mx.example.com. IN MX 5 mail.example.com.');
        $this->assert_str_contains(
            $zone_db->output,
            'test.zone.example.com. IN SOA ns1.example.com. admin.example.com.',
        );

        # Update the zone
        $zone->name->value = 'new.zone.example.com';
        $zone->description->value = 'An updated test zone.';
        $zone->type->value = 'master';
        $zone->ttl->value = 123;
        $zone->baseip->value = '127.0.0.53';
        $zone->nameserver->value = 'ns2.example.com';
        $zone->mail->value = 'admin2.example.com';
        $zone->serial->value = 654321;
        $zone->refresh->value = 3606;
        $zone->retry->value = 606;
        $zone->expire->value = 86406;
        $zone->minimum->value = 2606;
        $zone->records->value = [
            ['name' => 'a2.example.com.', 'type' => 'A', 'rdata' => '5.5.5.5'],
            ['name' => 'mx2.example.com.', 'type' => 'MX', 'rdata' => 'mail2.example.com.', 'priority' => 15],
        ];
        $zone->update(apply: true);

        # Read the named.conf and zone file
        $named_conf = file_get_contents(self::NAMED_PATH . 'named.conf');
        $zone_db = new Command(
            'cat ' . self::NAMED_PATH . 'master/test_view/new.zone.example.com.DB',
            trim_whitespace: true,
        );

        # Ensure the zone was updated in named.conf and the zone file has expected values
        $this->assert_str_contains($named_conf, 'zone "new.zone.example.com" {');
        $this->assert_str_does_not_contain($named_conf, 'zone "test.zone.example.com" {');
        $this->assert_str_contains($zone_db->output, '$TTL 123');
        $this->assert_str_contains($zone_db->output, '654321 ; serial');
        $this->assert_str_contains($zone_db->output, '3606 ; refresh');
        $this->assert_str_contains($zone_db->output, '606 ; retry');
        $this->assert_str_contains($zone_db->output, '86406 ; expire');
        $this->assert_str_contains($zone_db->output, '2606 ; default_ttl');
        $this->assert_str_contains($zone_db->output, 'a2.example.com. IN A 5.5.5.5');
        $this->assert_str_contains($zone_db->output, 'mx2.example.com. IN MX 15 mail2.example.com.');
        $this->assert_str_contains(
            $zone_db->output,
            'new.zone.example.com. IN SOA ns2.example.com. admin2.example.com.',
        );

        # Delete the zone
        $zone->delete(apply: true);

        # Ensure the zone was deleted from named.conf and the zone file was removed
        $named_conf = file_get_contents(self::NAMED_PATH . 'named.conf');
        $this->assert_str_does_not_contain($named_conf, 'zone "new.zone.example.com" {');
    }
}
