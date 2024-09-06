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
     * @var string $file_name
     * The file name for this schema.
     */
    public string $file_name = 'unnamed_schema.txt';

    /**
     * @var string $file_path
     * The full file path to the schema file.
     */
    public string $file_path;

    /**
     * @var string $content_type
     * The content type for this schema. This is used to determine how the schema is formatted and displayed when
     * accessed via the URL.
     */
    public string $content_type = 'text/plain';

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
     * Builds a unique endpoint URL for this schema in the pfSense webroot directory.
     */
    public function build_schema_url(): void
    {
        $name = pathinfo($this->file_name, PATHINFO_FILENAME);

        # Specify the PHP code to write to the Endpoints index.php file
        $code =
            "<?php\n" .
            "require_once('RESTAPI/autoloader.inc');\n" .
            "header('Content-Type: $this->content_type');\n" .
            "header('Referer: no-referrer');\n" .
            "echo (new {$this->get_class_fqn()}())->read_schema_from_file();\n" .
            "exit();\n";

        # Write the code to the schema's index.php file
        mkdir("/usr/local/www/api/v2/schema/$name", 0755, true);
        file_put_contents("/usr/local/www/api/v2/schema/$name/index.php", $code);
    }

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
