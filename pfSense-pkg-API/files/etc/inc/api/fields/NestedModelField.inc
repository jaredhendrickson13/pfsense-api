<?php

namespace API\Fields;

use API\Core\Field;
use API\Core\Model;
use API\Core\Response;
use API\Responses\ValidationError;

class NestedModelField extends Field
{
    public string $model_class;
    public Model $model;
    
    /**
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
     * from being displayed in the representation data. This is ideal for potentially sensitive fields like passwords,
     * keys, and hashes.
     * @param string $verbose_name The detailed name for this Field. This name will be used in non-programmatic areas
     * like web pages and help text. This Field will default to property name assigned to the parent Model with
     * underscores converted to spaces.
     * @param string $verbose_name_plural The plural form of $verbose_name. This defaults to $verbose_name with `s`
     * suffixed or `es` suffixes to strings already ending with `s`.
     * @param array $conditions An array of conditions the field must meet to be included. This allows you to specify
     * conditions of other fields within the parent Model context. For example, if the parent Model context has two
     * fields, one field named `type` and the other being this field; and you only want this field to be included if
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
        $this->model_class = "\\API\\Models\\$model_class";
        
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
            verbose_name: $verbose_name,
            verbose_name_plural: $verbose_name_plural,
            conditions: $conditions, 
            help_text: $help_text
        );

        # Assign the internal name and namespace to match the nested model
        $this->model = new $this->model_class();
        $config_path = explode("/", $this->model->config_path);
        $this->internal_name = array_pop($config_path);
        $this->internal_namespace = implode("/", $config_path);
        $this->many = $this->model->many;
    }
    
    /**
     * Validates the incoming value using the assigned Model's `validate()` method.
     */
    public function validate_extra(mixed $value)
    {
        # Remove any existing objects nested under this model
        $full_config_path = "{$this->context->config_path}/{$this->context->id}/{$this->model->config_path}";
        $this->context->del_config($full_config_path);

        # Create a new Model object for this value and validate it
        $model = new $this->model_class(parent_id: $this->context->id, data: $value);

        # Try to validate the nested Model object
        try {
            $model->validate(skip_parent: true);
        }
        catch (Response $resp) {
            throw new ValidationError(
                message: "Field `$this->name` encountered a nested validation error: $resp->message",
                response_id: $resp->response_id
            );
        }

        return $value;
    }

    /**
     * Converts the value from its representation value to its internal value.
     * @return array|string|null The internal value to be stored in the pfSense config
     */
    public function to_internal(): array|string|null
    {
        # Create the nested Model object
        $model = new $this->model_class();

        # For `many` Models, loop through each value and convert it to its internal value
        if ($model->many) {
            $internal_objects = [];
            foreach ($this->value as $representation_data) {
                $representation_object = new $this->model_class(data: $representation_data);
                $internal_objects[] = $representation_object->to_internal();
            }
            return $internal_objects;
        }
        # Otherwise, just convert the one object
        else {
            $representation_object = new $this->model_class(data: $this->value);
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
                $object->from_internal_object($internal_data);
                $representation = ["id" => $id] + $object->to_representation();
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
     * Converts this Field object to a PHP array representation of an OpenAPI schema property configuration. This is
     * used when auto-generating API documentation. This method can be extended to add additional options to the OpenAPI
     * schema property.
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
        $openapi_property["editable"] = $this->editable;

        # Set the default if defined
        if ($this->default) {
            $openapi_property["default"] = $this->default;
        }

        # Set values that are unique to `many` Fields.
        if ($this->many) {
            $openapi_property["type"] = "array";
            $openapi_property["minItems"] = $this->many_minimum;
            $openapi_property["maxItems"] = $this->many_maximum;
            $openapi_property["items"] = ["type" => $this->type];

            # Assign choices if defined
            if ($this->choices) {
                $openapi_property["items"]["enum"] = $this->choices;
            }
        }
        else {
            $openapi_property["type"] = $this->type;

            # Assign choices if defined
            if ($this->choices and !$this->choices_callable) {
                $openapi_property["enum"] = $this->choices;
            }
        }

        return $openapi_property;
    }
}
