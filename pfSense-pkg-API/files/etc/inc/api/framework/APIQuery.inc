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

require_once("api/framework/APITools.inc");

class APIQuery {
    public $response;
    public $data;
    public $payload;
    public $excluded;

    public function __construct($response, $excluded=[], $payload=null) {
        $this->response = $response;
        $this->data = [];
        $this->payload = (is_null($payload)) ? APITools\get_request_data() : $payload;
        $this->excluded = $excluded;
    }

    # Executes our query
    public function query() {
        $q_response = [];
        $q_count = 0;

        # First check that our response was successful
        if ($this->response["return"] === 0) {
            # Ensure our data field is an array and format our data property
            if (is_array($this->response["data"])) {
                $this->data = $this->response["data"];
                # Loop through each data item and check for query matches
                foreach ($this->data as $id=>$entry) {
                    $match = true;
                    foreach ($this->payload as $q_key=>$q_value) {
                        $q_result = $this->recurse($entry, $q_key, $q_value);
                        # Add entry if query matched, if null skip iteration, otherwise break
                        if ($q_result === true) {
                            $q_count++;
                        } elseif (is_null($q_result)) {
                            continue;
                        } else {
                            $match = false;
                            $q_count++;
                            break;
                        }
                    }
                    # If this query matched, add it to our new response
                    if ($match) {
                        $q_response[$id] = $entry;
                    }
                }
            }
        }

        # Return our query response if at least 1 query was performed
        if ($q_count > 0) {
            $this->response["data"] = $q_response;
            return $this->response;
        } else {
            return $this->response;
        }
    }

    # Checks if we can find a value match using array recursion
    private function recurse($entry, $key, $value) {
        # Variables
        $q_count = 0;
        $q_params = explode("__", $key);
        $q_key = $q_params[0];
        $q_params_count = count($q_params) - 1;
        $q_actions = ["startswith", "endswith", "contains", "lt", "lte", "gt", "gte"];
        $q_action = null;

        # Set our action if one was requested and remove the action from the parameters
        if ($q_params_count > 0 and in_array($q_params[$q_params_count], $q_actions)) {
            $q_action = $q_params[$q_params_count];
            unset($q_params[$q_params_count]);
            $q_params_count = count($q_params) - 1;
        }


        # Only proceed if key is not excluded from queries
        if (!$this->is_excluded($q_key)) {
            # Always prefer exact matches first
            if (is_array($entry) and $entry[$key] === $value) {
                return true;
            }

            # Loop through each query parameter and ensure it exists/matches the end value
            foreach ($q_params as $q) {
                if (is_array($entry) and array_key_exists($q, $entry)) {
                    # Check if we've reach the last entry
                    if ($q_count === $q_params_count) {
                        # Determine our corresponding query action and return it's result
                        switch ($q_action) {
                            case "startswith":
                                return $this->startswith($entry[$q], $value);
                            case "endswith":
                                return $this->endswith($entry[$q], $value);
                            case "contains":
                                return $this->contains($entry[$q], $value);
                            case "lt":
                                return $this->lt($entry[$q], $value);
                            case "lte":
                                return $this->lte($entry[$q], $value);
                            case "gt":
                                return $this->gt($entry[$q], $value);
                            case "gte":
                                return $this->gte($entry[$q], $value);
                            case null:
                                return ($entry[$q] === $value);
                            default:
                                return false;
                        }
                    } elseif (is_array($entry[$q])) {
                        $entry = $entry[$q];
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
                $q_count++;
            }
        } else {
            return true;
        }
    }

    # Checks if this value is excluded from queries
    private function is_excluded($value) {
        # Only exclude this field if it has an actual value set (e.g. not null)
        if ($value and $this->excluded) {
            return in_array($value, $this->excluded);
        }

        return false;
    }

    # Checks if our target data starts with a specific sub string
    private function startswith($value, $substr) {
        if (substr(strval($value), 0, strlen(strval($substr))) === strval($substr)) {
            return true;
        }
        return false;
    }

    # Checks if our target data ends with a specific sub string
    private function endswith($value, $substr) {
        if (substr(strval($value), -strlen(strval($substr))) === strval($substr)) {
            return true;
        }
        return false;
    }

    # Checks if our target data contains a specific sub string
    private function contains($value, $substr) {
        if (strpos(strval($value), strval($substr)) !== false) {
            return true;
        }
        return false;
    }

    # Checks if an integer or numeric string is less than a given value
    private function lt($value, $limit) {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) < floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    # Checks if an integer or numeric string is less than or equal to a given value
    private function lte($value, $limit) {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) <= floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    # Checks if an integer or numeric string is greater than a given value
    private function gt($value, $limit) {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) > floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    # Checks if an integer or numeric string is greater than or equal to a given value
    private function gte($value, $limit) {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) >= floatval($limit)) {
                return true;
            }
        }
        return false;
    }
}
