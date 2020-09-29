#!/usr/local/bin/php -f
<?php
//   Copyright 2020 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

# MAKEENDPOINTS COMMAND
if ($argv[1] === "buildendpoints") {
    # Import each endpoint class
    foreach(glob("/etc/inc/api/endpoints/*.inc") as $file) {
        # Import classes files and create object
        require_once($file);
        $endpoint_class = str_replace(".inc", "", basename($file));
        $endpoint_obj = new $endpoint_class();

        # Specify the PHP code to write to the endpoints index.php file
        $code = "<?php\nrequire_once('".$file."');\n(new ".$endpoint_class."())->listen();\n";

        # Create directories and files corresponding with class
        if (!is_null($endpoint_obj->url)) {
            mkdir("/usr/local/www".$endpoint_obj->url, 0755, true);
            file_put_contents(
                "/usr/local/www".$endpoint_obj->url."/index.php",
                $code
            );
        }

        # Print success output if file now exists, otherwise output error and exit on non-zero code
        if (!is_null($endpoint_obj->url) and is_file("/usr/local/www".$endpoint_obj->url."/index.php")) {
            echo "Builing ".$endpoint_class." endpoint at URL \"".$endpoint_obj->url."\"... done.".PHP_EOL;
        } else {
            echo "Failed to build ".$endpoint_class." endpoint at URL \"".$endpoint_obj->url."\"".PHP_EOL;
            exit(1);
        }
    }
}
