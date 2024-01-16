<?php

namespace RESTAPI\Fields;

use RESTAPI\Core\Field;
use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\Response;
use RESTAPI\Responses\ServerError;
use RESTAPI\Responses\ValidationError;

class NestedModelField extends Field
{
    public string $model_class;
    private string $model_class_short;
    public Model $model;
    private ModelSet $validation_modelset;

    /**
     * @param string $model_class The Model class that is nested within this Field.
     * @param bool $required Set to `true` if this field is required to be explicitly set. $required and $default are
     * mutually exclusive.
     * @param mixed|null $default The default value to assign this field if no value was found. If not set, it assumed
     * this Field requires a value and should fail validation if no value was found.
     * @param bool $allow_empty If `true`, empty strings will be allowed by this field.
     * @param bool $allow_null Set to `true` if `null` values should be supported for this field. When `null` values
     * are supported, the underlying pfSense configuration field will be removed entirely when receiving a null value.
     * @param bool $editable Set to `false` to prevent this field's value from being changed after its initial creation.
     * @param bool $read_only Set to `true` to make this field read-only. Attempts to write to a read-only field will
     * result in a thrown exception.
     * @param bool $write_only Set to `true` to make this field write-only. This will prevent the field's current value
     * from being displayed in the representation data. This is ideal for potentially sensitive Fields like passwords,
     * keys, and hashes.
     * @param string $verbose_name The detailed name for this Field. This name will be used in non-programmatic areas
     * like web pages and help text. This Field will default to property name assigned to the parent Model with
     * underscores converted to spaces.
     * @param string $verbose_name_plural The plural form of $verbose_name. This defaults to $verbose_name with `s`
     * suffixed or `es` suffixes to strings already ending with `s`.
     * @param array $conditions An array of conditions the field must meet to be included. This allows you to specify
     * conditions of other Fields within the parent Model context. For example, if the parent Model context has two
     * Fields, one field named `type` and the other being this field; and you only want this field to be included if
     * `type` is equal to `type1`, you could assign ["type" => "type1"] to this parameter.
     * @param string $help_text Sets a description for this field. This description will be used as the description
     * for this field in the API documentation.
     */
    public function __construct(
        string $model_class,
        bool $required = false,
        mixed $default = null,
        bool $allow_empty = false, 
        bool $allow_null = false, 
        bool $editable = true, 
        bool $read_only = false, 
        bool $write_only = false, 
        string $verbose_name = "",
        string $verbose_name_plural = "",
        array $conditions = [], 
        string $help_text = ""
    )
    {
        # Assign attributes specific to this Field
        $this->model_class = "\\RESTAPI\\Models\\$model_class";
        $this->model_class_short = $model_class;
        $this->model = new $this->model_class();
        $config_path = explode("/", $this->model->config_path);

        # Initialize the validation ModelSet. This is used to keep track of nested Model objects that have already
        # been validated.
        $this->validation_modelset = new ModelSet([]);

        # Construct the parent Field object with desired parameters.
        parent::__construct(
            type: "array",
            required: $required,
            default: $default,
            allow_empty: $allow_empty,
            allow_null: $allow_null,
            editable: $editable,
            read_only: $read_only,
            write_only: $write_only,
            many: $this->model->many,
            many_minimum: $this->model->many_minimum,
            many_maximum: $this->model->many_maximum,
            verbose_name: $verbose_name,
            verbose_name_plural: $verbose_name_plural,
            internal_name: array_pop($config_path),
            internal_namespace: implode("/", $config_path),
            conditions: $conditions,
            help_text: $help_text
        );
    }
    
    /**
     * Validates the incoming value using the assigned Model's `validate()` method.
     * @throws ValidationError If any Response error occurred when validating the nested Model object
     */
    public function validate_extra(mixed $value)
    {
        # Use a variable to keep track of 
        
        # Create a new Model object for this value and validate it. Remove any assigned IDs
        unset($value["id"]);
        unset($value["parent_id"]);
        $model = new $this->model_class(data: $value);

        # Try to validate the nested Model object
        try {
            $model->validate(skip_parent: true);
            $this->check_unique_fields($model);
            $this->check_unique_together_fields($model);
            $this->validation_modelset->model_objects[] = $model;
        }
        catch (Response $resp) {
            throw new ValidationError(
                message: "Field `$this->name` encountered a nested validation error: $resp->message",
                response_id: $resp->response_id
            );
        }

        return $model;
    }

    /**
     * Extends the default Field validation method to clear the validation_modelset before starting validation
     */
    public function validate(): bool
    {
        # Empty any existing ModelSet entries in the event this is a subsequent `validate()` call.
        $this->validation_modelset->model_objects = [];
        $return_value = parent::validate();
        
        # Overwrite the existing `value` with the representation of the validated model object
        $this->value = ($this->many) ? $this->validation_modelset->to_representation() : $this->validation_modelset->first()->to_representation();
        return $return_value;
    }

    /**
     * Checks that any of the nested Model's `unique` Fields are unique from any other value being assigned.
     * @param Model $model The Model to check Fields for uniqueness.
     * @throws ValidationError When a `unique` field value is not unique
     */
    private function check_unique_fields(Model $model) {
        # Only validate `unique` if this is a `many` model
        if ($model->many) {
            # Loop through each of this Model's Fields and check for `unique` Fields.
            foreach ($model->get_fields() as $field) {
                if ($model->$field->unique) {
                    # Query for other validated Models with the same field value
                    $model_query = $this->validation_modelset->query([$field => $model->$field->value]);
                    if ($model_query->exists()) {
                        throw new ValidationError(
                            message: "Field '$field' must be unique.",
                            response_id: "MODEL_FIELD_MUST_BE_UNIQUE"
                        );
                    }
                }
            }
        }
    }

    /**
     * Checks that any Fields which must be unique together are unique from all other existing objects for this Model.
     * @throws ServerError When the Fields specified in `unique_together_fields` are not Field objects known to this
     * Model.
     * @throws ValidationError When the unique together Fields are not unique and are present in another Model object.
     */
    private function check_unique_together_fields(Model $model) {
        # Skip this validation for non `many` Models or Models without unique together Fields
        if (!$model->many or !$model->unique_together_fields) {
            return;
        }

        # Use this variable to keep track of query parameters to use when checking uniqueness
        $query_params = [];

        # Format the query parameters as an associative array
        foreach ($model->unique_together_fields as $field) {
            $query_params[$field] = $model->$field->value;
        }

        # Query for existing objects with the same Field values
        $model_query = $this->validation_modelset->query(query_params: $query_params);

        # If any existing Models were found matching the query, Fields are not unique together, throw an error
        if ($model_query->exists()) {
            $unique_fields_string = implode(", ", $model->unique_together_fields);
            throw new ValidationError(
                message: "Fields [$unique_fields_string] together must be unique from all other objects of its kind.",
                response_id: "MODEL_FIELDS_NOT_UNIQUE_TOGETHER"
            );
        }
    }

    /**
     * Converts the value from its representation value to its internal value.
     * @return array|string|null The internal value to be stored in the pfSense config
     */
    public function to_internal(): array|string|null
    {
        # Sort values before convert them to their internal form
        $this->sort();

        # For `many` Models, loop through each value and convert it to its internal value
        if ($this->model->many) {
            $internal_objects = [];
            foreach ($this->value as $representation_data) {
                # Remove any assigned IDs
                unset($representation_data["id"]);
                unset($representation_data["parent_id"]);
                $representation_object = new $this->model(data: $representation_data);
                $internal_objects[] = $representation_object->to_internal();
            }
            return $internal_objects;
        }
        # Otherwise, just convert the one object
        else {
            $representation_object = new $this->model(data: $this->value);
            return $representation_object->to_internal();
        }
    }

    /**
     * Converts any nested Model objects from their internal values to their representation values.
     * @param string|null $internal_value The raw internal config value to convert to a representation value.
     */
    public function from_internal(mixed $internal_value) {
        # Create the nested Model object
        $model = new $this->model_class();

        # For `many` Models, loop through each value and convert it to its representation value
        if ($model->many) {
            $representations = [];
            foreach ($internal_value as $id => $internal_data) {
                $object = new $this->model_class();
                $object->id = $id;
                $object->parent_id = $this->context->id;
                $object->parent_model = $this->context;
                $object->from_internal_object($internal_data);
                $representation = $object->to_representation();
                $representations[] = $representation;
            }
            $this->value = $representations;
        }
        # Otherwise, just convert the one object
        else {
            $object = new $this->model_class();
            $this->value = $object->to_representation();
        }
    }

    /**
     * Sorts the nested Model data use the nested Model's `sort_option` and `sort_by_field` setting. This method
     * closely resembles the Model->sort() method, but does not require values to be set in config beforehand. 
     */
    public function sort() {
        # Only sort this Field's values if this is a `many` field and it has a value
        if ($this->many and $this->value) {
            # Do not sort if there is no `sort_option` or `sort_by_field` set for this nested Model
            if (!$this->model->sort_option or !$this->model->sort_by_field) {
                return;
            }

            # Loop through each rule and map its sort field value to our sort criteria array
            foreach ($this->value as $id => $object) {
                # Map the real field if it's not empty, otherwise assume an empty string
                if (!empty($object[$this->model->sort_by_field])) {
                    $criteria[$id] = $object[$this->model->sort_by_field];
                } else {
                    $criteria[$id] = "";
                }
            }

            # Sort the internal objects using the previously determined criteria
            array_multisort($criteria, $this->model->sort_option, $this->value);

            $this->value = array_values($this->value);
        }
    }

    /**
     * Converts this NestedModelField object to a PHP array representation of an OpenAPI schema property configuration. 
     * This is used when auto-generating API documentation.
     * @link https://swagger.io/docs/specification/data-models/
     * @return array A PHP array containing this field as an OpenAPI schema property configuration.
     */
    public function to_openapi_property() : array {
        # Assign primitive property attributes
        $openapi_property = [];
        $openapi_property["description"] = $this->help_text."<br>".$this->get_conditions_text();
        $openapi_property["nullable"] = $this->allow_null;
        $openapi_property["readOnly"] = $this->read_only;
        $openapi_property["writeOnly"] = $this->write_only;

        # Reference the nested Model schema as the items of this property for `many` Fields
        if ($this->many) {
            $openapi_property["type"] = $this->type;
            $openapi_property["minItems"] = $this->many_minimum;
            $openapi_property["maxItems"] = $this->many_maximum;
            $openapi_property["items"] = [
                "allOf" => [
                    ['$ref' => "#/components/schemas/$this->model_class_short"],
                    ["type" => "object", "required" => $this->model->get_fields(required_only: true)]
                ]
            ];
        }
        else {
            $openapi_property["allOf"] = [
                ['$ref' => "#/components/schemas/$this->model_class_short"],
                ["type" => "object", "required" => $this->model->get_fields(required_only: true)]
            ];
        }

        return $openapi_property;
    }
}
