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
    public function test_bandwidth_cannot_exceed_100_with_percent_bandwidth_type(): void {
        $this->assert_throws_response(
            response_id: "TRAFFIC_SHAPER_BANDWIDTH_EXCEED_MAXIMUM_PERCENTAGE",
            code: 400,
            callable: function () {
                $shaper = new TrafficShaper();
                $shaper->bandwidthtype->value = "%";
                $shaper->validate_bandwidth(bandwidth: 101);
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $shaper = new TrafficShaper();
                $shaper->bandwidthtype->value = "%";
                $shaper->validate_bandwidth(bandwidth: 100);
                $shaper->validate_bandwidth(bandwidth: 99);
                $shaper->validate_bandwidth(bandwidth: 1);
            }
        );
    }

    /**
     * Checks that HFSC traffic shapers are correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_hfsc_shaper_found_in_pfctl(): void {
        # Create a HFSC shaper to test with
        $shaper = new TrafficShaper(
            data: [
                "enabled" => true,
                "interface" => "lan",
                "scheduler" => "HFSC",
                "bandwidthtype" => "Mb",
                "bandwidth" => 100,
                "qlimit" => 100,
                "tbrconfig" => 10000,
                "queue" => [
                    [
                        "enabled" => true,
                        "name" => "test",
                        "priority" => 1,
                        "qlimit" => 50,
                        "description" => null,
                        "default" => true,
                        "bandwidthtype" => "Mb",
                        "bandwidth" => 100,
                    ]
                ]
            ],
            async: false
        );
        $shaper->create(apply: true);

        # Ensure the traffic shaper is correctly shown in pfctl
        $pfctl = new Command("pfctl -s queue");
        $if = $shaper->interface->get_related_model()->if->value;
        $bw = $shaper->bandwidth->value.$shaper->bandwidthtype->value;
        $descr = $shaper->queue->value[0]["description"];
        $this->assert_str_contains(
            $pfctl->output,
            "queue root_$if on $if bandwidth $bw priority 0 {$descr}"
        );

        # Delete the shaper and ensure it is no longer found in pfctl
        $shaper->delete(apply: true);
        $pfctl = new Command("pfctl -s queue");
        $this->assert_str_contains($pfctl->output, "No queue in use");
    }

    /**
     * Checks that CODELQ traffic shapers are correctly represented in pfctl after creation, modification and deletion.
     */
    public function test_codelq_shaper_found_in_pfctl(): void {
        # Create a CODELQ shaper to test with
        $shaper = new TrafficShaper(
            data: [
                "enabled" => true,
                "interface" => "lan",
                "scheduler" => "CODELQ",
                "bandwidthtype" => "Mb",
                "bandwidth" => 100,
                "tbrconfig" => 10000
            ],
            async: false
        );
        $shaper->create(apply: true);

        # Ensure the shaper is seen in pfctl
        $pfctl = new Command("pfctl -s queue");
        $if = $shaper->interface->get_related_model()->if->value;
        $bw = $shaper->bandwidth->value.$shaper->bandwidthtype->value;
        $tbr = $shaper->tbrconfig->value;
        $this->assert_str_contains(
            $pfctl->output,
            "altq on $if codel( target 5 interval 100 ) bandwidth $bw tbrsize $tbr"
        );

        # Update the shaper with new values and ensure they are updated in pfctl
        $shaper->bandwidthtype->value = "Gb";
        $shaper->bandwidth->value = 2;
        $shaper->tbrconfig->value = 11500;
        $shaper->update(apply: true);
        $pfctl = new Command("pfctl -s queue");
        $bw = $shaper->bandwidth->value.$shaper->bandwidthtype->value;
        $tbr = $shaper->tbrconfig->value;
        $this->assert_str_contains(
            $pfctl->output,
            "altq on $if codel( target 5 interval 100 ) bandwidth $bw tbrsize $tbr"
        );

        # Delete the shaper and ensure it is no longer found in pfctl
        $shaper->delete(apply: true);
        $pfctl = new Command("pfctl -s queue");
        $this->assert_str_contains($pfctl->output, "No queue in use");
    }
}