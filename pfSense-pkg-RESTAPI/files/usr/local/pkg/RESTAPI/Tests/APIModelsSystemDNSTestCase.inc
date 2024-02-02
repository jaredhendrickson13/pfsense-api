<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemDNS;

class APIModelsSystemDNSTestCase extends TestCase
{
    /**
     * Checks that setting `dnslocalhost` to `local` only adds 127.0.0.1 as a DNS server in resolv.conf
     */
    public function test_dnslocalhost_local_resolvconf()
    {
        # Create the system DNS model object to test with
        $system_dns = new SystemDNS(async: false);

        # Set dnslocalhost to local and ensure 127.0.0.1 is the only DNS server listed
        $system_dns->dnslocalhost->value = 'local';
        $system_dns->update();
        $resolvconf = file_get_contents('/etc/resolv.conf');
        $this->assert_str_contains($resolvconf, 'nameserver 127.0.0.1');
    }

    /**
     * Checks that setting `dnslocalhost` to `remote` only adds specific DNS servers in resolv.conf
     */
    public function test_dnslocalhost_remote_resolvconf()
    {
        # Create the system DNS model object to test with
        $system_dns = new SystemDNS(async: false);

        # Set dnslocalhost to remote and ensure the remote DNS servers are the only servers listed
        $system_dns->dnslocalhost->value = 'remote';
        $system_dns->dnsserver->value = ['8.8.8.8', '8.8.4.4'];
        $system_dns->update();
        $resolvconf = file_get_contents('/etc/resolv.conf');
        $this->assert_str_does_not_contain($resolvconf, 'nameserver 127.0.0.1');
        $this->assert_str_contains($resolvconf, 'nameserver 8.8.8.8');
        $this->assert_str_contains($resolvconf, 'nameserver 8.8.4.4');
    }

    /**
     * Checks that setting `dnslocalhost` to `null` only adds the local and remote DNS servers in resolv.conf
     */
    public function test_dnslocalhost_null_resolvconf()
    {
        # Create the system DNS model object to test with
        $system_dns = new SystemDNS(async: false);

        # Set dnslocalhost to null and ensure 127.0.0.1 and remote DNS servers are listed
        $system_dns->dnslocalhost->value = null;
        $system_dns->dnsserver->value = ['8.8.8.8', '8.8.4.4'];
        $system_dns->update();
        $resolvconf = file_get_contents('/etc/resolv.conf');
        $this->assert_str_contains($resolvconf, 'nameserver 127.0.0.1');
        $this->assert_str_contains($resolvconf, 'nameserver 8.8.8.8');
        $this->assert_str_contains($resolvconf, 'nameserver 8.8.4.4');
    }
}
