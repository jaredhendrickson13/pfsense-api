#!/usr/local/bin/php -f
<?php

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
