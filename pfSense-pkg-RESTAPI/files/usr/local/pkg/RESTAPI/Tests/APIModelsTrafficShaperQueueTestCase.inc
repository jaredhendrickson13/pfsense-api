<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\TrafficShaper;
use RESTAPI\Models\TrafficShaperQueue;
use function RESTAPI\Core\Tools\bandwidth_to_bits;

class APIModelsTrafficShaperQueueTestCase extends TestCase {
    const PARENT_BASE_DATA = [
        'interface' => 'lan',
        'enabled' => true,
        'scheduler' => 'HFSC',
        'bandwidthtype' => 'Mb',
        'bandwidth' => 100,
    ];

    /**
     * Ensure TrafficShaperQueues cannot be created for parent TrafficShapers using the CODELQ scheduler.
     */
    public function test_no_child_queues_when_codelq_parent(): void {
        # Create a parent CODELQ TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->scheduler->value = 'CODELQ';
        $shaper->create();

        # Ensure a queue cannot be created because the parent uses the CODELQ scheduler
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_QUEUE_NOT_SUPPORTED_BY_PARENT_SCHEDULER',
            code: 422,
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id, data: []);
                $queue->validate_extra();
            },
        );

        # Delete the shaper
        $shaper->delete(apply: true);
    }

    /**
     * Checks that a child TrafficShaperQueue must use the % bandwidthtype if the parent TrafficShaper is using the
     * % bandwidthtype.
     */
    public function test_queue_bandwdith_type_must_match_parent_with_percent(): void {
        # Create a parent TrafficShaper and child TrafficShaperQueue
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->bandwidthtype->value = '%';
        $shaper->create();
        $queue = new TrafficShaperQueue(parent_id: $shaper->id);

        # Ensure an error is thrown if we try to set the queue's bandwidth type to something other than %
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_QUEUE_BANDWIDTHTYPE_MUST_MATCH_PARENT_WITH_PERCENT',
            code: 422,
            callable: function () use ($queue) {
                $queue->validate_bandwidthtype('Mb');
            },
        );

        # Delete the shaper
        $shaper->delete();
    }

    /**
     * Checks that the sum of all child TrafficShaperQueues bandwidth for a parent TrafficShaper cannot exceed the
     * parent's allowed bandwidth.
     */
    public function test_queues_cannot_exceed_parent_bandwidth_limits(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->bandwidth->value = 100;
        $shaper->bandwidthtype->value = 'Mb';
        $shaper->create();

        # Ensure an error is thrown if we try to give a child queue more bandwidth than the parent allows
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_EXCEEDS_PARENTS_BANDWIDTH_LIMITS',
            code: 422,
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->bandwidth->value = 101;
                $queue->bandwidthtype->value = $shaper->bandwidthtype->value;
                $queue->validated_bandwidth($queue->bandwidth->value);
            },
        );

        # Ensure no error is thrown if we try to set the queue's bandwidth type to something other than %
        $this->assert_does_not_throw(
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->bandwidth->value = 100;
                $queue->bandwidthtype->value = $shaper->bandwidthtype->value;
                $queue->validated_bandwidth($queue->bandwidth->value);
            },
        );

        # Delete the shaper
        $shaper->delete();
    }

    /**
     * Checks that the `priority` field value cannot be greater than 7 unless the parent TrafficShaper's `scheduler`
     * is set to `PRIQ`.
     */
    public function test_priority_maximum_condition(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->scheduler->value = 'HFSC';
        $shaper->create();

        # Ensure an error is thrown if the priority value is greater than 7.
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_QUEUE_PRIORITY_MAXIMUM_EXCEEDED_FOR_PARENT_SCHEDULER',
            code: 400,
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->validate_priority(8);
            },
        );

        # Update the parent to use PRIQ and ensure no error is thrown for values above 7
        # Note: the `priority` field's `maximum` property will still force a value for 15 or less.
        $shaper->scheduler->value = 'PRIQ';
        $shaper->update(apply: true);
        $this->assert_does_not_throw(
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->validate_priority(8);
            },
        );

        # Delete the shaper
        $shaper->delete();
    }

    /**
     * Checks that the `validate_upperlimit_m1()` method correctly enforces the `upperlimit_m1` field being a
     * bandwidth formatting string (i.e. 10Mb).
     */
    public function test_upperlimit_m1_must_be_bandwidth_formatted_string(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->scheduler->value = 'HFSC'; // Required to enable the upperlimit fields
        $shaper->create();

        # Ensure an error is thrown if upperlimit_m1 is not a bandwidth formatted string
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_QUEUE_UPPERLIMIT_M1_INVALID_BANDWIDTH_FORMAT',
            code: 400,
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->validate_upperlimit_m1('not a bandwidth string!');
            },
        );

        # Ensure valid bandwidth strings are accepted
        $this->assert_does_not_throw(
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->upperlimit_d->value = 30;
                $queue->validate_upperlimit_m1('10Mb');
                $queue->validate_upperlimit_m1('10b');
                $queue->validate_upperlimit_m1('10Gb');
                $queue->validate_upperlimit_m1('10Kb');
            },
        );

        # Delete the shaper
        $shaper->delete();
    }

    /**
     * Checks that the `validate_realtime_m1()` method correctly enforces the `realtime_m1` field being a
     * bandwidth formatting string (i.e. 10Mb).
     */
    public function test_realtime_m1_must_be_bandwidth_formatted_string(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->scheduler->value = 'HFSC'; // Required to enable the realtime fields
        $shaper->create();

        # Ensure an error is thrown if realtime_m1 is not a bandwidth formatted string
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_QUEUE_REALTIME_M1_INVALID_BANDWIDTH_FORMAT',
            code: 400,
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->validate_realtime_m1('not a bandwidth string!');
            },
        );

        # Ensure valid bandwidth strings are accepted
        $this->assert_does_not_throw(
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->realtime_d->value = 30;
                $queue->validate_realtime_m1('10Mb');
                $queue->validate_realtime_m1('10b');
                $queue->validate_realtime_m1('10Gb');
                $queue->validate_realtime_m1('10Kb');
            },
        );

        # Delete the shaper
        $shaper->delete();
    }

    /**
     * Checks that the `validate_linkshare_m1()` method correctly enforces the `linkshare_m1` field being a
     * bandwidth formatting string (i.e. 10Mb).
     */
    public function test_linkshare_m1_must_be_bandwidth_formatted_string(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->scheduler->value = 'HFSC'; // Required to enable the linkshare fields
        $shaper->create();

        # Ensure an error is thrown if linkshare_m1 is not a bandwidth formatted string
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_QUEUE_LINKSHARE_M1_INVALID_BANDWIDTH_FORMAT',
            code: 400,
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->validate_linkshare_m1('not a bandwidth string!');
            },
        );

        # Ensure valid bandwidth strings are accepted
        $this->assert_does_not_throw(
            callable: function () use ($shaper) {
                $queue = new TrafficShaperQueue(parent_id: $shaper->id);
                $queue->linkshare_d->value = 30;
                $queue->validate_linkshare_m1('10Mb');
                $queue->validate_linkshare_m1('10b');
                $queue->validate_linkshare_m1('10Gb');
                $queue->validate_linkshare_m1('10Kb');
            },
        );

        # Delete the shaper
        $shaper->delete();
    }

    /**
     * Checks that the `is_bandwidth_method()` correctly identifies bandwidth formatted strings (i.e. 10Mb, 1Gb, 100Kb)
     */
    public function test_is_bandwidth_string(): void {
        # Ensure valid bandwidth formatted strings are accepted
        $this->assert_is_true(TrafficShaperQueue::is_bandwidth_string('10Mb'));
        $this->assert_is_true(TrafficShaperQueue::is_bandwidth_string('1000Kb'));
        $this->assert_is_true(TrafficShaperQueue::is_bandwidth_string('100000b'));
        $this->assert_is_true(TrafficShaperQueue::is_bandwidth_string('1Gb'));

        # Ensure invalid bandwidth formatted strings are not accepted
        $this->assert_is_false(TrafficShaperQueue::is_bandwidth_string('10'));
        $this->assert_is_false(TrafficShaperQueue::is_bandwidth_string('Mb'));
        $this->assert_is_false(TrafficShaperQueue::is_bandwidth_string('invalid'));
        $this->assert_is_false(TrafficShaperQueue::is_bandwidth_string('Mb10'));
    }

    /**
     * Checks that the `get_bandwidth_bits()` method correctly determines the total amount of bandwidth in bits for
     * queues with exact bandwidth amounts (not percentages).
     */
    public function test_get_bandwidth_bits_exact(): void {
        # Create a parent TrafficShaper and child TrafficShaperQueue
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->create();
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'name' => 'testqueue',
                'qlimit' => 50,
                'default' => true,
            ],
        );

        # Ensure the get_bandwidth_bits() correctly obtains the exact amount of bits the bandwidth provides
        $queue->bandwidth->value = 10;
        $queue->bandwidthtype->value = 'Mb';
        $this->assert_equals(
            $queue->get_bandwidth_bits($queue),
            bandwidth_to_bits($queue->bandwidth->value, $queue->bandwidthtype->value),
        );

        $queue->bandwidth->value = 100;
        $queue->bandwidthtype->value = 'Kb';
        $this->assert_equals(
            $queue->get_bandwidth_bits($queue),
            bandwidth_to_bits($queue->bandwidth->value, $queue->bandwidthtype->value),
        );

        $queue->bandwidth->value = 2;
        $queue->bandwidthtype->value = 'Gb';
        $this->assert_equals(
            $queue->get_bandwidth_bits($queue),
            bandwidth_to_bits($queue->bandwidth->value, $queue->bandwidthtype->value),
        );

        $queue->bandwidth->value = 200000;
        $queue->bandwidthtype->value = 'b';
        $this->assert_equals(
            $queue->get_bandwidth_bits($queue),
            bandwidth_to_bits($queue->bandwidth->value, $queue->bandwidthtype->value),
        );

        # Delete this shaper
        $shaper->delete();
    }

    /**
     * Checks that the `get_bandwidth_bits()` method correctly determines the total amount of bandwidth in percentage
     * for queues with percentage bandwidth amounts when both the parent TrafficShaper and child TrafficShaperQueue
     * have % set as their `bandwidthtype`.
     */
    public function test_get_bandwidth_bits_percent(): void {
        # Create a parent TrafficShaper and child TrafficShaperQueue
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->bandwidth->value = 100;
        $shaper->bandwidthtype->value = '%';
        $shaper->create();
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'name' => 'testqueue',
                'qlimit' => 50,
                'default' => true,
                'bandwidthtype' => '%',
            ],
        );

        # Ensure the get_bandwidth_bits() obtains the percentage of the parent's bandwidth total assigned to the queue
        $queue->bandwidth->value = 10;
        $this->assert_equals($queue->get_bandwidth_bits($queue), $queue->bandwidth->value);

        $queue->bandwidth->value = 100;
        $this->assert_equals($queue->get_bandwidth_bits($queue), $queue->bandwidth->value);

        # Delete this shaper
        $shaper->delete();
    }

    /**
     * Checks that the `get_bandwidth_bits()` method correctly determines the total amount of bandwidth in bits a
     * child queue is allocated from its parent when the child uses a percentage and the parent uses an exact total
     * (i.e. a parent with a 100b bandwidth would give a child queue 30b of bandwidth when the child queues bandwidth
     * value is 30%)
     */
    public function test_get_bandwidth_bits_mixed(): void {
        # Create a parent TrafficShaper and child TrafficShaperQueue
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->bandwidth->value = 100;
        $shaper->bandwidthtype->value = 'b';
        $shaper->create();
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'name' => 'testqueue',
                'qlimit' => 50,
                'default' => true,
                'bandwidthtype' => '%',
            ],
        );

        # Ensure the get_bandwidth_bits() obtains the percentage of the parent's bandwidth total bits
        $queue->bandwidth->value = 10;
        $this->assert_equals($queue->get_bandwidth_bits($queue), 10);

        $queue->bandwidth->value = 50;
        $this->assert_equals($queue->get_bandwidth_bits($queue), 50);

        # Ensure this also works correctly when the parent uses a large bandwidth scale like Mb
        $shaper->bandwidthtype->value = 'Mb';
        $shaper->update();

        $queue->bandwidth->value = 30; // Percentage of parents total bits given to this queue
        $q_percent_in_bits = intval(($queue->bandwidth->value / 100) * $shaper->get_bandwidth_bits());
        $this->assert_equals($queue->get_bandwidth_bits($queue), $q_percent_in_bits);

        $queue->bandwidth->value = 24; // Percentage of parents total bits given to this queue
        $q_percent_in_bits = intval(($queue->bandwidth->value / 100) * $shaper->get_bandwidth_bits());
        $this->assert_equals($queue->get_bandwidth_bits($queue), $q_percent_in_bits);

        # Delete this shaper
        $shaper->delete();
    }

    /**
     * Checks that a TrafficShaperQueue cannot be saved if its current bandwidth value will force the sum of all the
     * parents child queues bandwidth to exceed the parents maximum bandwidth
     */
    public function test_is_bandwidth_exceeded(): void {
        # Create a parent TrafficShaper with a maximum of 100Mb bandwidth
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA);
        $shaper->bandwidth->value = 100;
        $shaper->bandwidthtype->value = 'Mb';
        $shaper->create();

        # Create a child queue that utilizes 50% of the parent's bandwidth
        $queue1 = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'name' => 'testqueue1',
                'qlimit' => 50,
                'default' => true,
                'bandwidth' => 50,
                'bandwidthtype' => '%',
            ],
        );
        $queue1->create();

        # Create another child queue to use to test exceeding the allowed bandwidth
        $queue2 = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'name' => 'testqueue1',
                'qlimit' => 50,
                'default' => true,
                'bandwidthtype' => 'Mb',
            ],
        );

        # Ensure $queue1's bandwidth can be updated without including its current bandwidth as utilized
        $queue1->bandwidth->value = 100;
        $this->assert_is_false($queue1->is_bandwidth_exceeded());

        # Since that parent allows 100Mb, and the first queue is using 50% of that, ensure $queue2 cannot use 51Mb
        $queue2->bandwidth->value = 51;
        $this->assert_is_true($queue2->is_bandwidth_exceeded());

        # Ensure 50Mb does not exceed the bandwidth total since that effectively utilizes the remaining 50%
        $queue2->bandwidth->value = 50;
        $this->assert_is_false($queue2->is_bandwidth_exceeded());

        # Ensure this also works with percentages. Since the other queue is using 50%, ensure this queue cant use 51%
        $queue2->bandwidth->value = 51;
        $queue2->bandwidthtype->value = '%';
        $this->assert_is_true($queue2->is_bandwidth_exceeded());

        # Ensure 50% is allowed since that effectively utilizes the remaining bandwidth
        $queue2->bandwidth->value = 50;
        $queue2->bandwidthtype->value = '%';
        $this->assert_is_false($queue2->is_bandwidth_exceeded());

        # Delete the shaper used for testing
        $shaper->delete();
    }

    /**
     * Checks that an HFSC TrafficShaperQueue is correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_hfsc_crud(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA, async: false);
        $shaper->create();

        # Create a child TrafficShaperQueue
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'enabled' => true,
                'name' => 'testqueue',
                'qlimit' => 60,
                'default' => true,
                'bandwidth' => 100,
                'bandwidthtype' => 'Mb',
                'upperlimit' => true,
                'upperlimit_m1' => '20Mb',
                'upperlimit_m2' => '10Mb',
                'upperlimit_d' => 2000,
                'realtime' => true,
                'realtime_m1' => '20Mb',
                'realtime_m2' => '10Mb',
                'realtime_d' => 2000,
                'linkshare' => true,
                'linkshare_m1' => '20Mb',
                'linkshare_m2' => '10Mb',
                'linkshare_d' => 2000,
            ],
            async: false,
        );
        $queue->create(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_hfsc_pfctl_queue_line($queue));

        # Update the child TrafficShaperQueue
        $queue->from_representation(
            data: [
                'enabled' => true,
                'name' => 'testqueue2',
                'qlimit' => 65,
                'default' => true,
                'bandwidth' => 50,
                'bandwidthtype' => 'Mb',
                'upperlimit' => true,
                'upperlimit_m1' => '25Mb',
                'upperlimit_m2' => '15Mb',
                'upperlimit_d' => 3350,
                'realtime' => true,
                'realtime_m1' => '23Mb',
                'realtime_m2' => '13Mb',
                'realtime_d' => 2210,
                'linkshare' => true,
                'linkshare_m1' => '22Mb',
                'linkshare_m2' => '12Mb',
                'linkshare_d' => 4450,
            ],
        );
        $queue->update(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_hfsc_pfctl_queue_line($queue));

        # Delete the shaper and ensure the queue is no longer present
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Obtains the expected pfctl HFSC queue line.
     * @param TrafficShaperQueue $queue The queue to obtain the line for.
     * @return string The expected pfctl HFSC queue line.
     */
    private function get_hfsc_pfctl_queue_line(TrafficShaperQueue $queue): string {
        $bw = $queue->bandwidth->value . $queue->bandwidthtype->value;
        $if = $queue->interface->get_related_model()->if->value;
        $name = $queue->name->value;
        $qlimit = $queue->qlimit->value;

        $rt = "realtime({$queue->realtime_m1->value} {$queue->realtime_d->value} {$queue->realtime_m2->value})";
        $ls = "linkshare({$queue->linkshare_m1->value} {$queue->linkshare_d->value} {$queue->linkshare_m2->value})";
        $ul = "upperlimit({$queue->upperlimit_m1->value} {$queue->upperlimit_d->value} {$queue->upperlimit_m2->value})";

        return "queue $name on $if bandwidth $bw qlimit $qlimit hfsc( default $rt $ls $ul )";
    }

    /**
     * Checks that an CBQ TrafficShaperQueue is correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_cbq_crud(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA, async: false);
        $shaper->scheduler->value = 'CBQ';
        $shaper->create();

        # Create a child TrafficShaperQueue
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'enabled' => true,
                'name' => 'testqueue',
                'qlimit' => 60,
                'default' => true,
                'bandwidth' => 100,
                'bandwidthtype' => 'Mb',
                'borrow' => true,
            ],
            async: false,
        );
        $queue->create(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_cbq_pfctl_queue_line($queue));

        # Update the child TrafficShaperQueue
        $queue->from_representation(
            data: [
                'enabled' => true,
                'name' => 'testqueue2',
                'qlimit' => 55,
                'default' => true,
                'bandwidth' => 50,
                'bandwidthtype' => 'Mb',
                'borrow' => false,
            ],
        );
        $queue->update(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_cbq_pfctl_queue_line($queue));

        # Delete the shaper and ensure the queue is no longer present
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Obtains the expected pfctl CBQ queue line.
     * @param TrafficShaperQueue $queue The queue to obtain the line for.
     * @return string The expected pfctl CBQ queue line.
     */
    private function get_cbq_pfctl_queue_line(TrafficShaperQueue $queue): string {
        $bw = $queue->bandwidth->value . $queue->bandwidthtype->value;
        $if = $queue->interface->get_related_model()->if->value;
        $name = $queue->name->value;
        $qlimit = $queue->qlimit->value;
        $borrow = $queue->borrow->value ? ' borrow' : '';

        return "queue $name on $if bandwidth $bw qlimit $qlimit cbq($borrow default )";
    }

    /**
     * Checks that an FAIRQ TrafficShaperQueue is correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_fairq_crud(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA, async: false);
        $shaper->scheduler->value = 'FAIRQ';
        $shaper->create();

        # Create a child TrafficShaperQueue
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'enabled' => true,
                'name' => 'testqueue',
                'qlimit' => 60,
                'default' => true,
                'bandwidth' => 100,
                'bandwidthtype' => 'Mb',
                'hogs' => 5,
            ],
            async: false,
        );
        $queue->create(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_fairq_pfctl_queue_line($queue));

        # Update the child TrafficShaperQueue
        $queue->from_representation(
            data: [
                'enabled' => true,
                'name' => 'testqueue2',
                'qlimit' => 44,
                'default' => true,
                'bandwidth' => 50,
                'bandwidthtype' => 'Mb',
                'hogs' => 15,
            ],
        );
        $queue->update(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_fairq_pfctl_queue_line($queue));

        # Delete the shaper and ensure the queue is no longer present
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Obtains the expected pfctl FAIRQ queue line.
     * @param TrafficShaperQueue $queue The queue to obtain the line for.
     * @return string The expected pfctl FAIRQ queue line.
     */
    private function get_fairq_pfctl_queue_line(TrafficShaperQueue $queue): string {
        $bw = $queue->bandwidth->value . $queue->bandwidthtype->value;
        $if = $queue->interface->get_related_model()->if->value;
        $name = $queue->name->value;
        $qlimit = $queue->qlimit->value;

        return "queue $name on $if bandwidth $bw qlimit $qlimit fairq( default )";
    }

    /**
     * Checks that an PRIQ TrafficShaperQueue is correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_priq_crud(): void {
        # Create a parent TrafficShaper
        $shaper = new TrafficShaper(data: self::PARENT_BASE_DATA, async: false);
        $shaper->scheduler->value = 'PRIQ';
        $shaper->create();

        # Create a child TrafficShaperQueue
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: [
                'enabled' => true,
                'name' => 'testqueue',
                'qlimit' => 60,
                'default' => true,
                'priority' => 2,
            ],
            async: false,
        );
        $queue->create(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_priq_pfctl_queue_line($queue));

        # Update the child TrafficShaperQueue
        $queue->from_representation(
            data: [
                'enabled' => true,
                'name' => 'testqueue2',
                'qlimit' => 44,
                'default' => true,
                'priority' => 4,
            ],
        );
        $queue->update(apply: true);

        # Ensure the queue is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue', trim_whitespace: true);
        $this->assert_str_contains($pfctl->output, $this->get_priq_pfctl_queue_line($queue));

        # Delete the shaper and ensure the queue is no longer present
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Obtains the expected pfctl PRIQ queue line.
     * @param TrafficShaperQueue $queue The queue to obtain the line for.
     * @return string The expected pfctl PRIQ queue line.
     */
    private function get_priq_pfctl_queue_line(TrafficShaperQueue $queue): string {
        $if = $queue->interface->get_related_model()->if->value;
        $name = $queue->name->value;
        $qlimit = $queue->qlimit->value;
        $priority = $queue->priority->value;

        return "queue $name on $if priority $priority qlimit $qlimit priq( default )";
    }
}
