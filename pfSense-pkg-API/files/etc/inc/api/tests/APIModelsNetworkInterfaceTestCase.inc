<?php

namespace API\Tests;

use API\Core\Model;
use API\Core\TestCase;
use API\Models\InterfaceVLAN;
use API\Models\NetworkInterface;

class APIModelsNetworkInterfaceTestCase extends TestCase
{
    /**
     * Checks to ensure the `from_internal_descr()` correctly populates empty interface
     * `descr` field values.
     */
    public function test_from_internal_descr() {
        # Create a NetworkInterface model to test with
        $test_model = new NetworkInterface();
        $test_model->get_next_id();

        # Ensure empty internal `descr` values are populate as the ID in all caps
        $this->assert_equals($test_model->from_internal_descr(null), strtoupper($test_model->id));

        # Ensure non-empty internal `descr` values retain their original value
        $this->assert_equals($test_model->from_internal_descr("test"), "test");
    }

    /**
     * Checks that the `descr' field value must be 22 characters or less when the `if` field is set
     * to an OpenVPN interface
     */
    public function test_descr_max_length_with_openvpn_if() {
        $this->assert_throws_response(
            response_id: "NETWORK_INTERFACE_DESCR_OPENVPN_MAX_LENGTH_EXCEEDED",
            code: 400,
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "ovpn1";

                # Run the `validate_descr()` method to trigger the thrown response
                $test_model->validate_descr("this descr is too long for an openvpn interface");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "ovpn1";

                # Ensure the validation succeeds when value is an acceptable length
                $test_model->validate_descr("length is ok");
            }
        );
    }

    /**
     * Checks that the `descr' field value must be 22 characters or less when the `if` field is set
     * to an IPsec interface
     */
    public function test_descr_max_length_with_ipsec_if() {
        $this->assert_throws_response(
            response_id: "NETWORK_INTERFACE_DESCR_IPSEC_MAX_LENGTH_EXCEEDED",
            code: 400,
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "ipsec1";

                # Run the `validate_descr()` method to trigger the thrown response
                $test_model->validate_descr("this descr is too long for an ipsec interface");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "ipsec1";

                # Ensure the validation succeeds when value is an acceptable length
                $test_model->validate_descr("length is ok");
            }
        );
    }

    /**
     * Checks that the `descr' field value must be 22 characters or less when the `if` field is set
     * to a GIF interface
     */
    public function test_descr_max_length_with_gif_if() {
        $this->assert_throws_response(
            response_id: "NETWORK_INTERFACE_DESCR_GIF_MAX_LENGTH_EXCEEDED",
            code: 400,
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "gif1";

                # Run the `validate_descr()` method to trigger the thrown response
                $test_model->validate_descr("this descr is too long for a gif interface");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "gif1";

                # Ensure the validation succeeds when value is an acceptable length
                $test_model->validate_descr("length is ok");
            }
        );
    }

    /**
     * Checks that the `descr' field value must be 22 characters or less when the `if` field is set
     * to a GRE interface
     */
    public function test_descr_max_length_with_gre_if() {
        $this->assert_throws_response(
            response_id: "NETWORK_INTERFACE_DESCR_GRE_MAX_LENGTH_EXCEEDED",
            code: 400,
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "gre1";

                # Run the `validate_descr()` method to trigger the thrown response
                $test_model->validate_descr("this descr is too long for a gre interface");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                # Create a NetworkInterface model to test with
                $test_model = new NetworkInterface();
                $test_model->if->value = "gre1";

                # Ensure the validation succeeds when value is an acceptable length
                $test_model->validate_descr("length is ok");
            }
        );
    }

    /**
     * Checks that a VLAN interface cannot set its MTU higher than its parent interface's MTU.
     */
    public function test_vlan_interface_cannot_have_higher_mtu_than_parent() {
        # Create a new interface VLAN
        $vlan = new InterfaceVLAN(
            representation_data: [
                "if" => "em2",
                "tag" => 2
            ]
        );
        $vlan->create();

        # Configure the MTU on the VLANs parent interface
        $parent_if = new NetworkInterface(
            representation_data: [
                "if" => "em2",
                "enable" => true,
                "descr" => "test_parent",
                "mtu" => 1450,
                "typev4" => "none",
                "typev6" => "none"
            ]
        );
        $parent_if->create(apply: true);

        # Ensure a validation error is thrown when trying to validate a VLAN interface with a higher MTU than its parent
        $this->assert_throws_response(
            response_id: "INTERFACE_WITH_MTU_HIGHER_THAN_PARENT_MTU",
            code: 400,
            callable: function () {
                # Try to configure the VLAN interface with an MTU higher than the parent's
                $vlan_if = new NetworkInterface(
                    representation_data: [
                        "if" => "em2.2",
                        "descr" => "test_vlan_if",
                        "mtu" => 1500,
                        "typev4" => "none",
                        "typev6" => "none"
                    ]
                );
                $vlan_if->validate();
            }
        );

        # Delete model objects used for testing
        $parent_if->delete();
        $vlan->delete();
    }

    /**
     * Checks that the `validate_media()` method properly validates available interface media types.
     */
    public function test_validate_media() {
        # Ensure an error is thrown if media is not a supported value
        $this->assert_throws_response(
            response_id: "NETWORK_INTERFACE_WITH_UNSUPPORTED_MEDIA_TYPE",
            code: 400,
            callable: function () {
                (new NetworkInterface())->validate_media("not a valid media type");
            }
        );

        # Ensure valid media types are accepted
        $this->assert_does_not_throw(
            callable: function () {
                (new NetworkInterface())->validate_media("autoselect");
            }
        );
    }

    /**
     * Checks that the `validate_mediaopt()` method properly validates available interface media options.
     */
    public function test_validate_mediaopt() {
        # Ensure an error is thrown if mediaopt is not a supported value
        $this->assert_throws_response(
            response_id: "NETWORK_INTERFACE_WITH_UNSUPPORTED_MEDIA_OPTION",
            code: 400,
            callable: function () {
                $if = new NetworkInterface();
                $if->media->value = "1000baseT";
                $if->validate_mediaopt("not a valid media option");
            }
        );

        # Ensure valid media options are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $if = new NetworkInterface();
                $if->media->value = "1000baseT";
                $if->validate_mediaopt("full-duplex");
            }
        );
    }

    /**
     * Checks that the `validate_blockbogons()` correctly determines if bogons can be blocked on the current system.
     */
    public function test_validate_blockbogons() {
        # Ensure no error is thrown when trying to block bogons with a sufficient table entries size limit
        $this->assert_does_not_throw(
            callable: function () {
                (new NetworkInterface())->validate_blockbogons(true);
            }
        );

        # Ensure an error is thrown when trying to block bogons with an insufficient table entries size limit
        Model::set_config(path: "system/maximumtableentries", value: g_get("minimumtableentries_bogonsv6") - 1);
        $this->assert_throws_response(
            response_id: "NETWORK_INTERFACE_BLOCKBOGONS_ENABLED_WITH_INSUFFICIENT_TABLE_ENTRY_LIMIT",
            code: 400,
            callable: function () {
                (new NetworkInterface())->validate_blockbogons(true);
            }
        );

        # Ensure no error is thrown with an insufficient table entries size limit, but IPv6 disabled
        Model::del_config("system/ipv6allow");
        $this->assert_does_not_throw(
            callable: function () {
                (new NetworkInterface())->validate_blockbogons(true);
            }
        );
    }
}