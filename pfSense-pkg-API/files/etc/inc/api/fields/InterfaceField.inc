<?php

namespace API\Fields;

require_once("api/auto_loader.inc");
require_once("api/fields/StringField.inc");

use API;

/**
 * Defines an InterfaceField class that extends the core Field method. This Field adds validation and representation for
 * fields that accept interface IDs and/or names as input.
 */
class InterfaceField extends API\Fields\StringField {
    public bool $allow_interface_groups;
    public bool $allow_ipsec_interface;
    public bool $allow_openvpn_interface;
    public bool $allow_carp_interface;
    private string $represented_as;

    /**
     * Defines the StringField object and sets its options.
     * @param bool $required If `true`, this field is required to have a value at all times.
     * @param bool $unique If `true`, this field must be unique from all other parent model objects. Enabling this
     * option requires the Model $context to be set AND the Model $context must have a `config_path` set.
     * @param mixed|null $default Assign a default string value to assign this Field if no value is present.
     * @param bool $allow_interface_groups If `true`, all existing interface groups will be included as valid choices.
     * @param bool $allow_ipsec_interface If `true`, IPsec's `enc0` interface will included as a valid choice.
     * @param bool $allow_openvpn_interface If `true`, the `openvpn` interface will be included as valid choice.
     * @param bool $allow_carp_interface If `true`, all CARP unique IDs will be included as valid choices.
     * @param bool $allow_empty If `true`, empty strings will be allowed by this field.
     * @param bool $allow_null If `true`, null values will be allowed by this field.
     * @param bool $read_only If `true`, this field can only read its value and cannot write its value to config.
     * @param bool $write_only Set to `true` to make this field write-only. This will prevent the field's current value
     * from being displayed in the representation data. This is ideal for potentially sensitive fields like passwords,
     * keys, and hashes.
     * @param bool $representation_only Set to `true` to make this field only present in its representation form. This
     * effectively prevents the Field from being converted to an internal value which is saved to the pfSense config.
     * This should only be used for fields that do not relate directly to a configuration value.
     * @param bool $editable Set to `false` to prevent this field's value from being changed after its initial creation.
     * @param bool $many If `true`, the value must be an array of many strings.
     * @param int $many_minimum When $many is set to `true`, this sets the minimum number of array entries required.
     * @param int $many_maximum When $many is set to `true`, this sets the maximum number of array entries allowed.
     * @param string $delimiter Assign the delimiter used to join the value array back to its internal string form when
     * $many is enabled.
     * @param string $verbose_name The detailed name for this Field. This name will be used in non-programmatic areas
     * like web pages and help text. This Field will default to property name assigned to the parent Model with
     * underscores converted to spaces.
     * @param string $verbose_name_plural The plural form of $verbose_name. This defaults to $verbose_name with `s`
     * suffixed or `es` suffixes to strings already ending with `s`.
     * @param string $internal_name Assign a different field name to use when referring to the internal field as it's
     * stored in the pfSense configuration.
     * @param string $internal_namespace Sets the namespace this field belongs to internally. This can be used to nest
     * the fields internal value under a specific namespace as an associative array. This only applies to the internal
     * value, not the representation value.
     * @param array $referenced_by An array that specifies other Models and Field's that reference this Field's parent
     * Model using this Field's value. This will prevent the parent Model object from being deleted while it is actively
     * referenced by another Model object. The array key must be the name of the Model class that references this Field,
     * and the value must be a Field within that Model. The framework will automatically search for any existing Model
     * objects that have the referenced Field assigned a value that matches this Field's value.
     * @param array $conditions An array of conditions the field must meet to be included. This allows you to specify
     * conditions of other fields within the parent Model context. For example, if the parent Model context has two
     * fields, one field named `type` and the other being this field; and you only want this field to be included if
     * `type` is equal to `type1`, you could assign ["type" => "type1"] to this parameter.
     * @param array $validators An array of Validator objects to run against this field.
     * @param string $help_text Set a description for this field. This description will be used in API documentation.
     */
    public function __construct(
        bool $required = false,
        bool $unique = false,
        mixed $default = null,
        bool $allow_interface_groups = false,
        bool $allow_ipsec_interface = false,
        bool $allow_openvpn_interface = false,
        bool $allow_carp_interface = false,
        bool $allow_empty = false,
        bool $allow_null = false,
        bool $read_only = false,
        bool $write_only = false,
        bool $representation_only = false,
        bool $editable = true,
        bool $many = false,
        int $many_minimum = 1,
        int $many_maximum = 128,
        string $delimiter = ",",
        string $verbose_name = "",
        string $verbose_name_plural = "",
        string $internal_name = "",
        string $internal_namespace = "",
        array $referenced_by = [],
        array $conditions = [],
        array $validators = [],
        string $help_text = ""
    )
    {
        # Assign properties unique to this Field
        $this->allow_interface_groups = $allow_interface_groups;
        $this->allow_ipsec_interface = $allow_ipsec_interface;
        $this->allow_openvpn_interface = $allow_openvpn_interface;
        $this->allow_carp_interface = $allow_carp_interface;
        $this->represented_as = API\Models\APISettings::get_api_config()["config"]["represent_interfaces_as"];

        # Construct the parent Field object
        parent::__construct(
            required: $required,
            unique: $unique,
            default: $default,
            choices: $this->get_interface_choices(),
            allow_empty: $allow_empty,
            allow_null: $allow_null,
            editable: $editable,
            read_only: $read_only,
            write_only: $write_only,
            representation_only: $representation_only,
            many: $many,
            many_minimum: $many_minimum,
            many_maximum: $many_maximum,
            delimiter: $delimiter,
            verbose_name: $verbose_name,
            verbose_name_plural: $verbose_name_plural,
            internal_name: $internal_name,
            internal_namespace: $internal_namespace,
            referenced_by: $referenced_by,
            conditions: $conditions,
            validators: $validators,
            help_text: $help_text
        );
    }

    /**
     * Converts the field value to its representation form from it's internal pfSense configuration value.
     * @param mixed $internal_value The internal value from the pfSense configuration.
     * @return string The field value in its representation form.
     */
    protected function _from_internal(mixed $internal_value) : string {
        # Get all current interfaces
        $interfaces = API\Core\Model::get_config("interfaces", []);

        # If the value is an existing interface ID, return the representation value for that interface
        if (array_key_exists($internal_value, $interfaces)) {
            switch ($this->represented_as) {
                case "internal":
                    return $internal_value;
                case "description":
                    return $interfaces[$internal_value]["descr"] ?: strtoupper($internal_value);
                case "physical":
                    return $interfaces[$internal_value]["if"];
            }
        }

        return strval($internal_value);
    }

    /**
     * Converts the field's representation value into it's internal pfSense value.
     */
    protected function _to_internal(mixed $representation_value): string {
        # Loop through current interfaces and find a match for this value.
        $interfaces = API\Core\Model::get_config("interfaces", []);
        foreach ($interfaces as $id => $interface) {
            # In case this interface doesn't have a descriptive name, assume the default uppercase ID name
            $interface["descr"] = $interface["descr"] ?: strtoupper($id);
            if ($this->represented_as === "internal" and $representation_value === $id) {
                return $id;
            }
            if ($this->represented_as === "description" and $representation_value === $interface["descr"]) {
                return $id;
            }
            if ($this->represented_as === "physical" and $representation_value === $interface["if"]) {
                return $id;
            }
        }

        return $representation_value;
    }

    /**
     * Enumerates a list of every interface choice available to this Field.
     * @return array A list of every interface option available to this Field.
     */
    private function get_interface_choices() : array {
        # Variables
        $choices = [];
        $interfaces = API\Core\Model::get_config("interfaces", []);
        $interface_groups = API\Core\Model::get_config("ifgroups/ifgroupentry", []);
        $virtual_ips = API\Core\Model::get_config("virtualip/vip", []);

        # Loop through each configured interface and check for a match
        foreach ($interfaces as $id => $interface) {
            # In case this interface doesn't have a descriptive name, assume the default uppercase ID name
            $interface["descr"] = $interface["descr"] ?: strtoupper($id);

            # Add the interface's value according to the APIs `represent_interfaces_as` setting.
            # Always assign the interfaces descriptive name as it's verbose choice
            switch ($this->represented_as) {
                case "internal":
                    $choices[$id] =$interface["descr"];
                    break;
                case "description":
                    $choices[$interface["descr"]] = $interface["descr"];
                    break;
                case "physical":
                    $choices[$interface["if"]] = $interface["descr"];
                    break;
            }
        }

        # Include interface groups if requested
        if ($this->allow_interface_groups) {
            # Loop through all existing interface groups and add their names
            foreach ($interface_groups as $interface_group) {
                $choices[$interface_group["ifname"]] = $interface_group["ifname"];
            }
        }

        # Include CARP interfaces if requested
        if ($this->allow_carp_interface) {
            # Loop through all existing interface groups and add their names
            foreach ($virtual_ips as $virtual_ip) {
                # Only include CARP virtual iPs with unique IDs
                if ($virtual_ip["mode"] === "carp" and $virtual_ip["uniqid"]) {
                    $choices[$virtual_ip["uniqid"]] = $virtual_ip["uniqid"];
                }
            }
        }

        # Allow the IPsec `enc0` interface if requested
        if ($this->allow_ipsec_interface) {
            $choices["enc0"] = "IPsec";
        }

        # Allow the OpenVPN `openvpn` interface if requested
        if ($this->allow_openvpn_interface) {
            $choices["openvpn"] = "OpenVPN";
        }

        return $choices;
    }

    /**
     * Obtains this Fields current interface values as an array of their internal values.
     * @return array|string An array of the internal interfaces assigned to this Field (e.g. ["wan", "lan", "opt1"])
     * when this is a `many` Field, or the internal interface value as a string
     */
    public function to_internal_interfaces() {
        # Return the internal interface values for each interface value when this is a `many` Field
        if ($this->many) {
            return explode($this->delimiter, $this->to_internal());
        }

        # Otherwise, just return the internal value
        return $this->to_internal();
    }

}
