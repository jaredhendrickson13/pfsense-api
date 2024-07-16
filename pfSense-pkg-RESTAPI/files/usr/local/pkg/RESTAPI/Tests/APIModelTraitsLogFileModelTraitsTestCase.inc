<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemLog;

class APIModelTraitsLogFileModelTraitsTestCase extends TestCase {
    /**
     * Checks that the LogFileModelTraits::check_file_exists() method throws a NotFoundError when the file does not exist.
     */
    public function test_check_file_exists(): void {
        # Ensure an error is thrown when the requested log file does not exist
        $this->assert_throws_response(
            response_id: 'LOG_FILE_TRAITS_FILE_NOT_FOUND',
            code: 404,
            callable: function () {
                # Use the SystemLog Model to test the LogFileModelTraits::check_file_exists() method
                $model = new SystemLog();

                # LogFileModelTraits::check_file_exists() is private, call it via read_log() instead
                $model->read_log('/var/log/does_not_exist.log');
            },
        );

        # Ensure no error is thrown when the requested log file exists
        $this->assert_does_not_throw(
            callable: function () {
                # Use the SystemLog Model to test the LogFileModelTraits::check_file_exists() method
                $model = new SystemLog();

                # LogFileModelTraits::check_file_exists() is private, call it via read_log() instead
                $model->read_log('/var/log/system.log');
            },
        );
    }

    /**
     * Checks that the LogFileModelTraits::read_uncompressed_log() method reads an uncompressed log file correctly.
     */
    public function test_read_uncompressed_log(): void {
        # Use the SystemLog Model to test the LogFileModelTraits::read_uncompressed_log() method
        $model = new SystemLog();

        # Create some mock log files that are uncompressed
        file_put_contents('/tmp/test_uncompressed.log', "Line 7\nLine 8\nLine 9\n");
        file_put_contents('/tmp/test_uncompressed.log.0', "Line 4\nLine 5\nLine 6\n");
        file_put_contents('/tmp/test_uncompressed.log.1', "Line 1\nLine 2\nLine 3\n");

        # Read the system log file
        $log = $model->read_log('/tmp/test_uncompressed.log');

        # Ensure all log files were read correctly and in the correct order
        $this->assert_equals(
            [
                ['text' => 'Line 1'],
                ['text' => 'Line 2'],
                ['text' => 'Line 3'],
                ['text' => 'Line 4'],
                ['text' => 'Line 5'],
                ['text' => 'Line 6'],
                ['text' => 'Line 7'],
                ['text' => 'Line 8'],
                ['text' => 'Line 9'],
            ],
            $log,
        );
    }

    /**
     * Checks that the LogFileModelTraits::read_bzip2_log() method reads a bzip2 compressed log file correctly.
     */
    public function test_read_bzip2_log(): void {
        # Use the SystemLog Model to test the LogFileModelTraits::read_bzip2_log() method
        $model = new SystemLog();

        # Create some mock log files that are bzip2 compressed
        file_put_contents('/tmp/test_bzip2.log', "Line 7\nLine 8\nLine 9\n");
        file_put_contents('/tmp/test_bzip2.log.0.bz2', bzcompress("Line 4\nLine 5\nLine 6\n"));
        file_put_contents('/tmp/test_bzip2.log.1.bz2', bzcompress("Line 1\nLine 2\nLine 3\n"));

        # Read the system log file
        $log = $model->read_log('/tmp/test_bzip2.log');

        # Ensure all log files were read correctly and in the correct order
        $this->assert_equals(
            [
                ['text' => 'Line 1'],
                ['text' => 'Line 2'],
                ['text' => 'Line 3'],
                ['text' => 'Line 4'],
                ['text' => 'Line 5'],
                ['text' => 'Line 6'],
                ['text' => 'Line 7'],
                ['text' => 'Line 8'],
                ['text' => 'Line 9'],
            ],
            $log,
        );
    }

    /**
     * Checks that the LogFileModelTraits::read_gzip_log() method reads a gzip compressed log file correctly.
     */
    public function test_read_gzip_log(): void {
        # Use the SystemLog Model to test the LogFileModelTraits::read_gzip_log() method
        $model = new SystemLog();

        # Create some mock log files that are gzip compressed
        file_put_contents('/tmp/test_gzip.log', "Line 7\nLine 8\nLine 9\n");
        file_put_contents('/tmp/test_gzip.log.0.gz', gzencode("Line 4\nLine 5\nLine 6\n"));
        file_put_contents('/tmp/test_gzip.log.1.gz', gzencode("Line 1\nLine 2\nLine 3\n"));

        # Read the system log file
        $log = $model->read_log('/tmp/test_gzip.log');

        # Ensure all log files were read correctly and in the correct order
        $this->assert_equals(
            [
                ['text' => 'Line 1'],
                ['text' => 'Line 2'],
                ['text' => 'Line 3'],
                ['text' => 'Line 4'],
                ['text' => 'Line 5'],
                ['text' => 'Line 6'],
                ['text' => 'Line 7'],
                ['text' => 'Line 8'],
                ['text' => 'Line 9'],
            ],
            $log,
        );
    }

    /**
     * Checks that the LogFileModelTraits::read_xz_log() method reads an xz compressed log file correctly.
     */
    public function test_read_xz_log(): void {
        # Use the SystemLog Model to test the LogFileModelTraits::read_xz_log() method
        $model = new SystemLog();

        # Create some mock log files that are xz compressed
        file_put_contents('/tmp/test_xz.log', "Line 7\nLine 8\nLine 9\n");
        new Command('printf "Line 4\nLine 5\nLine 6\n" | xz -c > /tmp/test_xz.log.0.xz');
        new Command('printf "Line 1\nLine 2\nLine 3\n" | xz -c > /tmp/test_xz.log.1.xz');

        # Read the system log file
        $log = $model->read_log('/tmp/test_xz.log');

        # Ensure all log files were read correctly and in the correct order
        $this->assert_equals(
            [
                ['text' => 'Line 1'],
                ['text' => 'Line 2'],
                ['text' => 'Line 3'],
                ['text' => 'Line 4'],
                ['text' => 'Line 5'],
                ['text' => 'Line 6'],
                ['text' => 'Line 7'],
                ['text' => 'Line 8'],
                ['text' => 'Line 9'],
            ],
            $log,
        );
    }
}
