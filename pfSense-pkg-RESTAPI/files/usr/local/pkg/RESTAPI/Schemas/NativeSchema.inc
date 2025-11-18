<?php

namespace RESTAPI\Schemas;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;
use RESTAPI\Core\Field;
use RESTAPI\Core\Model;
use RESTAPI\Core\Schema;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Models\RESTAPIVersion;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

/**
 * Defines a builtin schema that provides all available metadata for the API. This schema is used to generate a file
 * containing all the metadata for endpoint, model and field definitions. It is intended to be used by custom
 * applications that need more context about the API than other schemas can provide.
 */
class NativeSchema extends Schema {
    /**
     * @var string $file_name
     * The file name for this schema.
     */
    public string $file_name = 'native.json';

    /**
     * Extracts all applicable metadata from a given field object
     */
    public function field_to_metadata(Field $field): array {
        return [
            'name' => $field->name,
            'class' => $field->get_class_shortname(),
            'verbose_name' => $field->verbose_name,
            'verbose_name_plural' => $field->verbose_name_plural,
            'type' => $field->type,
            'required' => $field->required,
            'default' => $field->default,
            'choices' => $field->choices,
            'unique' => $field->unique,
            'allow_empty' => $field->allow_empty,
            'allow_null' => $field->allow_null,
            'editable' => $field->editable,
            'sensitive' => $field->sensitive,
            'read_only' => $field->read_only,
            'write_only' => $field->write_only,
            'many' => $field->many,
            'many_minimum' => $field->many_minimum,
            'many_maximum' => $field->many_maximum,
            'minimum' => property_exists($field, 'minimum') ? $field->minimum : null,
            'maximum' => property_exists($field, 'maximum') ? $field->maximum : null,
            'minimum_length' => property_exists($field, 'minimum_length') ? $field->minimum_length : null,
            'maximum_length' => property_exists($field, 'maximum_length') ? $field->maximum_length : null,
            'internal_name' => $field->internal_name,
            'internal_namespace' => $field->internal_namespace,
            'referenced_by' => $field->referenced_by,
            'nested_model_class' => $field instanceof NestedModelField ? $field->model->get_class_shortname() : null,
            'foreign_model_class' => $field instanceof ForeignModelField ? $field->model_name : null,
            'foreign_model_field' => $field instanceof ForeignModelField ? $field->model_field : null,
            'conditions' => $field->conditions,
            'help_text' => $field->help_text,
        ];
    }

    /**
     * Extracts all applicable metadata from a given model object
     * @param Model $model The model object to extract metadata from
     * @return array An associative array containing metadata about the model.
     */
    public function model_to_metadata(Model $model): array {
        # Set base metadata
        $metadata = [
            'class' => $model->get_class_shortname(),
            'id_type' => $model->many ? $model->id_type : null,
            'parent_model_class' => $model->parent_model_class ?? null,
            'parent_id_type' => $model->parent_model_class ? $model->parent_id_type : null,
            'verbose_name' => $model->verbose_name,
            'verbose_name_plural' => $model->verbose_name_plural,
            'many' => $model->many,
            'many_minimum' => $model->many_minimum,
            'many_maximum' => $model->many_maximum,
            'packages' => $model->packages,
            'unique_together_fields' => $model->unique_together_fields,
            'always_apply' => $model->always_apply,
            'subsystem' => $model->subsystem,
            'fields' => [],
        ];

        # Convert each field to metadata
        foreach ($model->get_fields() as $field_name) {
            $metadata['fields'][$field_name] = $this->field_to_metadata($model->$field_name);
        }

        return $metadata;
    }

    /**
     * Extracts all applicable metadata from a given endpoint object
     * @param Endpoint $endpoint The endpoint object to extract metadata from
     * @return array An associative array containing metadata about the endpoint.
     */
    public function endpoint_to_metadata(Endpoint $endpoint): array {
        # Set base metadata
        return [
            'url' => $endpoint->url,
            'class' => $endpoint->get_class_shortname(),
            'model_class' => $endpoint->model_name,
            'tag' => $endpoint->tag,
            'deprecated' => $endpoint->deprecated,
            'many' => $endpoint->many,
            'request_method_options' => $endpoint->request_method_options,
            'requires_auth' => $endpoint->requires_auth,
            'auth_methods' => $endpoint->auth_methods,
            'get_privileges' => $endpoint->get_privileges,
            'post_privileges' => $endpoint->post_privileges,
            'patch_privileges' => $endpoint->patch_privileges,
            'put_privileges' => $endpoint->put_privileges,
            'delete_privileges' => $endpoint->delete_privileges,
            'get_help_text' => $endpoint->get_help_text,
            'post_help_text' => $endpoint->post_help_text,
            'patch_help_text' => $endpoint->patch_help_text,
            'put_help_text' => $endpoint->put_help_text,
            'delete_help_text' => $endpoint->delete_help_text,
        ];
    }

    /**
     * Obtains the full schema string for this Schema class in JSON format
     * @return string The full schema string for this Schema class in JSON format
     */
    public function get_schema_str(): string {
        $version = new RESTAPIVersion();
        $schema = ['version' => $version->current_version->value, 'endpoints' => [], 'models' => []];

        # Get all endpoint metadata
        foreach (get_classes_from_namespace('RESTAPI\Endpoints') as $endpoint_class) {
            $endpoint = new $endpoint_class();
            $schema['endpoints'][$endpoint->url] = $this->endpoint_to_metadata($endpoint);
        }

        # Get all model metadata
        foreach (get_classes_from_namespace('RESTAPI\Models') as $model_class) {
            $model = new $model_class(skip_init: true);
            $schema['models'][$model->get_class_shortname()] = $this->model_to_metadata($model);
        }

        return json_encode($schema);
    }
}
