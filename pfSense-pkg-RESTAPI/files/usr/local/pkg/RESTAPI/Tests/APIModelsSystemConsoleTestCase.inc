<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemConsole;

class APIModelsSystemConsoleTestCase extends TestCase
{
    /**
     * Ensure /etc/ttys correctly shows the console menu's current password requirement.
     */
    public function test_ttys_set_after_update(): void
    {
        # Create a SystemConsole object to test with
        $console = new SystemConsole();

        # Enable password protected console and ensure it is correctly represented in /etc/ttys
        $console->passwd_protect_console->value = true;
        $console->update();
        $ttys = file_get_contents('/etc/ttys');
        $this->assert_str_contains($ttys, 'ttyu0	"/usr/libexec/getty 3wire"');
        $this->assert_str_contains($ttys, 'ttyu1	"/usr/libexec/getty 3wire"');
        $this->assert_str_contains($ttys, 'ttyu2	"/usr/libexec/getty 3wire"');
        $this->assert_str_contains($ttys, 'ttyu3	"/usr/libexec/getty 3wire"');
        $this->assert_str_contains($ttys, 'xc0	"/usr/libexec/getty Pc"');

        # Disable password protected console and ensure it is correctly represented in /etc/ttys
        $console->passwd_protect_console->value = false;
        $console->update();
        $ttys = file_get_contents('/etc/ttys');
        $this->assert_str_contains($ttys, 'ttyu0	"/usr/libexec/getty al.3wire"');
        $this->assert_str_contains($ttys, 'ttyu1	"/usr/libexec/getty al.3wire"');
        $this->assert_str_contains($ttys, 'ttyu2	"/usr/libexec/getty al.3wire"');
        $this->assert_str_contains($ttys, 'ttyu3	"/usr/libexec/getty al.3wire"');
        $this->assert_str_contains($ttys, 'xc0	"/usr/libexec/getty al.Pc"');
    }
}
