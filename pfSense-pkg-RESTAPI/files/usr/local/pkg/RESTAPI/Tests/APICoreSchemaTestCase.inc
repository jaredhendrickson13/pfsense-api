<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Schemas\GraphQLSchema;

class APICoreSchemaTestCase extends TestCase {
    /**
     * Ensure the full file path is populated after construction
     */
    public function test_file_path(): void {
        $schema = new GraphQLSchema();
        $this->assert_equals($schema->file_path, $schema::SCHEMA_DIR . $schema->file_name);
    }

    /**
     * Ensure the schema is saved to file when the save_schema method is called.
     */
    public function test_save_schema(): void {
        # Use the GraphQLSchema for this since Schema is an abstract class.
        $schema = new GraphQLSchema();

        # Remove the schema file if it exists.
        unlink($schema->file_path);
        $schema->save_schema();
        $this->assert_is_true(file_exists($schema->file_path));
    }

    /**
     * Ensure we can read the schema from file.
     */
    public function test_read_schema_from_file(): void {
        # Use the GraphQLSchema for this since Schema is an abstract class.
        $schema = new GraphQLSchema();

        # Ensure the schema file exists.
        $schema->save_schema();
        $this->assert_is_true(file_exists($schema->file_path));

        # Ensure the schema is read from file.
        $this->assert_is_not_empty($schema->read_schema_from_file());
    }

    /**
     * Ensure the schema URL is built correctly.
     */
    public function test_build_schema_url(): void {
        # Use the GraphQLSchema for this since Schema is an abstract class.
        $schema = new GraphQLSchema();

        # Remove the schema file if it exists.
        unlink($schema->file_path);
        $schema->save_schema();
        $schema->build_schema_url();
        $this->assert_is_true(
            file_exists(
                '/usr/local/www/api/v2/schema/' . pathinfo($schema->file_name, PATHINFO_FILENAME) . '/index.php',
            ),
        );
    }
}
