<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDView;

class APIModelsBINDViewTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-bind'];

    /**
     * Check that we can create, read, update and delete a BIND view.
     */
    public function test_crud(): void {
        $view = new BINDView(
            name: 'test_view',
            descr: 'A test view.',
            recursion: true,
            match_clients: ['any'],
            allow_recursion: ['localhost'],
            bind_custom_options: 'example_option value;',
            async: false,
        );
        $view->create(apply: true);

        # Ensure the view was created in named.conf
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_contains($named_conf, 'view "test_view" {');
        $this->assert_str_contains($named_conf, 'recursion yes;');
        $this->assert_str_contains($named_conf, 'match-clients { any; };');
        $this->assert_str_contains($named_conf, 'allow-recursion { localhost; };');
        $this->assert_str_contains($named_conf, 'example_option value;');

        # Update the view
        $view->name->value = 'test_view_updated';
        $view->recursion->value = false;
        $view->match_clients->value = ['localhost'];
        $view->allow_recursion->value = ['any'];
        $view->bind_custom_options->value = 'example_option updated;';
        $view->update(apply: true);

        # Ensure the view was updated in named.conf
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_contains($named_conf, 'view "test_view_updated" {');
        $this->assert_str_does_not_contain($named_conf, 'view "test_view" {');
        $this->assert_str_contains($named_conf, 'recursion no;');
        $this->assert_str_contains($named_conf, 'match-clients { localhost; };');
        $this->assert_str_contains($named_conf, 'allow-recursion { any; };');
        $this->assert_str_contains($named_conf, 'example_option updated;');

        # Delete the view
        $view->delete(apply: true);

        # Ensure the view was deleted from named.conf
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_does_not_contain($named_conf, 'view "test_view_updated" {');
    }
}
