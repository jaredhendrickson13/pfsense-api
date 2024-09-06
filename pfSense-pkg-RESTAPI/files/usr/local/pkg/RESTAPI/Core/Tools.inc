<?php
//   Copyright 2024 Jared Hendrickson
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

namespace RESTAPI\Core\Tools;

use RESTAPI\Core\Response;
use RESTAPI\Models\RESTAPIVersion;
use RESTAPI\Responses\ServerError;
use stdClass;

require_once 'RESTAPI/autoloader.inc';

/**
 * Checks if a given array is an associative array.
 * @param array $array The array to check.
 * @return bool `true` if the value is an associative array. `false` if it is an indexed array.
 */
function is_assoc_array(array $array): bool {
    # Start index tracking at 0
    $index = 0;

    # Loop through the keys of this array to check for non-associative indexing
    foreach (array_keys($array) as $key) {
        # This is an associative array if the key is not an integer, or the index is not sequential
        if (!is_integer($key) or $key !== $index) {
            return true;
        }

        # Increase the index tracker
        $index++;
    }

    return false;
}

/**
 * Converts a string to UpperCamelCase.
 * @param string $string The string to convert to UpperCamelCase.
 * @return string The string converted to UpperCamelCase.
 */
function to_upper_camel_case(string $string): string {
    # Remove non-alphanumeric characters and convert to lowercase
    $string = preg_replace('/[^a-zA-Z0-9]+/', ' ', $string);
    $string = trim($string);

    # Capitalize the first letter of each word and remove whitespace
    return str_replace(' ', '', ucwords($string));
}

/**
 * Conditionally prints a message to the console. The specified $message will only be printed if $condition is true.
 * @param string $message The message to be printed if the $condition is true.
 * @param bool $condition A true or false condition. If the condition is true, the $message will be printed. Otherwise
 * it will not.
 */
function cprint(string $message, bool $condition): void {
    if ($condition) {
        print $message;
    }
}

/**
 * Converts a $bandwidth to bits for it's given $bandwidthtype
 * @param int $bandwidth The amount of bandwidth scaled using $bandwidthtype.
 * @param string $scale The scale of $bandwidth. Options are: b, Kb, Mb, and Gb.
 * @return int The bandwidth's total number of bits.
 * @throws ServerError When $bandwidthtype is not a supported option.
 */
function bandwidth_to_bits(int $bandwidth, string $scale): int {
    # Set scale factors we can use to convert various bandwidth values into total bit count
    $gigabit_scale_factor = 1024 * 1024 * 1024;
    $megabit_scale_factor = 1024 * 1024;
    $kilobit_scale_factor = 1024;

    return match ($scale) {
        'Gb' => $bandwidth * $gigabit_scale_factor,
        'Mb' => $bandwidth * $megabit_scale_factor,
        'Kb' => $bandwidth * $kilobit_scale_factor,
        'b' => $bandwidth, // Already represented as bits
        default => throw new ServerError(
            message: "Unsupported \$bandwidthtype value `$scale` provided to bandwidth_to_bits(). " .
                'Supported options are b, Kb, Mb, and Gb.',
            response_id: 'TOOLS_BANDWIDTH_TO_BITS_INVALID_SCALE_OPTION',
        ),
    };
}

/**
 * Makes an HTTP request.
 * @param string $url The full URL of the resource to submit the request to.
 * @param string $method The HTTP method to use to submit the request.
 * @param array $data The data payload to submit to the remote host. Only JSON payloads are supported.
 * @param array $headers The HTTP headers to submit in the request.
 * @param string $username The username portion for basic authentication (if applicable)
 * @param string $password The password portion for basic authentication (if applicable)
 * @param bool $validate_certs Enables or disables TLS certificate verification.
 * @return bool|string The response from the remote host.
 */
function http_request(
    string $url,
    string $method,
    array $data = [],
    array $headers = [],
    string $username = '',
    string $password = '',
    bool $validate_certs = true,
): bool|string {
    # Format data and headers
    $data = json_encode($data);

    $headers[] = 'content-type: application/json';
    $headers[] = 'content-length: ' . strlen($data);

    # Setup cURL options
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, (int) $validate_certs);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, (int) $validate_certs);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 10);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, strtoupper($method));
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);

    # Only include basic authentication if username and password are set
    if ($username and $password) {
        curl_setopt($ch, CURLOPT_USERPWD, $username . ':' . $password);
    }

    # Make our request and return the response
    $result = curl_exec($ch);
    curl_close($ch);
    return $result;
}

/**
 * Obtains the classes nested under a specified namespace. This only works for namespaces that match the directory
 * structure (e.g. RESTAPI\Core for /usr/local/pkg/RESTAPI/Core) and only includes classes defined in their own class .inc file with
 * a matching name.
 * @param string $namespace The namespace to obtain classes from. This must be the full namespace including leading
 * and trailing slash (\Full\Namespace\).
 * @param bool $shortnames Obtain the class shortnames instead of FQNs.
 * @return array An array containing the class names for each .inc class file.
 */
function get_classes_from_namespace(string $namespace, bool $shortnames = false): array {
    # Variables
    $classes = [];
    $namespace = !str_starts_with($namespace, '\\') ? $namespace . '\\' : $namespace;
    $namespace = !str_starts_with($namespace, '\\') ? '\\' . $namespace : $namespace;
    $namespace_path = str_replace(['\\', '//'], '/', '/usr/local/pkg/' . $namespace);

    foreach (glob($namespace_path . '*.inc') as $inc_file) {
        # Obtain the class name from the file name
        $class_name = basename($inc_file);
        $class_name = str_replace('.inc', '', $class_name);
        $classes[] = $shortnames ? $class_name : $namespace . $class_name;
    }

    return $classes;
}
