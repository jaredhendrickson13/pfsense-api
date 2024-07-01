<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallSchedule;
use RESTAPI\Models\RESTAPIAccessListEntry;

class APIModelsRESTAPIAccessListEntryTestCase extends TestCase {
    /**
     * Checks that we can create, update and delete REST API Access List Entries.
     */
    public function test_crud(): void {
        # Create firewall schedules to use in the test
        $sched1 = new FirewallSchedule(
            name: 'test_schedule_1',
            timerange: [
                [
                    'month' => [1],
                    'day' => [1],
                    'hour' => '0:00-23:59',
                ],
            ],
            async: false,
        );
        $sched2 = new FirewallSchedule(
            name: 'test_schedule_2',
            timerange: [
                [
                    'month' => [2],
                    'day' => [2],
                    'hour' => '0:00-23:59',
                ],
            ],
            async: false,
        );
        $sched1->create();
        $sched2->create();

        # Create a REST API Access List Entry
        $entry = new RESTAPIAccessListEntry(
            type: 'allow',
            weight: 11,
            network: '0.0.0.0/0',
            users: [],
            sched: $sched1->name->value,
            descr: 'Test entry',
        );
        $entry->create();

        # Ensure we can read the entry
        $this->assert_is_true(RESTAPIAccessListEntry::query(id: $entry->id)->exists());

        # Update the entry
        $entry->from_representation(
            type: 'deny',
            weight: 15,
            network: '1.2.3.4/32',
            users: ['admin'],
            sched: $sched2->name->value,
            descr: 'New test entry',
        );
        $entry->update();

        # Ensure the entry was updated
        $this->assert_equals(
            RESTAPIAccessListEntry::query(id: $entry->id)
                ->first()
                ->to_representation(),
            [
                'id' => $entry->id,
                'type' => 'deny',
                'weight' => 15,
                'network' => '1.2.3.4/32',
                'users' => ['admin'],
                'sched' => $sched2->name->value,
                'descr' => 'New test entry',
            ],
        );

        # Update the entry again
        $entry->from_representation(
            type: 'allow',
            weight: 20,
            network: '4.3.2.1/24',
            users: [],
            sched: '',
            descr: 'Another test entry',
        );
        $entry->update();

        # Ensure the entry was updated
        $this->assert_equals(
            RESTAPIAccessListEntry::query(id: $entry->id)
                ->first()
                ->to_representation(),
            [
                'id' => $entry->id,
                'type' => 'allow',
                'weight' => 20,
                'network' => '4.3.2.1/24',
                'users' => [],
                'sched' => '',
                'descr' => 'Another test entry',
            ],
        );

        # Delete the entry and associated schedules
        $entry->delete();
        $sched1->delete();
        $sched2->delete();

        # Ensure the entry was deleted
        $this->assert_is_false(RESTAPIAccessListEntry::query(id: $entry->id)->exists());
        $this->assert_is_false(RESTAPIAccessListEntry::query(descr: 'Another test entry')->exists());
    }

    /**
     * Checks that the get_user_choices() method returns the expected choices.
     */
    public function test_get_user_choices(): void {
        $choices = array_keys(RESTAPIAccessListEntry::get_user_choices());
        $this->assert_is_true(in_array('admin', $choices));
    }

    /**
     * Checks that the get_schedule_choices() method returns the expected choices.
     */
    public function test_get_schedule_choices(): void {
        $sched1 = new FirewallSchedule(
            name: 'test_schedule_1',
            timerange: [
                [
                    'month' => [1],
                    'day' => [1],
                    'hour' => '0:00-23:59',
                ],
            ],
            async: false,
        );
        $sched1->create();

        $choices = array_keys(RESTAPIAccessListEntry::get_schedule_choices());
        $this->assert_is_true(in_array('', $choices));
        $this->assert_is_true(in_array($sched1->name->value, $choices));

        $sched1->delete();
    }

    /**
     * Checks that the is_ip_match() method returns the expected results.
     */
    public function test_is_ip_match(): void {
        $entry = new RESTAPIAccessListEntry(type: 'allow', network: '192.168.1.1/24');
        $entry->create();
        $this->assert_is_true($entry->is_ip_match('192.168.1.2'));
        $this->assert_is_false($entry->is_ip_match('192.168.2.1'));
        $entry->delete();
    }

    /**
     * Checks that the is_user_match() method returns the expected results.
     */
    public function test_is_user_match(): void {
        # Ensure entries with users set require a username match
        $entry = new RESTAPIAccessListEntry(type: 'allow', network: '192.168.1.1/24', users: ['admin']);
        $entry->create();
        $this->assert_is_true($entry->is_user_match('admin'));
        $this->assert_is_false($entry->is_user_match('not_admin'));

        # Ensure entries with no users set allow all users
        $entry->users->value = [];
        $entry->update();
        $this->assert_is_true($entry->is_user_match('admin'));
        $this->assert_is_true($entry->is_user_match('not_admin'));

        # Clean up
        $entry->delete();
    }

    /**
     * Checks that the is_schedule_active() method returns the expected results.
     */
    public function test_is_schedule_active(): void {
        # Get integers of the current month and day. Ensure there are no conflicts between tests dates and todays date
        $current_month = (int) date('m');
        $current_day = (int) date('d');
        $non_current_day = $current_day !== 15 ? 15 : 14;

        # Create a FirewallSchedule to test with that is currently active
        $active_sched = new FirewallSchedule(
            name: 'active_sched',
            timerange: [
                [
                    'month' => [$current_month],
                    'day' => [$current_day],
                    'hour' => '0:00-23:59',
                ],
            ],
            async: false,
        );
        $active_sched->create();

        # Create a FirewallSchedule to test with that is currently inactive
        $inactive_sched = new FirewallSchedule(
            name: 'inactive_sched',
            timerange: [
                [
                    'month' => [$current_month],
                    'day' => [$non_current_day],
                    'hour' => '0:00-23:59',
                ],
            ],
            async: false,
        );
        $inactive_sched->create();

        # Ensure the entry sees the schedule as active when the active schedule is used
        $entry = new RESTAPIAccessListEntry(
            type: 'allow',
            network: '192.168.1.1/24',
            sched: $active_sched->name->value,
        );
        $entry->create();
        $this->assert_is_true($entry->is_schedule_active());

        # Ensure the entry sees the schedule as inactive when the inactive schedule is used
        $entry->sched->value = $inactive_sched->name->value;
        $entry->update();
        $this->assert_is_false($entry->is_schedule_active());

        # Ensure the entry sees the schedule as active when no schedule is used
        $entry->sched->value = '';
        $entry->update();
        $this->assert_is_true($entry->is_schedule_active());

        # Clean up
        $entry->delete();
        $active_sched->delete();
        $inactive_sched->delete();
    }

    /**
     * Ensures the implicit deny on the is_allowed_by_acl() method works as expected.
     */
    public function test_is_allowed_by_acl_implicit_deny(): void {
        # Create an entry that allows all users but a single IP
        $entry = new RESTAPIAccessListEntry();
        $entry->replace_all([['type' => 'allow', 'network' => '1.2.3.4/32']]);

        # Ensure the implicit deny works as expected
        $this->assert_is_false($entry->is_allowed_by_acl('192.168.1.1', 'admin'));
    }

    /**
     * Ensures that an entry with a lower weight is matched before an entry with a higher weight.
     */
    public function test_is_allowed_by_acl_weight_order(): void {
        # Create two entries with different weights. Ensure the lower is a deny rule that is matched first.
        $entry1 = new RESTAPIAccessListEntry();
        $entry1->replace_all([
            ['type' => 'deny', 'weight' => 10, 'network' => '192.168.1.0/24'],
            ['type' => 'allow', 'weight' => 20, 'network' => '192.168.1.0/24'],
        ]);

        # Ensure the deny rule is matched first
        $this->assert_is_false($entry1->is_allowed_by_acl('192.168.1.1', 'admin'));

        # Re-weigh the entries so the allow rule is matched first
        $entry1->replace_all([
            ['type' => 'allow', 'weight' => 10, 'network' => '192.168.1.0/24'],
            ['type' => 'deny', 'weight' => 20, 'network' => '192.168.1.0/24'],
        ]);
        $this->assert_is_true($entry1->is_allowed_by_acl('192.168.1.1', 'admin'));
    }

    /**
     * Ensures that entries with usernames assigned only allow the specified users even if the IP matches.
     */
    public function test_is_allowed_by_acl_user_match(): void {
        # Create an entry that allows a single user
        $entry = new RESTAPIAccessListEntry();
        $entry->replace_all([['type' => 'allow', 'network' => '0.0.0.0/0', 'users' => ['admin']]]);
        $this->assert_is_true($entry->is_allowed_by_acl('1.2.3.4', 'admin'));
        $this->assert_is_false($entry->is_allowed_by_acl('1.2.3.4', 'not_admin'));

        # Ensure that an entry with no users allows all users
        $entry->replace_all([['type' => 'allow', 'network' => '0.0.0.0/0', 'users' => []]]);
        $this->assert_is_true($entry->is_allowed_by_acl('1.2.3.4', 'admin'));
        $this->assert_is_true($entry->is_allowed_by_acl('1.2.3.4', 'not_admin'));
    }

    /**
     * Ensures that entries with schedules only allow access when the schedule is active.
     */
    public function test_is_allowed_by_acl_schedule_match(): void {
        # Get integers of the current month and day. Ensure there are no conflicts between tests dates and todays date
        $current_month = (int) date('m');
        $current_day = (int) date('d');
        $non_current_day = $current_day !== 15 ? 15 : 14;

        # Create a FirewallSchedule to test with that is currently active
        $active_sched = new FirewallSchedule(
            name: 'active_sched',
            timerange: [
                [
                    'month' => [$current_month],
                    'day' => [$current_day],
                    'hour' => '0:00-23:59',
                ],
            ],
            async: false,
        );
        $active_sched->create();

        # Create a FirewallSchedule to test with that is currently inactive
        $inactive_sched = new FirewallSchedule(
            name: 'inactive_sched',
            timerange: [
                [
                    'month' => [$current_month],
                    'day' => [$non_current_day],
                    'hour' => '0:00-23:59',
                ],
            ],
            async: false,
        );
        $inactive_sched->create();

        # Create an entry that allows all users but a single IP
        $entry = new RESTAPIAccessListEntry();
        $entry->replace_all([['type' => 'allow', 'network' => '0.0.0.0/0', 'sched' => $active_sched->name->value]]);

        # Ensure the entry is allowed when the schedule is active
        $this->assert_is_true($entry->is_allowed_by_acl('1.2.3.4', 'admin'));

        # Ensure the entry is denied when the schedule is inactive
        $entry->replace_all([['type' => 'allow', 'network' => '0.0.0.0/0', 'sched' => $inactive_sched->name->value]]);
        $this->assert_is_false($entry->is_allowed_by_acl('1.2.3.4', 'admin'));

        # Clean up
        $active_sched->delete();
        $inactive_sched->delete();
    }
}
