<?php

namespace RESTAPI\ModelTraits;

use RESTAPI\Core\Command;
use RESTAPI\Responses\NotAcceptableError;
use RESTAPI\Responses\NotFoundError;

/**
 * Defines a set of traits that are intended to be used by Models to read, decompress and process log files.
 */
trait LogFileModelTraits {
    /**
     * Checks if a given log file exists.
     * @param string $filepath The path to the log file.
     * @throws NotFoundError If the log file does not exist.
     */
    private function check_file_exists(string $filepath): void {
        # Check if the file exists
        if (!file_exists($filepath)) {
            throw new NotFoundError(
                message: "The log file at $filepath does not exist.",
                response_id: 'LOG_FILE_TRAITS_FILE_NOT_FOUND',
            );
        }
    }

    /**
     * Reads a given log file with no compression and returns the contents as an array of lines.
     * @param string $filepath The path to the log file.
     * @return array An array of log file contents.
     */
    private function read_uncompressed_log(string $filepath): array {
        # Check if the file exists
        $this->check_file_exists($filepath);

        # Read the file and return the contents as an array of lines
        return file($filepath, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    }

    /**
     * Decompresses a bzip2 compressed log file and returns the contents as an array of lines.
     * @param string $filepath The path to the log file.
     * @return array An array of log file contents.
     */
    private function read_bzip2_log(string $filepath): array {
        # Check if the file exists
        $this->check_file_exists($filepath);

        # Decompress the file and return the contents as an array of lines
        return explode(PHP_EOL, bzdecompress(file_get_contents($filepath)));
    }

    /**
     * Decompresses a gzip compressed log file and returns the contents as an array of lines.
     * @param string $filepath The path to the log file.
     * @return array An array of log file contents.
     */
    private function read_gzip_log(string $filepath): array {
        # Check if the file exists
        $this->check_file_exists($filepath);

        # Decompress the file and return the contents as an array of lines
        return explode(PHP_EOL, gzdecode(file_get_contents($filepath)));
    }

    /**
     * Decompresses a xz compressed log file and returns the contents as an array of lines.
     * @param string $filepath The path to the log file.
     * @return array An array of log file contents.
     */
    private function read_xz_log(string $filepath): array {
        # Check if the file exists
        $this->check_file_exists($filepath);

        # Run xz to read the compressed file
        $xz_decompress = new Command('xz -dc ' . escapeshellarg($filepath));

        # Decompress the file and return the contents as an array of lines
        return explode(PHP_EOL, $xz_decompress->output);
    }

    /**
     * Gather all log filepaths for a given base log.
     * @param string $base_log The base log file path.
     * @return array An array of all log file paths for the given base log.
     */
    private function gather_log_filepaths(string $base_log): array {
        # Variables
        $log_filepaths = [];

        # Gather all log file paths
        foreach (glob($base_log . '*') as $log_filepath) {
            $log_filepaths[] = $log_filepath;
        }

        # Reverse the array so the oldest logs are read first
        return array_reverse($log_filepaths);
    }

    /**
     * Reads all available logs for a given base log file path and returns the contents as an array of lines. Warning:
     * This will include rotated logs, including compressed logs which may have a performance impact.
     * @note zstd compressed logs are not supported.
     * @param string $base_log The base log file path.
     * @return array An array of all log file contents for the given base log.
     */
    public function read_log(string $base_log): array {
        # Variables
        $log_contents = [];

        # Ensurethe base log file exists
        $this->check_file_exists($base_log);

        # Gather all log file paths
        $log_filepaths = $this->gather_log_filepaths($base_log);

        # Read each log file
        foreach ($log_filepaths as $log_filepath) {
            # For uncompressed rotated logs, remove numeric extension to use the correct method to read it below
            $log_filepath_normalized = preg_replace('/\.\d+$/', '', $log_filepath);

            # Determine the log file type and add the contents of the log file to the log contents array
            $log_contents = match (pathinfo($log_filepath_normalized, PATHINFO_EXTENSION)) {
                'bz2' => array_merge($log_contents, $this->read_bzip2_log($log_filepath)),
                'gz' => array_merge($log_contents, $this->read_gzip_log($log_filepath)),
                'xz' => array_merge($log_contents, $this->read_xz_log($log_filepath)),
                'log' => array_merge($log_contents, $this->read_uncompressed_log($log_filepath)),
                default => throw new NotAcceptableError(
                    message: "The log file at $log_filepath has an unsupported file extension.",
                    response_id: 'LOG_FILE_TRAITS_UNSUPPORTED_LOG_FILE_EXTENSION',
                ),
            };
        }

        # Clear out empty array elements and reindex the array
        $log_contents = array_filter($log_contents);
        $log_contents = array_values($log_contents);

        # Map the log contents so each entry is an object with a 'text' property
        return array_map(fn($line) => ['text' => $line], $log_contents);
    }
}
