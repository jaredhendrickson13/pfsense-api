<?php

namespace API\Validators;

require_once("api/auto_loader.inc");

use API;
use API\Models\FirewallAlias;
use API\Models\NetworkInterface;
use API\Responses\ValidationError;

/**
 * Validates that a given string can be used as valid filter names. Filter names are any object name that can be used
 * as a filter address value. Interface and alias names are examples of filter names.
 */
class FilterNameValidator extends API\Core\Validator {
    /**
     * Checks if a given name is suitable to used as a filter name. Filter names are any name that can be used by the
     * firewall filter such as interfaces and aliases.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Responses\ValidationError When the value is not a valid IPv4 address.
     */
    public function validate(mixed $value, string $field_name = "") {
        global $pf_reserved_keywords, $reserved_table_names;

        # Throw an exception if this name is reserved
        if (in_array($value, array_merge($pf_reserved_keywords, $reserved_table_names))) {
            throw new ValidationError(
                message: "Field '$field_name' cannot be '$value' because it is a name reserved by the system.",
                response_id: "FILTER_NAME_VALIDATOR_RESERVED_NAME_USED"
            );
        }

        # Throw an exception if this name starts with `pkg_`
        if (str_starts_with($value, "pkg_")) {
            throw new ValidationError(
                message: "Field '$field_name' cannot be '$value' because it starts with 'pkg_'.",
                response_id: "FILTER_NAME_VALIDATOR_CANNOT_START_WITH_PKG"
            );
        }

        # Throw an exception if this name is entirely numerical
        if (is_numeric($value)) {
            throw new ValidationError(
                message: "Field '$field_name' cannot be '$value' because it is entirely numerical.",
                response_id: "FILTER_NAME_VALIDATOR_CANNOT_BE_NUMERICAL"
            );
        }

        # Throw an exception if any existing NetworkInterface object is using this name
        if (NetworkInterface::read_all()->query(["id__except" => $this->model_context->id, "id" => $value])->exists()) {
            throw new ValidationError(
                message: "Field '$field_name' cannot be '$value' because it is in use by an existing interface.",
                response_id: "FILTER_NAME_VALIDATOR_NAME_IN_USE_BY_INTERFACE"
            );
        }

        # Throw an exception if any existing FirewallAlias object is using this name
        if (FirewallAlias::read_all()->query(["id__except" => $this->model_context->id, "id" => $value])->exists()) {
            throw new ValidationError(
                message: "Field '$field_name' cannot be '$value' because it is in use by an existing interface.",
                response_id: "FILTER_NAME_VALIDATOR_NAME_IN_USE_BY_ALIAS"
            );
        }

        # TODO: Change this to use a ModelSet query when an interface group model is developed
        # Throw an exception if value is an existing InterfaceGroup
        foreach (config_get_path('ifgroups/ifgroupentry', []) as $id => $if_group_ent) {
            if ($if_group_ent["ifname"] === $value and $id !== $this->model_context->id) {
                throw new ValidationError(
                    message: "Field '$field_name' cannot be '$value' because it is in use by an existing interface.",
                    response_id: "FILTER_NAME_VALIDATOR_NAME_IN_USE_BY_INTERFACE"
                );
            }
        }
    }
}
