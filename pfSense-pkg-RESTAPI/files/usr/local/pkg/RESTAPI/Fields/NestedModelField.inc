<?php

namespace RESTAPI\Fields;

use RESTAPI\Core\Field;
use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\Response;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a Field object for storing nested Model objects in the pfSense configuration. This field allows certain
 * Model objects to be stored as field values for another Model object.
 */
class NestedModelField extends Field {
    public string $model_class;
    private string $model_class_short;
    public Model $model;
    private ModelSet $modelset;

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
        string $verbose_name = '',
        string $verbose_name_plural = '',
        array $conditions = [],
        string $help_text = '',
    ) {
        # Assign attributes specific to this Field
        $this->model_class = "\\RESTAPI\\Models\\$model_class";
        $this->model_class_short = $model_class;
        $this->model = new $this->model_class();
        $config_path = explode('/', $this->model->config_path);

        # Initialize the validation ModelSet. This is used to keep track of nested Model objects that have already
        # been validated.
        $this->modelset = new ModelSet([]);

        # Construct the parent Field object with desired parameters.
        parent::__construct(
            type: 'array',
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
            internal_namespace: implode('/', $config_path),
            conditions: $conditions,
            help_text: $help_text,
        );
    }

    /**
     * Validates the incoming value using the assigned Model's `validate()` method.
     * @throws ValidationError If any Response error occurred when validating the nested Model object
     */
    public function validate_extra(mixed $value): void {
        # Create a new Model object for this value and validate it. Remove any assigned IDs
        unset($value['id']);
        unset($value['parent_id']);

        # Construct the model without specifying IDs to prevent the object or its parent from being loaded
        $model = new $this->model_class(data: $value);

        # Assign the IDs and parent manually using this Field's context in case it does not yet exist in config
        $model->parent_id = $this->context->id;
        $model->parent_model = $this->context;
        $model->id = $this->modelset->count();

        # Try to validate the nested Model object
        try {
            $model->validate(skip_parent: true, modelset: $this->modelset);
            $this->modelset->model_objects[] = $model;
        } catch (Response $resp) {
            throw new ValidationError(
                message: "Field `$this->name` encountered a nested validation error: $resp->message",
                response_id: $resp->response_id,
            );
        }
    }

    /**
     * Extends the default Field validation method to clear the validation_modelset before starting validation
     * @param ModelSet|null $modelset
     * @return bool Returns true if the validation was successful.
     */
    public function validate(?ModelSet $modelset = null): bool {
        # Variables
        $return_value = true;

        # Only validate if the current value is different than our ModelSet's values
        if ($this->value !== $this->modelset->to_representation()) {
            # Empty any existing ModelSet entries in the event this is a subsequent `validate()` call.
            $this->modelset->model_objects = [];
            $return_value = parent::validate();
        }

        # Sort the nested Model objects before re-assigning the value
        $this->sort();

        # Overwrite the existing `value` with the representation of the validated model object
        $this->value = $this->many
            ? $this->modelset->to_representation()
            : $this->modelset->first()->to_representation();
        return $return_value;
    }

    /**
     * Converts the value from its representation value to its internal value.
     * @return array|string|null The internal value to be stored in the pfSense config
     */
    public function to_internal(): array|string|null {
        # Sort values before convert them to their internal form
        $this->sort();

        # For `many` Models, loop through each value and convert it to its internal value
        if ($this->model->many) {
            return $this->modelset->to_internal();
        }
        # Otherwise, just convert the one object
        else {;
            return $this->modelset->first()->to_internal();
        }
    }

    /**
     * Converts any nested Model objects from their internal values to their representation values.
     * @param string|null $internal_value The raw internal config value to convert to a representation value.
     */
    public function from_internal(mixed $internal_value): void {
        # Variables
        $representations = [];
        $model = new $this->model_class();
        $this->modelset->model_objects = [];

        # For `many` Models, loop through each value and convert it to its representation value
        if ($model->many) {
            foreach ($internal_value as $id => $internal_data) {
                $object = new $this->model_class();
                $object->id = $id;
                $object->parent_id = $this->context->id;
                $object->parent_model = $this->context;
                $object->from_internal_object($internal_data);
                $this->modelset->model_objects[] = $object;
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
    public function sort(): void {
        # Only sort this Field's values if this is a `many` field and it has a value
        if ($this->many and $this->value) {
            # Do not sort if there is no `sort_option` or `sort_by_field` set for this nested Model
            if (!$this->model->sort_option or !$this->model->sort_by_field) {
                return;
            }

            # Sort the nested Model objects by the specified field
            $this->modelset = $this->modelset->sort(
                field: $this->model->sort_by_field,
                order: $this->model->sort_option,
            );
        }
    }

    /**
     * Converts this NestedModelField object to a PHP array representation of an OpenAPI schema property configuration.
     * This is used when auto-generating API documentation.
     * @link https://swagger.io/docs/specification/data-models/
     * @return array A PHP array containing this field as an OpenAPI schema property configuration.
     */
    public function to_openapi_property(): array {
        # Variables
        $required_fields = $this->model->get_fields(required_only: true);

        # Assign primitive property attributes
        $openapi_property = [];
        $openapi_property['description'] = $this->help_text . '<br>' . $this->get_conditions_text();
        $openapi_property['nullable'] = $this->allow_null;
        $openapi_property['readOnly'] = $this->read_only;
        $openapi_property['writeOnly'] = $this->write_only;

        # Reference the nested Model schema as the items of this property for `many` Fields
        if ($this->many) {
            $openapi_property['type'] = $this->type;
            $openapi_property['minItems'] = $this->many_minimum;
            $openapi_property['maxItems'] = $this->many_maximum;
            $openapi_property['items'] = [
                'allOf' => [['$ref' => "#/components/schemas/$this->model_class_short"], ['type' => 'object']],
            ];
            if ($required_fields) {
                $openapi_property['items']['allOf'][1]['required'] = $required_fields;
            }
        } else {
            $openapi_property['allOf'] = [
                ['$ref' => "#/components/schemas/$this->model_class_short"],
                ['type' => 'object'],
            ];
            if ($required_fields) {
                $openapi_property['allOf']['required'] = $required_fields;
            }
        }

        return $openapi_property;
    }
}
