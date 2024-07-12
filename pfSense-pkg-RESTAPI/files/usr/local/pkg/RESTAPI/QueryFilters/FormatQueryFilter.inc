<?php

namespace RESTAPI\QueryFilters;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\QueryFilter;
use RESTAPI\Models\FirewallAlias;

/**
 * Defines a query filter that obtains matches to a field's value exactly.
 */
class FormatQueryFilter extends QueryFilter {
    public string $name = 'format';

    /**
     * Checks if the given field value matches the format given filter value.
     *
     * Supported formats are:
     * - ipv4: Checks if the field value is a valid IPv4 address.
     * - ipv6: Checks if the field value is a valid IPv6 address.
     * - ip: Checks if the field value is a valid IPv4 or IPv6 address.
     * - mac: Checks if the field value is a valid MAC address.
     * - subnetv4: Checks if the field value is a valid IPv4 subnet.
     * - subnetv6: Checks if the field value is a valid IPv6 subnet.
     * - subnet: Checks if the field value is a valid IPv4 or IPv6 subnet.
     * - email: Checks if the field value is a valid email address.
     * - url: Checks if the field value is a valid URL.
     * - numeric: Checks if the field value is a numeric value.
     * - hostname: Checks if the field value is a valid hostname.
     * - fqdn: Checks if the field value is a valid fully qualified domain name.
     * - port: Checks if the field value is a valid port number.
     * - portrange: Checks if the field value is a valid port range.
     * - alias: Checks if the field value is a valid alias.
     *
     * @param mixed $field_value The value of the field being used to filter.
     * @param mixed $filter_value The value of the filter criteria to evaluate against.
     * @return bool true if the field_value matches the filter_value, false otherwise.
     */
    public function evaluate(mixed $field_value, mixed $filter_value): bool {
        return match ($filter_value) {
            'ipv4' => is_ipaddrv4($field_value),
            'ipv6' => is_ipaddrv6($field_value),
            'ip' => (bool) is_ipaddr($field_value),
            'mac' => is_macaddr($field_value),
            'subnetv4' => is_subnetv4($field_value),
            'subnetv6' => is_subnetv6($field_value),
            'subnet' => (bool) is_subnet($field_value),
            'email' => filter_var($field_value, FILTER_VALIDATE_EMAIL) !== false,
            'url' => filter_var($field_value, FILTER_VALIDATE_URL) !== false,
            'numeric' => is_numeric($field_value),
            'hostname' => is_hostname($field_value),
            'fqdn' => is_fqdn($field_value),
            'port' => is_port($field_value),
            'portrange' => is_portrange($field_value),
            'alias' => FirewallAlias::query(name: $field_value)->exists(),
            default => false,
        };
    }
}
