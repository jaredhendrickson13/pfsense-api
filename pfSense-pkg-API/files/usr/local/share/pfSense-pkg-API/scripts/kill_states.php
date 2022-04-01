<?php
//   Copyright 2022 Jared Hendrickson
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


# This script is intended to help API endpoints that need to kill firewall states. This script can be ran in the
# background by PHP to prevent the connection from being killed before the API can respond.
# ---------------------------------------------------------------------------------------------------------------
# Argument 'sleep': required : the number (in seconds) the script should wait before killing firewall states
# Argument 'source': required : the source IP/CIDR of states to be killed
# Argument 'destination: optional : the destination IP/CIDR of states to be killed
# Argument 'protocol': optional : the protocol of states to be killed
# Argument 'interface': optional : the interface of states to be killed
#
# Example: php -f kill_states.php source=0.0.0.0/0 destination=1.2.3.4 protocol=tcp interface=em0 sleep=5

require_once("api/framework/APITools.inc");

# Variables
$source = null;
$destination = null;
$protocol = null;
$interface = null;
$sleep = 0;

# Loop through all passed in arguments and extract our expected arguments
foreach ($argv as $id=>$arg) {
    # Look for our sleep argument
    if (APITools\str_starts_with("sleep=", $arg)) {
        $sleep = str_replace("sleep=", "", $arg);
    }
    # Look for our source argument
    elseif (APITools\str_starts_with("source=", $arg)) {
        $source = str_replace("source=", "", $arg);
    }
    # Look for our destination argument
    elseif (APITools\str_starts_with("destination=", $arg)) {
        $destination = str_replace("destination=", "", $arg);
    }
    # Look for our protocol argument
    elseif (APITools\str_starts_with("protocol=", $arg)) {
        $protocol = str_replace("protocol=", "", $arg);
    }
    # Look for our interface argument
    elseif (APITools\str_starts_with("interface=", $arg)) {
        $interface = str_replace("interface=", "", $arg);
    }
}

# Wait for the desired amount of time and then kill the requested states
sleep(intval($sleep));
pfSense_kill_states(
    $source,
    $destination,
    $protocol,
    $interface
);
