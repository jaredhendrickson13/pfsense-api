<?php

namespace RESTAPI\Core;

/**
 * Defines a class that is used to represent a specific schema for this API. This class is intended to be extended
 * by a child class to implement custom schema definitions using Endpoints, Models and Fields defined in this package.
 * Generated schema files are generally saved to /usr/local/www/api/v2/schemas/*. Schemas provide a way to define and
 * represent this API's data structure in a format that can be easily shared and understood by developers such as
 * OpenAPI, GraphQL, etc.
 */
abstract class Schema {
    use BaseTraits;

    /**
     * The directory where schema files are stored.
     */
    const SCHEMA_DIR = '/usr/local/pkg/RESTAPI/.resources/schemas/';

    /**
     * The file name for this schema.
     */
    public string $file_name = 'unnamed_schema.txt';

    /**
     * The full file path to the schema file.
     * @var string $file_path
     */
    public string $file_path;

    public function __construct() {
        $this->file_path = self::SCHEMA_DIR . $this->file_name;
    }

    /**
     * Obtains the schema as a string. This method must be implemented by the child class and is used to generate
     * the schema file for this class.
     * @return string The schema as a string.
     */
    abstract public function get_schema_str(): string;

    /**
     * Saves the schema to file.
     */
    public function save_schema(): void {
        file_put_contents($this->file_path, $this->get_schema_str());
    }

    /**
     * Reads the schema from file.
     * @return string The schema as a string.
     */
    public function read_schema_from_file(): string {
        return file_get_contents($this->file_path);
    }
}
