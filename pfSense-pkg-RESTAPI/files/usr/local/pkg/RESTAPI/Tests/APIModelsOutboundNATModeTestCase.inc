<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\OutboundNATMapping;
use RESTAPI\Models\OutboundNATMode;

class APIModelsOutboundNATModeTestCase extends TestCase {
    /**
     * Checks that the `automatic` outbound NAT mode auto-generates NAT rules, but does not included user defined
     * outbound NAT mappings. `hybrid` outbound NAT mode auto-generates NAT rules and includes user defined outbound
     * NAT mappings, `advanced` does not auto-generate NAT rules but includes user defined outbound NAT mappings,
     * and `disabled` does not auto-generate NAT rules and does not include user defined outbound NAT mappings
     */
    public function test_nat_modes() {
        # Create OutboundNATMode and OutboundNATMapping objects to test with
        $nat_mode = new OutboundNATMode(data: ['mode' => 'automatic'], async: false);
        $nat_map = new OutboundNATMapping(
            data: [
                'interface' => 'wan',
                'source' => 'any',
                'destination' => '127.5.0.1/24',
                'target' => 'wan:ip',
                'descr' => 'test outbound nat mode',
            ],
            async: false,
        );
        $nat_mode->update(apply: true);
        $nat_map->create(apply: true);

        # Ensure the automatic NAT rules are present and user defined rules are not
        $pfctl_rules = file_get_contents('/tmp/rules.debug');
        $this->assert_str_contains($pfctl_rules, "\$tonatsubnets");
        $this->assert_str_does_not_contain($pfctl_rules, $nat_map->descr->value);

        # Update the outbound NAT mode to hybrid and ensure both the auto generated and user defined rules are present
        $nat_mode->mode->value = 'hybrid';
        $nat_mode->update(apply: true);
        $pfctl_rules = file_get_contents('/tmp/rules.debug');
        $this->assert_str_contains($pfctl_rules, "\$tonatsubnets");
        $this->assert_str_contains($pfctl_rules, $nat_map->descr->value);

        # Update the outbound NAT mode to advanced and ensure only user defined rules are present
        $nat_mode->mode->value = 'advanced';
        $nat_mode->update(apply: true);
        $pfctl_rules = file_get_contents('/tmp/rules.debug');
        $this->assert_str_does_not_contain($pfctl_rules, "\$tonatsubnets");
        $this->assert_str_contains($pfctl_rules, $nat_map->descr->value);

        # Update the outbound NAT mode to disabled and ensure no nat rules are present
        $nat_mode->mode->value = 'disabled';
        $nat_mode->update(apply: true);
        $pfctl_rules = file_get_contents('/tmp/rules.debug');
        $this->assert_str_does_not_contain($pfctl_rules, "\$tonatsubnets");
        $this->assert_str_does_not_contain($pfctl_rules, $nat_map->descr->value);

        # Restore the NAT mode and delete the outbound NAT mapping
        $nat_mode->mode->value = 'automatic';
        $nat_mode->update(apply: true);
        $nat_map->delete(apply: true);
    }
}
