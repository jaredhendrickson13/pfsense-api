<?php

namespace API\Models;

use API\Core\Auth;
use API\Core\Model;
use API\Dispatchers\DNSResolverApplyDispatcher;
use API\Fields\IntegerField;
use API\Fields\StringField;
use API\Responses\ValidationError;
use API\Validators\IPAddressValidator;

/**
 * Defines access list entries for the DNS Resolver (Unbound) service.
 */
class DNSResolverAccessListNetwork extends Model
{
    public StringField $network;
    public IntegerField $mask;
    public StringField $description;

    /**
     * @inheritDoc
     */
    public function __construct(
        mixed $id = null,
        mixed $parent_id = null,
        mixed $data = [],
        Auth $client = null,
        bool $async = true
    )
    {
        # Set model attributes
        $this->parent_model_class = "DNSResolverAccessList";
        $this->config_path = "row";
        $this->subsystem = "unbound";
        $this->many = true;

        # Set model fields
        $this->network = new StringField(
            required: true,
            allow_empty: true,
            internal_name: "acl_network",
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The network address of this access list entry."
        );
        $this->mask = new IntegerField(
            required: true,
            minimum: 0,
            maximum: 128,
            help_text: "The subnet mask of this access list entry's network."
        );
        $this->description = new StringField(
            default: "",
            allow_empty: true,
            help_text: "A description for this access list entry."
        );

        # Construct the parent model object
        parent::__construct(id: $id, parent_id: $parent_id, data: $data, client: $client, async: $async);
    }

    /**
     * Adds extra validation to the `mask` field.
     * @param int $mask The incoming value to be validated.
     * @return int The validated value to be set.
     * @throws ValidationError When the `mask` value is greater than 32 but `network` is not an IPv4
     */
    public function validate_mask(int $mask) : int {
        # Do not allow value to be greater than 32 if `network` is not an IPv6
        if (!$this->network->has_label("is_ipaddrv6") and $mask > 32) {
            throw new ValidationError(
                message: "Field `mask` must be `32` or less when `network` is an IPv4 address.",
                response_id: "DNS_RESOLVER_ACCESS_LIST_NETWORK_MASK_EXCEEDS_MAXIMUM_IPV4"
            );
        }

        return $mask;
    }

    /**
     * Applies pending DNS Resolver changes.
     */
    public function apply()
    {
        (new DNSResolverApplyDispatcher(async: $this->async))->spawn_process();
    }
}
