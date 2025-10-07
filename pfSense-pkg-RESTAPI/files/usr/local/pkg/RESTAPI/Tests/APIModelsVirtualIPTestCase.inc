<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\CARP;
use RESTAPI\Models\VirtualIP;

class APIModelsVirtualIPTestCase extends TestCase {
    /**
     * Checks that the virtual IP `type` cannot be `network` when `mode` is set to
     * `ipalias` or `carp`.
     */
    public function test_type_cannot_be_network_with_ipalias_or_carp(): void {
        $this->assert_throws_response(
            response_id: 'VIRTUALIP_MODE_DOES_NOT_SUPPORT_NETWORK_TYPE',
            code: 400,
            callable: function () {
                $test_model = new VirtualIP();
                $test_model->mode->value = 'carp';
                $test_model->validate_type('network');
            },
        );

        $this->assert_throws_response(
            response_id: 'VIRTUALIP_MODE_DOES_NOT_SUPPORT_NETWORK_TYPE',
            code: 400,
            callable: function () {
                $test_model = new VirtualIP();
                $test_model->mode->value = 'ipalias';
                $test_model->validate_type('network');
            },
        );
    }

    /**
     * Checks that the virtual IP `subnet_bits` is effectively limited to >=32 for IPv4 vips
     */
    public function test_ipv4_subnet_bits_is_limited(): void {
        $this->assert_throws_response(
            response_id: 'VIRTUALIP_IPV4_SUBNET_BITS_EXCEEDED',
            code: 400,
            callable: function () {
                $test_model = new VirtualIP(
                    data: [
                        'mode' => 'ipalias',
                        'interface' => 'wan',
                        'subnet' => '1.2.3.4',
                        'subnet_bits' => 33,
                    ],
                );
                $test_model->validate();
            },
        );

        $this->assert_does_not_throw(
            callable: function () {
                $test_model = new VirtualIP(
                    data: [
                        'mode' => 'ipalias',
                        'interface' => 'wan',
                        'subnet' => '1.2.3.4',
                        'subnet_bits' => 32,
                    ],
                );
                $test_model->validate();
            },
        );
    }

    /**
     * Checks that pending virtual IP changes are properly serialized before applying.
     */
    public function test_serialize_changes(): void {
        # Create a vip but don't apply it so we can check the serialized changes file
        $test_model_1 = new VirtualIP(
            data: [
                'mode' => 'ipalias',
                'interface' => 'wan',
                'subnet' => '1.2.3.4',
                'subnet_bits' => 24,
            ],
            async: false,
        );
        $test_model_1->create();

        # Ensure the file exists
        $this->assert_is_true(is_file('/tmp/.firewall_virtual_ip.apply'));

        # Apply the changes and ensure the serialized changes file is no longer present
        $test_model_1->apply();
        $this->assert_is_false(is_file('/tmp/.firewall_virtual_ip.apply'));

        $test_model_1->delete();
    }

    /**
     * Checks that ipalias virtual IPs are actually configured after applying.
     */
    public function test_ipalias_virtual_ip(): void {
        # Create a new ipalias virtual IP and ensure it is present in ifconfig afterward
        $test_model = new VirtualIP(
            data: [
                'mode' => 'ipalias',
                'interface' => 'wan',
                'subnet' => '1.2.3.4',
                'subnet_bits' => 24,
            ],
            async: false,
        );
        $test_model->create(apply: true);
        $ifconfig = new Command('ifconfig');
        $this->assert_str_contains($ifconfig->output, 'inet 1.2.3.4 netmask 0xffffff00 broadcast 1.2.3.255');

        # Update the IP alias vip and ensure the previous vip is no longer present and the new vip is present
        $test_model->subnet->value = '4.3.2.1';
        $test_model->subnet_bits->value = 25;
        $test_model->update(apply: true);
        $ifconfig = new Command('ifconfig');
        $this->assert_str_does_not_contain($ifconfig->output, 'inet 1.2.3.4 netmask 0xffffff00 broadcast 1.2.3.255');
        $this->assert_str_contains($ifconfig->output, 'inet 4.3.2.1 netmask 0xffffff80 broadcast 4.3.2.127');

        # Remove the vip and ensure it is no longer present in ifconfig
        $test_model->delete();
        $ifconfig = new Command('ifconfig');
        $this->assert_str_does_not_contain($ifconfig->output, 'inet 4.3.2.1 netmask 0xffffff80 broadcast 4.3.2.127');
    }

    /**
     * Checks that carp virtual IPs are actually configured after applying.
     */
    public function test_carp_virtual_ip(): void {
        # Create a new virtual IP and ensure it is present in ifconfig afterward
        $test_model = new VirtualIP(
            data: [
                'mode' => 'carp',
                'interface' => 'wan',
                'subnet' => '4.5.6.7',
                'subnet_bits' => 24,
                'vhid' => 7,
                'advbase' => 10,
                'advskew' => 11,
                'password' => 'testpasswd',
            ],
            async: false,
        );
        $test_model->create(apply: true);
        $ifconfig = new Command('ifconfig');
        $this->assert_str_contains($ifconfig->output, 'inet 4.5.6.7 netmask 0xffffff00 broadcast 4.5.6.255 vhid 7');
        $this->assert_str_contains($ifconfig->output, 'advbase 10 advskew 11');

        # Update the vip and ensure the previous vip is no longer present and the new vip is present
        $test_model->subnet->value = '7.6.5.4';
        $test_model->subnet_bits->value = 25;
        $test_model->vhid->value = 5;
        $test_model->advbase->value = 5;
        $test_model->advskew->value = 6;
        $test_model->update(apply: true);
        $ifconfig = new Command('ifconfig');
        $this->assert_str_does_not_contain(
            $ifconfig->output,
            'inet 4.5.6.7 netmask 0xffffff00 broadcast 4.5.6.255 vhid 7',
        );
        $this->assert_str_does_not_contain($ifconfig->output, 'advbase 10 advskew 11');
        $this->assert_str_contains($ifconfig->output, 'inet 7.6.5.4 netmask 0xffffff80 broadcast 7.6.5.127 vhid 5');
        $this->assert_str_contains($ifconfig->output, 'advbase 5 advskew 6');

        # Remove the vip and ensure it is no longer present in ifconfig
        $test_model->delete();
        $ifconfig = new Command('ifconfig');
        $this->assert_str_does_not_contain(
            $ifconfig->output,
            'inet 7.6.5.4 netmask 0xffffff80 broadcast 7.6.5.127 vhid 5',
        );
    }

    /**
     * Checks that proxyarp virtual IPs are actually configured after applying.
     */
    public function test_proxyarp_virtual_ip(): void {
        # Create a new proxyarp virtual IP and ensure it is present in ifconfig afterward
        $test_model = new VirtualIP(
            data: [
                'mode' => 'proxyarp',
                'interface' => 'wan',
                'subnet' => '1.2.3.4',
                'subnet_bits' => 32,
            ],
            async: false,
        );
        $test_model->create(apply: true);
        $choparp = new Command('/usr/bin/top -baHS 999 | grep choparp');
        $this->assert_str_contains($choparp->output, '1.2.3.4');

        # Update the IP alias vip and ensure the previous vip is no longer present and the new vip is present
        $test_model->subnet->value = '4.3.2.1';
        $test_model->update(apply: true);
        $choparp = new Command('/usr/bin/top -baHS 999 | grep choparp');
        $this->assert_str_does_not_contain($choparp->output, '1.2.3.4');
        $this->assert_str_contains($choparp->output, '4.3.2.1');

        # Remove the vip and ensure it is no longer present in ifconfig
        $test_model->delete();
        $choparp = new Command('/usr/bin/top -baHS 999 | grep choparp');
        $this->assert_str_does_not_contain($choparp->output, '4.3.2.1');
    }

    /**
     * Checks that the virtual IP `carp_status` is accurately represented.
     */
    public function test_carp_status(): void {
        # Create CARP status and CARP VIP objects to test with
        $carp_status = new CARP(async: false);
        $carp_vip = new VirtualIP(
            data: [
                'mode' => 'carp',
                'interface' => 'lan',
                'subnet' => '127.1.2.3',
                'subnet_bits' => 32,
                'password' => 'testpasswd',
                'vhid' => 5,
            ],
            async: false,
        );
        $carp_vip->create(apply: true);

        # Ensure the CARP VIP has assumed `master` status
        $carp_vip->from_internal();
        $this->assert_is_true(in_array($carp_vip->carp_status->value, ['master', 'backup', 'init']));

        # Disable CARP on the system and ensure the CARP virtual IP has assumed `disabled` status
        $carp_status->enable->value = false;
        $carp_status->update();
        $carp_vip->from_internal();
        $this->assert_equals($carp_vip->carp_status->value, 'disabled');

        # Re-enable CARP and delete the virtual IP
        $carp_status->enable->value = true;
        $carp_status->update();
        $carp_vip->delete(apply: true);
    }

    public function test_carp_vhid_must_be_unique_per_interface(): void {
        # Create a virtual IP to test with
        $vip = new VirtualIP(
            mode: 'carp',
            interface: 'lan',
            subnet: '127.1.2.3',
            subnet_bits: 32,
            password: 'testpasswd',
            vhid: 5,
        );
        $vip->create();

        # Ensure we can update the existing VIP with the same VHID without issue
        $this->assert_does_not_throw(
            callable: function () use ($vip) {
                $vip->validate_vhid(vhid: 5);
            },
        );

        # Ensure we cannot create a new VIP with the same VHID on the same interface
        $this->assert_throws_response(
            response_id: 'VIRTUALIP_VHID_ALREADY_IN_USE',
            code: 409,
            callable: function () {
                $vip = new VirtualIP(mode: 'carp', interface: 'lan');
                $vip->validate_vhid(vhid: 5);
            },
        );

        # Ensure we can create a new VIP with the same VHID on a different interface
        $this->assert_does_not_throw(
            callable: function () {
                $vip = new VirtualIP(mode: 'carp', interface: 'wan');
                $vip->validate_vhid(vhid: 5);
            },
        );

        # Clean up the VIP we created
        $vip->delete();
    }

    /**
     * Ensures we can create an IP alias virtual IP nested under a parent CARP virtual IP.
     */
    public function test_nested_vip_under_carp_vip(): void {
        # Create a CARP virtual IP to test with
        $carp_vip = new VirtualIP(
            interface: 'lan',
            mode: 'carp',
            subnet: '127.0.0.105',
            subnet_bits: 32,
            vhid: 105,
            password: 'test',
            async: false,
        );
        $carp_vip->create();

        # Create a new IP alias virtual IP that uses the CARP virtual IP as its interface
        $child_vip = new VirtualIP(
            interface: "_vip{$carp_vip->uniqid->value}",
            mode: 'ipalias',
            subnet: '127.0.0.106',
            subnet_bits: 32,
            async: false,
        );
        $child_vip->create(apply: true);

        # Ensure the parent interface shows both virtual IPs sharing the same vhid in ifconfig
        $iface = $carp_vip->interface->get_related_model()->if->value;
        $ifconfig = new Command("/sbin/ifconfig $iface");
        $this->assert_str_contains(
            $ifconfig->output,
            'inet 127.0.0.105 netmask 0xffffffff broadcast 127.0.0.105 vhid 105',
        );
        $this->assert_str_contains(
            $ifconfig->output,
            'inet 127.0.0.106 netmask 0xffffffff broadcast 127.0.0.106 vhid 105',
        );

        # Clean up the VIPs we created
        $child_vip->delete();
        $carp_vip->delete(apply: true);
    }
}
