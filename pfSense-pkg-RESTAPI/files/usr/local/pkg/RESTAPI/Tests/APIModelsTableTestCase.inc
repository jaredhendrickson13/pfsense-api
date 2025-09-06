<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\CARP;
use RESTAPI\Models\Table;
use RESTAPI\Models\VirtualIP;
use RESTAPI\Responses\ServerError;

class APIModelsTableTestCase extends TestCase {
    /**
     * Adds a new table using pfctl directly and waits until it is readable.
     * @param string $table_name The name of the table to add.
     * @param array $entries The entries to add to the table.
     * @return Command The Command object representing the result of pfctl command.
     */
    public function add_table(string $table_name, array $entries): Command {
        $entries_str = implode(' ', $entries);
        $add_cmd = new Command("/sbin/pfctl -t $table_name -T add $entries_str");

        # Wait until the table is readable by pfctl
        foreach (range(1, 5) as $i) {
            $show_cmd = new Command("/sbin/pfctl -t $table_name -T show");
            if (str_contains($show_cmd->output, $entries[0])) {
                return $add_cmd;
            }
            sleep(1);
        }

        throw new ServerError(
            message: "Failed to add table $table_name with entries",
            response_id: 'API_MODELS_TABLE_TEST_CASE_ADD_TABLE_FAILED',
        );
    }

    /**
     * Checks that we can successfully retrieve the list of available table names.
     */
    public function test_get_available_table_names(): void {
        # Create a new pf table to test with
        $this->add_table(table_name: 'pfrest_test_table', entries: ['1.2.3.4']);

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
        $this->add_table(table_name: 'pfrest_test_table', entries: ['1.2.3.4', '4.3.2.1']);

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
        $this->add_table(table_name: 'pfrest_test_table', entries: ['1.2.3.4', '4.3.2.1']);
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
