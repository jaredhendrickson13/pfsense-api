<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\CARP;
use RESTAPI\Models\Table;
use RESTAPI\Models\VirtualIP;

class APIModelsTableTestCase extends TestCase {
    /**
     * Checks that we can successful retrieve the list of available table names.
     */
    public function test_get_available_table_names(): void {
        # Create a new pf table to test with
        new Command('/sbin/pfctl -t pfrest_test_table -T add 1.2.3.4');

        # Ensure get_available_table_names returns the test table
        $table = new Table();
        $this->assert_is_true(
            in_array('pfrest_test_table', $table->get_available_table_names()),
            message: 'The test table should be in the list of available tables.',
        );

        # Delete the test table after the test
        new Command('/sbin/pfctl -t pfrest_test_table -T kill');

        # Ensure the test table is no longer available
        $this->assert_is_false(
            in_array('pfrest_test_table', $table->get_available_table_names()),
            message: 'The test table should no longer be in the list of available tables.',
        );
    }

    /**
     * Checks that we can successfully read entries from a table
     */
    public function test_read(): void {
        # Create a new pf table to test with
        new Command('/sbin/pfctl -t pfrest_test_table -T add 1.2.3.4 4.3.2.1');

        # Load the Table model
        $table = new Table(id: 'pfrest_test_table');

        # Ensure the table has the expected entries
        $this->assert_equals($table->entries->value, ['1.2.3.4', '4.3.2.1']);

        # Delete the test table after the test
        new Command('/sbin/pfctl -t pfrest_test_table -T kill');
    }

    /**
     * Checks that we can successfully delete (flush) entrries from a table
     */
    public function test_delete(): void {
        # Create a new pf table to test with
        new Command('/sbin/pfctl -t pfrest_test_table -T add 1.2.3.4 4.3.2.1');
        sleep(1);

        # Load the Table model
        $table = new Table(id: 'pfrest_test_table');

        # Ensure the table entries are present (so we know delete actually flushes them)
        $this->assert_equals($table->entries->value, ['1.2.3.4', '4.3.2.1']);

        # Delete (flush) the test table and ensure the entries are actually flushed
        $table->delete();
        $table_show = new Command('/sbin/pfctl -t pfrest_test_table -T show');
        $this->assert_str_does_not_contain($table_show->output, '1.2.3.4');
        $this->assert_str_does_not_contain($table_show->output, '4.3.2.1');

        # Delete the test table after the test
        new Command('/sbin/pfctl -t pfrest_test_table -T kill');
    }
}
