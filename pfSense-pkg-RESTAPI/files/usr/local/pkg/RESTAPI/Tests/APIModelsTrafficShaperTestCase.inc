<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\TrafficShaper;

class APIModelsTrafficShaperTestCase extends TestCase
{
    /**
     * Checks that the `bandwidth` field cannot exceed 100 if `bandwidthtype` is `%`
     */
    public function test_bandwidth_cannot_exceed_100_with_percent_bandwidth_type(): void
    {
        $this->assert_throws_response(
            response_id: 'TRAFFIC_SHAPER_BANDWIDTH_EXCEED_MAXIMUM_PERCENTAGE',
            code: 400,
            callable: function () {
                $shaper = new TrafficShaper();
                $shaper->bandwidthtype->value = '%';
                $shaper->validate_bandwidth(bandwidth: 101);
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $shaper = new TrafficShaper();
                $shaper->bandwidthtype->value = '%';
                $shaper->validate_bandwidth(bandwidth: 100);
                $shaper->validate_bandwidth(bandwidth: 99);
                $shaper->validate_bandwidth(bandwidth: 1);
            },
        );
    }

    /**
     * Checks that the get_bandwidth_bits() method correctly obtains the TrafficShapers bandwith amount in bits OR
     * obtains the percentage amount if `bandwidthtype` is `%`.
     */
    public function test_get_bandwidth_bits(): void
    {
        # Create a traffic shaper to test with
        $shaper = new TrafficShaper();
        $shaper->bandwidth->value = 1;

        # Ensure the output get_bandwidth_bits is the total number of bits corresponding to the bandwidth type
        $shaper->bandwidthtype->value = 'Kb';
        $this->assert_equals($shaper->get_bandwidth_bits(), 1024);
        $shaper->bandwidthtype->value = 'Mb';
        $this->assert_equals($shaper->get_bandwidth_bits(), 1048576);

        # Ensure the % bandwidth type just returns the 1-100 percent value
        $shaper->bandwidthtype->value = '%';
        $this->assert_equals($shaper->get_bandwidth_bits(), 1);
        $shaper->bandwidth->value = 100;
        $this->assert_equals($shaper->get_bandwidth_bits(), 100);
        $shaper->bandwidth->value = 55;
        $this->assert_equals($shaper->get_bandwidth_bits(), 55);
    }

    /**
     * Checks that HFSC traffic shapers are correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_hfsc_shaper_found_in_pfctl(): void
    {
        # Create a HFSC shaper to test with
        $shaper = new TrafficShaper(
            data: [
                'enabled' => true,
                'interface' => 'lan',
                'scheduler' => 'HFSC',
                'bandwidthtype' => 'Mb',
                'bandwidth' => 100,
                'qlimit' => 100,
                'tbrconfig' => 10000,
                'queue' => [
                    [
                        'enabled' => true,
                        'name' => 'test',
                        'priority' => 1,
                        'qlimit' => 50,
                        'description' => null,
                        'default' => true,
                        'bandwidthtype' => 'Mb',
                        'bandwidth' => 100,
                    ],
                ],
            ],
            async: false,
        );
        $shaper->create(apply: true);

        # Ensure the traffic shaper is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue');
        $if = $shaper->interface->get_related_model()->if->value;
        $bw = $shaper->bandwidth->value . $shaper->bandwidthtype->value;
        $descr = $shaper->queue->value[0]['description'];
        $this->assert_str_contains($pfctl->output, "queue root_$if on $if bandwidth $bw priority 0 $descr");

        # Delete the shaper and ensure it is no longer found in pfctl
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Checks that CBQ traffic shapers are correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_cbq_shaper_found_in_pfctl(): void
    {
        # Create a CBQ shaper to test with
        $shaper = new TrafficShaper(
            data: [
                'enabled' => true,
                'interface' => 'lan',
                'scheduler' => 'CBQ',
                'bandwidthtype' => 'Mb',
                'bandwidth' => 100,
                'qlimit' => 100,
                'tbrconfig' => 10000,
                'queue' => [
                    [
                        'enabled' => true,
                        'name' => 'test',
                        'priority' => 1,
                        'qlimit' => 50,
                        'description' => null,
                        'default' => true,
                        'bandwidthtype' => 'Mb',
                        'bandwidth' => 100,
                    ],
                ],
            ],
            async: false,
        );
        $shaper->create(apply: true);

        # Ensure the traffic shaper is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue');
        $if = $shaper->interface->get_related_model()->if->value;
        $bw = $shaper->bandwidth->value . $shaper->bandwidthtype->value;
        $qlimit = $shaper->qlimit->value;
        $this->assert_str_contains(
            $pfctl->output,
            "queue root_$if on $if bandwidth $bw priority 0 qlimit $qlimit cbq( wrr root )",
        );

        # Update the shaper an ensure the updated values are also updated in pfctl
        $shaper->bandwidthtype->value = 'Gb';
        $shaper->bandwidth->value = 1;
        $shaper->qlimit->value = 45;
        $shaper->update(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $bw = $shaper->bandwidth->value . $shaper->bandwidthtype->value;
        $qlimit = $shaper->qlimit->value;
        $this->assert_str_contains(
            $pfctl->output,
            "queue root_$if on $if bandwidth $bw priority 0 qlimit $qlimit cbq( wrr root )",
        );

        # Delete the shaper and ensure it is no longer found in pfctl
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Checks that FAIRQ traffic shapers are correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_fairq_shaper_found_in_pfctl(): void
    {
        # Create a FAIRQ shaper to test with
        $shaper = new TrafficShaper(
            data: [
                'enabled' => true,
                'interface' => 'lan',
                'scheduler' => 'FAIRQ',
                'bandwidthtype' => 'Mb',
                'bandwidth' => 100,
                'qlimit' => 100,
                'tbrconfig' => 10000,
                'queue' => [
                    [
                        'enabled' => true,
                        'name' => 'test',
                        'priority' => 1,
                        'qlimit' => 50,
                        'description' => null,
                        'default' => true,
                        'bandwidthtype' => 'Mb',
                        'bandwidth' => 100,
                    ],
                ],
            ],
            async: false,
        );
        $shaper->create(apply: true);

        # Ensure the traffic shaper is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue');
        $if = $shaper->interface->get_related_model()->if->value;
        $bw = $shaper->bandwidth->value . $shaper->bandwidthtype->value;
        $qlimit = $shaper->qlimit->value;
        $this->assert_str_contains($pfctl->output, "queue root_$if on $if bandwidth $bw priority 0 qlimit $qlimit");

        # Update the shaper an ensure the updated values are also updated in pfctl
        $shaper->bandwidthtype->value = 'Gb';
        $shaper->bandwidth->value = 1;
        $shaper->qlimit->value = 45;
        $shaper->update(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $bw = $shaper->bandwidth->value . $shaper->bandwidthtype->value;
        $qlimit = $shaper->qlimit->value;
        $this->assert_str_contains($pfctl->output, "queue root_$if on $if bandwidth $bw priority 0 qlimit $qlimit");

        # Delete the shaper and ensure it is no longer found in pfctl
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Checks that CODELQ traffic shapers are correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_codelq_shaper_found_in_pfctl(): void
    {
        # Create a CODELQ shaper to test with
        $shaper = new TrafficShaper(
            data: [
                'enabled' => true,
                'interface' => 'lan',
                'scheduler' => 'CODELQ',
                'bandwidthtype' => 'Mb',
                'bandwidth' => 100,
                'tbrconfig' => 10000,
            ],
            async: false,
        );
        $shaper->create(apply: true);

        # Ensure the shaper is seen in pfctl
        $pfctl = new Command('pfctl -s queue');
        $if = $shaper->interface->get_related_model()->if->value;
        $bw = $shaper->bandwidth->value . $shaper->bandwidthtype->value;
        $tbr = $shaper->tbrconfig->value;
        $this->assert_str_contains(
            $pfctl->output,
            "altq on $if codel( target 5 interval 100 ) bandwidth $bw tbrsize $tbr",
        );

        # Update the shaper with new values and ensure they are updated in pfctl
        $shaper->bandwidthtype->value = 'Gb';
        $shaper->bandwidth->value = 2;
        $shaper->tbrconfig->value = 11500;
        $shaper->update(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $bw = $shaper->bandwidth->value . $shaper->bandwidthtype->value;
        $tbr = $shaper->tbrconfig->value;
        $this->assert_str_contains(
            $pfctl->output,
            "altq on $if codel( target 5 interval 100 ) bandwidth $bw tbrsize $tbr",
        );

        # Delete the shaper and ensure it is no longer found in pfctl
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }

    /**
     * Checks that PRIQ traffic shapers are correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_priq_shaper_found_in_pfctl(): void
    {
        # Create a PRIQ shaper to test with
        $shaper = new TrafficShaper(
            data: [
                'enabled' => true,
                'interface' => 'lan',
                'scheduler' => 'PRIQ',
                'bandwidthtype' => 'Mb',
                'bandwidth' => 100,
                'qlimit' => 100,
                'tbrconfig' => 10000,
                'queue' => [
                    [
                        'enabled' => true,
                        'name' => 'test',
                        'priority' => 1,
                        'qlimit' => 55,
                        'description' => null,
                        'default' => true,
                    ],
                ],
            ],
            async: false,
        );
        $shaper->create(apply: true);

        # Ensure the traffic shaper is correctly shown in pfctl
        $pfctl = new Command('pfctl -s queue');
        $if = $shaper->interface->get_related_model()->if->value;
        $qlimit = $shaper->queue->value[0]['qlimit'];
        $this->assert_str_contains($pfctl->output, "queue test on $if qlimit $qlimit priq( default )");

        # Update the shaper an ensure the updated values are also updated in pfctl
        $shaper->queue->value[0]['qlimit'] = 45;
        $shaper->update(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $qlimit = $shaper->queue->value[0]['qlimit'];
        $this->assert_str_contains($pfctl->output, "queue test on $if qlimit $qlimit priq( default )");

        # Delete the shaper and ensure it is no longer found in pfctl
        $shaper->delete(apply: true);
        $pfctl = new Command('pfctl -s queue');
        $this->assert_str_contains($pfctl->output, 'No queue in use');
    }
}
