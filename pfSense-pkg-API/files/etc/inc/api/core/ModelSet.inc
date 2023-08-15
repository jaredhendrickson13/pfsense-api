<?php

namespace API\Core;

use API\Responses\ServerError;

require_once("api/auto_loader.inc");

/**
 * Creates a ModelSet object that acts as a container for multiple Model objects. This can be used to represent a
 * set of Model objects at once. Additionally, ModelSets can be further filtered using queries.
 */
class ModelSet {
    const QUERY_MODIFIERS = ["startswith", "endswith", "contains", "lt", "lte", "gt", "gte", "exact"];
    public array $model_objects;

    /**
     * Creates a ModelSet object that contains multiple Model objects.
     * @param array $model_objects An array of model objects to include in this model set
     */
    public function __construct(array $model_objects) {
        # Throw an error if any models is not a Model object
        foreach ($model_objects as $model_object) {
            if (get_parent_class($model_object) === "Model") {
                throw new ServerError(
                    message: "Model sets can only contain model objects.",
                    response_id: "MODEL_SET_CONTAINS_NON_MODEL"
                );
            }
        }

        # Assign the model objects to this model set
        $this->model_objects = $model_objects;
    }

    /**
     * Runs the to_representation() method for each model object within the model set and returns them all in an array.
     * @return array A serializable representation of all model objects within this model set.
     */
    public function to_representation() : array {
        return $this->run_model_method("to_representation");
    }

    /**
     * Runs a specified Model object method for each model object within this ModelSet.
     * @param string $method_name The name of the Model method to run for each model object in this model set.
     * @return array The return value of method call for each model object. Results are stored in the same order as
     * the model objects were passed in during construction.
     */
    private function run_model_method(string $method_name) : array {
        # Variables
        $results = [];

        # Loop through each model object and call its to_representation() method
        foreach ($this->model_objects as $model_object) {
            $results[] = $model_object->$method_name();
        }

        return $results;
    }

    /**
     * Filters the ModelSet to only include Model objects that match a specific query.
     * @param array $query_params An associative array of query targets and values. The array key will be the query
     * target and the array value will be the query value.
     * @param array $excluded An array of field names and namespaces to exclude from the query. This is helpful when
     * query data may have extra values that you do not want to include in the query.
     * @return ModelSet The filtered ModelSet object containing only Model objects that matched the query.
     */
    public function query(array $query_params=[], array $excluded=[]) : ModelSet {
        $queried_model_set = [];

        # Loop through each model object in the provided model set and check it against the query parameters
        foreach ($this->model_objects as $model_object) {
            # Assign variable to track matches
            $is_match = true;

            # Loop through each query parameter to determine if this object is a match
            foreach ($query_params as $query_target => $query_value) {
                # Loop variables
                $query_options = $this->get_query_options($query_target);
                $field = $query_options["field"];
                $namespace = $query_options["namespace"];
                $modifier = $query_options["modifier"];

                # Skip this field/namespace if it's excluded
                if (in_array($field, $excluded) or in_array($namespace, $excluded)) {
                    continue;
                }

                # Ensure this object has the requested field
                if (!property_exists($model_object, $field)) {
                    $is_match = false;
                    break;
                }

                # When a namespace was specified, ensure this field is a part of that namespace
                if ($namespace and $this->$field->namespace !== $namespace) {
                    $is_match = false;
                    break;
                }

                # Ensure the modifier matches
                if (!$this->$modifier($model_object->$field->value, $query_value)) {
                    $is_match = false;
                    break;
                }
            }
            # Add this model object to the filtered model set if it matched the query
            if ($is_match) {
                $queried_model_set[] = $model_object;
            }
        }
        return new ModelSet($queried_model_set);
    }

    /**
     * Checks for query target options. Query target options allow for more specific queries by limiting the scope to
     * a specific namespace and/or values that match a specific modifier.
     * @param string $query_target The query target string (e.g. `field_name`, `field_name__contains`)
     * @return array An array of query target options found in the $query_target string.
     */
    private function get_query_options(string $query_target) : array {
        # Variables
        $query_options = ["field"=>null, "namespace"=>null, "modifier"=>"exact"];

        # Check if the double-underscore indicator is present
        if (str_contains($query_target, "__")) {
            # Separate query target parameters by __ to determine query options
            $query_target = explode("__", $query_target,);

            # Check if a valid query modifier was specified
            if (in_array(end($query_target), self::QUERY_MODIFIERS)) {
                # Store the modifier and remove the last item of the array.
                $query_options["modifier"] = array_pop($query_target);
            }

            # Throw an error if there are still more than 2 query target items left
            if (count($query_target) > 2) {
                throw new ValidationError(
                    message: "Queries only support two levels of depth.",
                    response_id: "MODELSET_QUERY_TOO_DEEP"
                );
            }

            # Check if a namespace and field were specified
            if (count($query_target) == 2) {
                $query_options["namespace"] = $query_target[0];
                $query_options["field"] = $query_target[1];
            }
            # Otherwise, only the field was specified. Assign it.
            else {
                $query_options["field"] = $query_target[0];
            }
        }
        else {
            $query_options["field"] = $query_target;
        }

        return $query_options;
    }

    /**
     * Checks if a given value string starts with another substring.
     * @param mixed $value The object value to evaluate.
     * @param mixed $substr The substring to check for.
     * @return bool `true` if $value startswith $substr. `false` if it does not.
     */
    private static function startswith(mixed $value, mixed $substr) : bool {
        if (str_starts_with(strval($value), strval($substr))) {
            return true;
        }
        return false;
    }

    /**
     * Checks if a given value string ends with another substring.
     * @param mixed $value The object value to evaluate.
     * @param mixed $substr The substring to check for.
     * @return bool `true` if $value endswith $substr. `false` if it does not.
     */
    private static function endswith(mixed $value, mixed $substr) : bool {
        if (str_ends_with(strval($value), strval($substr))) {
            return true;
        }
        return false;
    }

    /**
     * Checks if a given value string contains with another substring.
     * @param mixed $value The object value to evaluate.
     * @param mixed $substr The substring to check for.
     * @return bool `true` if $value contains $substr. `false` if it does not.
     */
    private static function contains(mixed $value, mixed $substr) : bool {
        if (str_contains(strval($value), strval($substr))) {
            return true;
        }
        return false;
    }

    /**
     * Checks if two values are an exact match.
     * @param mixed $value1 The first value to evaluate.
     * @param mixed $value2 The second value to evaluate.
     * @return bool `true` if the $value1 and $value2 match exactly. `false` if they do not.
     */
    private static function exact(mixed $value1, mixed $value2) : bool {
        return ($value1 == $value2);
    }

    /**
     * Checks if a value is less than a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is less than.
     * @return bool `true` if $value is less than $limit. `false` if it is not.
     */
    private function lt(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) < floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if a value is less than or equal to a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is less than or equal to.
     * @return bool `true` if $value is less than or equal to $limit. `false` if it is not.
     */
    private function lte(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) <= floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if a value is greater than to a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is greater than.
     * @return bool `true` if $value is greater than $limit. `false` if it is not.
     */
    private function gt(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) > floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if a value is greater than or equal to a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is greater than or equal to.
     * @return bool `true` if $value is greater than or equal to $limit. `false` if it is not.
     */
    private function gte(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) >= floatval($limit)) {
                return true;
            }
        }
        return false;
    }
}
