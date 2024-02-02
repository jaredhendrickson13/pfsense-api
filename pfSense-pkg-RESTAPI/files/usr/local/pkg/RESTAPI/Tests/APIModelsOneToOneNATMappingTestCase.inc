<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\OneToOneNATMapping;

class APIModelsOneToOneNATMappingTestCase extends TestCase
{
    /**
     * Checks that a ValidationError is thrown when the `external` field is assigned an IP address that is not of the
     * IP family specified by the `ipprotocol` field.
     */
    public function test_validate_external(): void
    {
        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_EXTERNAL_IP_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet',
                    external: '1234::1',
                    source: '1.2.3.4',
                    destination: '4.3.2.1',
                );
                $one_to_one->validate();
            },
        );

        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_EXTERNAL_IP_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet6',
                    external: '1.2.3.4',
                    source: '1234::1',
                    destination: '4321::1',
                );
                $one_to_one->validate();
            },
        );
    }

    /**
     * Checks that a ValidationError is thrown when the `source` field is assigned an IP or subnet that is not of the
     * IP family specified by the `ipprotocol` field.
     */
    public function test_validate_source(): void
    {
        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_SOURCE_IP_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet',
                    external: '1.2.3.4',
                    source: '1234::1',
                    destination: '4.3.2.1',
                );
                $one_to_one->validate();
            },
        );

        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_SOURCE_IP_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet6',
                    external: '1234::1',
                    source: '1.2.3.4',
                    destination: '4321::1',
                );
                $one_to_one->validate();
            },
        );

        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_SOURCE_SUBNET_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet',
                    external: '1.2.3.4',
                    source: '1234::1/128',
                    destination: '4.3.2.1',
                );
                $one_to_one->validate();
            },
        );

        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_SOURCE_SUBNET_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet6',
                    external: '1234::1',
                    source: '1.2.3.4/32',
                    destination: '4321::1',
                );
                $one_to_one->validate();
            },
        );
    }

    /**
     * Checks that a ValidationError is thrown when the `destination` field is assigned an IP or subnet that is not of the
     * IP family specified by the `ipprotocol` field.
     */
    public function test_validate_destination(): void
    {
        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_DESTINATION_IP_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet',
                    external: '1.2.3.4',
                    destination: '1234::1',
                    source: '4.3.2.1',
                );
                $one_to_one->validate();
            },
        );

        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_DESTINATION_IP_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet6',
                    external: '1234::1',
                    destination: '1.2.3.4',
                    source: '4321::1',
                );
                $one_to_one->validate();
            },
        );

        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_DESTINATION_SUBNET_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet',
                    external: '1.2.3.4',
                    destination: '1234::1/128',
                    source: '4.3.2.1',
                );
                $one_to_one->validate();
            },
        );

        $this->assert_throws_response(
            response_id: 'ONE_TO_ONE_NAT_MAPPING_DESTINATION_SUBNET_DOES_NOT_MATCH_IPPROTOCOL',
            code: 400,
            callable: function () {
                $one_to_one = new OneToOneNATMapping(
                    interface: 'wan',
                    ipprotocol: 'inet6',
                    external: '1234::1',
                    destination: '1.2.3.4/32',
                    source: '4321::1',
                );
                $one_to_one->validate();
            },
        );
    }

    /**
     * Checks that the NAT 1:1 mapping is correctly found in pfctl after creation, modification and deletion.
     */
    public function test_one_to_one_found_in_pfctl(): void
    {
        # Create a new 1:1 mapping to test with
        $one_to_one = new OneToOneNATMapping(
            interface: 'lan',
            ipprotocol: 'inet',
            external: '1.2.3.4',
            source: '192.168.1.50',
            destination: '192.168.2.50',
            natreflection: 'disable',
            async: false,
        );
        $one_to_one->create(apply: true);

        # Ensure the created NAT rule is found in pfctl
        $pfctl = new Command('pfctl -sn');
        $if = $one_to_one->interface->get_related_model()->if->value;
        $proto = $one_to_one->ipprotocol->value;
        $external = $one_to_one->external->value;
        $source = $one_to_one->source->value;
        $dest = $one_to_one->destination->value;
        $expected_line = "binat on $if $proto from $source to $dest -> $external";
        $this->assert_str_contains($pfctl->output, $expected_line);

        # Update the mapping with new values
        $one_to_one->from_representation(
            interface: 'wan',
            ipprotocol: 'inet',
            external: '4.3.2.1',
            source: 'wan:ip',
            destination: '192.168.3.50',
            natreflection: 'enable',
        );
        $one_to_one->update(apply: true);

        # Ensure the created NAT rule is found in pfctl
        $pfctl = new Command('pfctl -sn');
        $if = $one_to_one->interface->get_related_model()->if->value;
        $proto = $one_to_one->ipprotocol->value;
        $external = $one_to_one->external->value;
        $source = $one_to_one->interface->get_related_model()->get_current_ipv4();
        $dest = $one_to_one->destination->value;
        $expected_line = "binat on $if $proto from $source to $dest -> $external";
        $this->assert_str_contains($pfctl->output, $expected_line);

        # Ensure a NAT reflection rule is also found
        $rdr_expected_line = "rdr on em1 $proto from $dest to $external -> $source bitmask";
        $this->assert_str_contains($pfctl->output, $rdr_expected_line);

        # Delete and ensure the mapping is no longer found
        $one_to_one->delete(apply: true);
        $pfctl = new Command('pfctl -sn');
        $this->assert_str_does_not_contain($pfctl->output, 'binat');
    }
}
