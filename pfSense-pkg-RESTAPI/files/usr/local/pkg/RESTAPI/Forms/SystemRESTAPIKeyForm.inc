<?php

namespace RESTAPI\Forms;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Form;
use RESTAPI\Core\Response;
use RESTAPI\Models\RESTAPISettings;

class SystemRESTAPIKeyForm extends Form {
    public string $url = '/system_restapi_key_edit.php';
    public string $model_class = 'RESTAPIKey';
    public array $title_path = ['System', 'REST API', 'Keys'];
    public array $tabs = [
        ['Settings', false, '/system_restapi_settings.php'],
        ['Keys', true, '/system_restapi_key.php'],
        ['Updates', false, '/system_restapi_updates.php'],
        ['Documentation', false, '/api/v2/documentation/'],
    ];
    public array $sections = [
        'Key Settings' => [
            'fields' => ['hash_algo', 'length_bytes', 'descr'],
        ],
    ];

    /**
     * Prints a warning banner when the page loads if this Form is accessed when KeyAuth is not enabled.
     */
    public function on_load(): void {
        # Print a warning at the top of the page if the user is accessing the page when KeyAuth is not enabled
        $api_settings = new RESTAPISettings();
        if (!in_array('KeyAuth', $api_settings->auth_methods->value)) {
            $this->print_info_banner(
                message: 'Key authentication is not currently enabled. You may create keys, but they cannot be used ' .
                    "until Key authentication is enabled on the <a href='/system_restapi_settings.php'>REST API > " .
                    'Settings</a> tab.',
            );
        }
    }

    /**
     * Overwrite the on_save() method to include the generated API key.
     */
    public function on_save(): void {
        # Update settings with the newly provided values
        $representation_data = $this->id ? ['id' => $this->id] : [];
        $representation_data += $this->form_data_to_model_representation(form_data: $_POST, model: $this->model);
        $this->model->from_representation(data: $representation_data);

        # When an ID was found or the assigned model is not a `many` model, update the object. Otherwise create it.
        try {
            ($this->id or !$this->model->many) ? $this->model->update() : $this->model->create();
        } catch (Response $resp_error) {
            $error_message = $resp_error->getMessage();
            $this->print_input_error_banner(message: $error_message);
            return;
        }

        # Print the real API key when the object is initially created.
        if ($this->model->key->value) {
            $this->print_success_banner(
                message: 'Store this API key in a secure location. Once you navigate away from this page, this key ' .
                    "cannot be recovered!<br><br>Your API key is: <code>{$this->model->key->value}</code>",
            );
        }

        $this->model = new $this->model();
    }
}
