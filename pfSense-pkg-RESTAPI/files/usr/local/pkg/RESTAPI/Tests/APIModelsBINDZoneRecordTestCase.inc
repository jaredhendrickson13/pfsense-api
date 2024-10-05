<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDView;
use RESTAPI\Models\BINDZone;
use RESTAPI\Models\BINDZoneRecord;

class APIModelsBINDZoneRecordTestCase extends TestCase {
    const NAMED_PATH = '/var/etc/named/etc/namedb/';
    public array $required_packages = ['pfSense-pkg-bind'];
    public BINDView $view;
    public BINDZone $zone;

    /**
     * Setup the test case by creating a view and zone
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

        $this->zone = new BINDZone(
            async: false,
            name: 'example.com',
            description: 'A test zone.',
            type: 'master',
            view: [$this->view->name->value],
            serial: 123456,
            baseip: '1.2.3.4',
            nameserver: 'ns1.example.com',
            mail: 'admin.example.com',
            records: [['name' => 'example.com.', 'type' => 'A', 'rdata' => '127.0.0.1']],
        );
        $this->zone->create(apply: true);
    }

    /**
     * Tear down the test case by deleting the view and zone.
     */
    public function teardown(): void {
        $this->zone->delete(apply: true);
        $this->view->delete(apply: true);
    }

    /**
     * Ensure an error is thrown when the parent zone is reverse zone and the type is not PTR or NS.
     */
    public function test_reverse_zone_type(): void {
        # Update the zone to be a reverse zone
        $this->zone->reversev4->value = true;
        $this->zone->update(apply: true);

        # Create a new zone record with an invalid type and ensure an error is thrown
        $this->assert_throws_response(
            response_id: 'BIND_ZONE_RECORD_INVALID_REVERSE_TYPE',
            code: 400,
            callable: function () {
                $record = new BINDZoneRecord(
                    parent_id: $this->zone->id,
                    name: 'in-addr.arpa.4.3.2.1',
                    type: 'A',
                    rdata: '1.2.3.4',
                );
                $record->validate();
            },
        );

        # Create a new zone record with a valid type and ensure no error is thrown
        $this->assert_does_not_throw(
            callable: function () {
                $record = new BINDZoneRecord(
                    parent_id: $this->zone->id,
                    name: 'in-addr.arpa.4.3.2.1',
                    type: 'PTR',
                    rdata: 'ptr.example.com',
                );
                $record->validate();
            },
        );

        # Update the zone to remove the reverse zone flag
        $this->zone->reversev4->value = false;
        $this->zone->update(apply: true);
    }

    /**
     * Ensure we can create, update, read and delete a zone
     */
    public function test_crud(): void {
        # Create a new zone record
        $record = new BINDZoneRecord(
            parent_id: $this->zone->id,
            rdata: '127.0.0.1',
            name: 'a.example.com.',
            type: 'A',
            async: false,
        );
        $record->create(apply: true);

        # Read the zone file and ensure the record was created
        $zone_db = new Command('cat ' . self::NAMED_PATH . 'master/test_view/example.com.DB', trim_whitespace: true);
        $this->assert_str_contains($zone_db->output, 'a.example.com. IN A 127.0.0.1');

        # Update the zone record
        $record->name->value = 'b.example.com.';
        $record->type->value = 'CNAME';
        $record->rdata->value = 'cname.example.com';
        $record->update(apply: true);

        # Read the zone file and ensure the record was updated
        $zone_db = new Command('cat ' . self::NAMED_PATH . 'master/test_view/example.com.DB', trim_whitespace: true);
        $this->assert_str_contains($zone_db->output, 'b.example.com. IN CNAME cname.example.com');
        $this->assert_str_does_not_contain($zone_db->output, 'a.example.com. IN A 127.0.0.1');

        # Delete the zone record
        $record->delete(apply: true);

        # Read the zone file and ensure the record was deleted
        $zone_db = new Command('cat ' . self::NAMED_PATH . 'master/test_view/example.com.DB', trim_whitespace: true);
        $this->assert_str_does_not_contain($zone_db->output, 'b.example.com. IN CNAME cname.example.com');
    }
}
