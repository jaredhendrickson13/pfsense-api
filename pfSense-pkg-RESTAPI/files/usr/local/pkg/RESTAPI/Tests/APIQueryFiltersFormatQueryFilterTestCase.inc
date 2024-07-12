<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallAlias;

class APIQueryFiltersFormatQueryFilterTestCase extends TestCase {
    /**
     * Checks that the evaluate method correctly identifies values that match the requested formats.
     */
    public function test_evaluate(): void {
        $filter = new \RESTAPI\QueryFilters\FormatQueryFilter();

        # Test IPv4 address
        $this->assert_is_true($filter->evaluate('1.2.3.4', 'ipv4'));
        $this->assert_is_false($filter->evaluate('not an ip', 'ipv4'));
        $this->assert_is_false($filter->evaluate('2001:0db8:85a3:0000:0000:8a2e:0370:7334', 'ipv4'));

        # Test IPv6 address
        $this->assert_is_true($filter->evaluate('2001:0db8:85a3:0000:0000:8a2e:0370:7334', 'ipv6'));
        $this->assert_is_false($filter->evaluate('not an ip', 'ipv6'));
        $this->assert_is_false($filter->evaluate('1.2.3.4', 'ipv6'));

        # Test IP address
        $this->assert_is_true($filter->evaluate('1.2.3.4', 'ip'));
        $this->assert_is_true($filter->evaluate('2001:0db8:85a3:0000:0000:8a2e:0370:7334', 'ip'));
        $this->assert_is_false($filter->evaluate('not an ip', 'ip'));

        # Test MAC address
        $this->assert_is_true($filter->evaluate('00:11:22:33:44:55', 'mac'));
        $this->assert_is_false($filter->evaluate('not a mac', 'mac'));

        # Test IPv4 subnet
        $this->assert_is_true($filter->evaluate('1.2.3.4/24', 'subnetv4'));
        $this->assert_is_false($filter->evaluate('not a subnet', 'subnetv4'));
        $this->assert_is_false($filter->evaluate('2001:0db8:85a3:0000:0000:8a2e:0370:7334/64', 'subnetv4'));

        # Test IPv6 subnet
        $this->assert_is_true($filter->evaluate('2001:0db8:85a3:0000:0000:8a2e:0370:7334/64', 'subnetv6'));
        $this->assert_is_false($filter->evaluate('not a subnet', 'subnetv6'));
        $this->assert_is_false($filter->evaluate('1.2.3.4/24', 'subnetv6'));

        # Test subnet
        $this->assert_is_true($filter->evaluate('1.2.3.4/24', 'subnet'));
        $this->assert_is_true($filter->evaluate('2001:0db8:85a3:0000:0000:8a2e:0370:7334/64', 'subnet'));
        $this->assert_is_false($filter->evaluate('not a subnet', 'subnet'));

        # Test email address
        $this->assert_is_true($filter->evaluate('test@example.com', 'email'));
        $this->assert_is_false($filter->evaluate('not an email', 'email'));

        # Test URL
        $this->assert_is_true($filter->evaluate('http://example.com', 'url'));
        $this->assert_is_false($filter->evaluate('not a url', 'url'));

        # Test numeric value
        $this->assert_is_true($filter->evaluate('123', 'numeric'));
        $this->assert_is_true($filter->evaluate('123.45', 'numeric'));
        $this->assert_is_false($filter->evaluate('not a number', 'numeric'));

        # Test hostname
        $this->assert_is_true($filter->evaluate('example.com', 'hostname'));
        $this->assert_is_false($filter->evaluate('not a hostname', 'hostname'));

        # Test fully qualified domain name
        $this->assert_is_true($filter->evaluate('example.com.', 'fqdn'));
        $this->assert_is_false($filter->evaluate('not a fqdn', 'fqdn'));

        # Test port number
        $this->assert_is_true($filter->evaluate('80', 'port'));
        $this->assert_is_false($filter->evaluate('not a port', 'port'));
        $this->assert_is_false($filter->evaluate('65536', 'port'));

        # Test port range
        $this->assert_is_true($filter->evaluate('80:443', 'portrange'));
        $this->assert_is_false($filter->evaluate('not a port range', 'portrange'));
        $this->assert_is_false($filter->evaluate('80:65536', 'portrange'));

        # Test alias
        $alias = new FirewallAlias(name: 'testalias', type: 'host', address: [], async: false);
        $alias->create(apply: true);
        $this->assert_is_true($filter->evaluate('testalias', 'alias'));
        $this->assert_is_false($filter->evaluate('not an alias', 'alias'));
        $alias->delete(apply: true);
    }
}
