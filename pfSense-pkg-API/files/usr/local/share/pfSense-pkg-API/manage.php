#!/usr/local/bin/php -f
<?php
//   Copyright 2021 Jared Hendrickson
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
require_once("api/framework/APITools.inc");

function build_endpoints() {
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
            echo "Building ".$endpoint_class." endpoint at URL \"".$endpoint_obj->url."\"... done.".PHP_EOL;
        } else {
            echo "Building ".$endpoint_class." endpoint at URL \"".$endpoint_obj->url."\"... failed.".PHP_EOL;
            exit(1);
        }
    }
}

function backup() {
    # Local Variables
    $api_conf = APITools\get_api_config()[1];
    $backup_api_conf = json_encode($api_conf);
    file_put_contents("/usr/local/share/pfSense-pkg-API/backup.json", $backup_api_conf);
    echo "Backing up API configuration... done.".PHP_EOL;
}

function restore() {
    global $config;

    # Local Variables
    $api_conf = APITools\get_api_config();

    # Only retrieve file contents if it exists
    if (is_file("/usr/local/share/pfSense-pkg-API/backup.json")) {
        $backup_api_conf_json = file_get_contents("/usr/local/share/pfSense-pkg-API/backup.json");
        $backup_api_conf = json_decode($backup_api_conf_json, true);;
    }

    # Restore our API configuration if the backup exists
    if (!empty($backup_api_conf_json)) {
        # Only restore the config if it has changed
        if ($api_conf[1] !== $backup_api_conf) {
            $config["installedpackages"]["package"][$api_conf[0]]["conf"] = $backup_api_conf;
            write_config("Synchronized persistent API configuration");
            echo "Restoring API configuration... done.".PHP_EOL;
        } else {
            echo "Restoring API configuration... no changes found.".PHP_EOL;
        }
    } else {
        echo "Restoring API configuration... no configuration found.".PHP_EOL;
    }
}

function sync() {
    APITools\sync(true);
}

function update() {
    $pf_version = substr(file_get_contents("/etc/version"), 0, 3);
    echo shell_exec("/usr/sbin/pkg delete -y pfSense-pkg-API");
    echo shell_exec("/usr/sbin/pkg add https://github.com/jaredhendrickson13/pfsense-api/releases/latest/download/pfSense-".$pf_version."-pkg-API.txz");
    echo shell_exec("/etc/rc.restart_webgui");
}

function revert($version) {
    # Local variables
    $pf_version = substr(file_get_contents("/etc/version"), 0, 3);
    $url = "https://github.com/jaredhendrickson13/pfsense-api/releases/download/".urlencode($version)."/pfSense-".$pf_version."-pkg-API.txz";
    echo "Locating pfSense-pkg-API-".$version." at ".$url."... ";

    # Check our URL for the existence of this version
    $headers = @get_headers($url);

    # If this release exists, remove the existing package and install the requested one. Otherwise return error
    if($headers && strpos($headers[0], '302')) {
        echo "done.".PHP_EOL;
        echo shell_exec("/usr/sbin/pkg delete -y pfSense-pkg-API");
        echo shell_exec("/usr/sbin/pkg add ".$url);
        echo shell_exec("/etc/rc.restart_webgui");
    } else {
        echo "no package found.".PHP_EOL;
        exit(1);
    }
}

function delete() {
    echo shell_exec("/usr/sbin/pkg delete -y pfSense-pkg-API");
    echo shell_exec("/etc/rc.restart_webgui");
}

function rotate_server_key() {
    global $config;
    $pkg_index = APITools\get_api_config()[0];
    $config["installedpackages"]["package"][$pkg_index]["conf"]["keys"] = [];
    echo "Rotating API server key... ";
    APITools\create_jwt_server_key(true);
    echo "done.".PHP_EOL;
}

function version() {
    echo shell_exec("/usr/sbin/pkg info pfSense-pkg-API");
}

function help() {
    echo "pfsense-api - CLI tool for pfSense API management".PHP_EOL;
    echo "Copyright - ".date("Y")."© - Jared Hendrickson".PHP_EOL;
    echo "SYNTAX:".PHP_EOL;
    echo "  pfsense-api <command> <args>".PHP_EOL;
    echo "COMMANDS:".PHP_EOL;
    echo "  version          : Display the current package version and build information".PHP_EOL;
    echo "  help             : Display the help page (this page)".PHP_EOL;
    echo "  buildendpoints   : Build all API endpoints included in this package".PHP_EOL;
    echo "  update           : Update package to the latest stable version available".PHP_EOL;
    echo "  revert           : Revert package to a specified version".PHP_EOL;
    echo "  delete           : Delete package from this system".PHP_EOL;
    echo "  rotateserverkey  : Rotate the API server key and remove all existing tokens".PHP_EOL;
    echo "  backup           : Create a backup of the API configuration".PHP_EOL;
    echo "  restore          : Restore the API configuration from the latest backup".PHP_EOL;
    echo "  sync             : Sync this system's API configuration to configured HA nodes".PHP_EOL;
    echo PHP_EOL;
}

# MAKEENDPOINTS COMMAND
if (in_array($argv[1], ["buildendpoints"])) {
    build_endpoints();
}
# BACKUP COMMAND
elseif (in_array($argv[1], ["backup"])) {
    backup();
}
# RESTORE COMMAND
elseif (in_array($argv[1], ["restore"])) {
    restore();
}
# SYNC COMMAND
elseif (in_array($argv[1], ["sync"])) {
    sync();
}
# UPDATE COMMAND
elseif (in_array($argv[1], ["update"])) {
    update();
}
# REVERT COMMAND
elseif (in_array($argv[1], ["revert"])) {
    revert($argv[2]);
}
# DELETE COMMAND
elseif (in_array($argv[1], ["delete"])) {
    delete();
}
# ROTATESERVERKEY COMMAND
elseif (in_array($argv[1], ["rotateserverkey"])) {
    rotate_server_key();
}
# VERSION COMMAND
elseif (in_array($argv[1], ["version"])) {
    version();
}
# HELP COMMAND
elseif (in_array($argv[1], ["help", null])) {
    help();

}
# UNKNOWN COMMAND/DEFAULT
else {
    echo "Unknown command".PHP_EOL.PHP_EOL;
    help();
}
