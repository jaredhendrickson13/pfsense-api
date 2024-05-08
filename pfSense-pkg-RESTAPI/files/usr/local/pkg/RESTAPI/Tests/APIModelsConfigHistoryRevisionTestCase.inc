<?php

namespace RESTAPI\Tests;

use RESTAPI\Models\ConfigHistoryRevision;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Core\TestCase;

class APIModelsConfigHistoryRevisionTestCase extends TestCase
{
    /**
     * Checks if we can correctly read the configuration history.
     */
    public function test_read(): void {
        $config_history = new ConfigHistoryRevision(id: 0);
        $this->assert_is_not_empty($config_history->time->value);
        $this->assert_is_not_empty($config_history->description->value);
        $this->assert_is_not_empty($config_history->version->value);
        $this->assert_is_not_empty($config_history->filesize->value);
    }

    /**
     * Check that the deletion of a ConfigHistoryRevision object deletes the associated backup file.
     */
    public function test_delete(): void {
        # Make some changes to populate config revisions
        $alias1 = new FirewallAlias(name: 'testalias1', type: 'host');
        $alias1->create();
        $alias2 = new FirewallAlias(name: 'testalias2', type: 'host');
        $alias2->create();

        # Get the most recent config revision and ensure it's backup exists
        $last_backup = new ConfigHistoryRevision(id: 0);
        $this->assert_is_true(is_file("/cf/conf/backup/config-{$last_backup->time->value}.xml"));

        # Delete the most recent config revision and ensure the  file no longer exists
        $last_backup->delete();
        $this->assert_is_false(is_file("/cf/conf/backup/config-{$last_backup->time->value}.xml"));
    }
}