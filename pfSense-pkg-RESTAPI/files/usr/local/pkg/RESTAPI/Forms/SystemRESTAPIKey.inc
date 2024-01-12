<?php

namespace RESTAPI\Forms;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Form;
use RESTAPI\Core\Response;

class SystemRESTAPIKey extends Form {
    public string $url = "/system_restapi_key_edit.php";
    public string $model_name = "\RESTAPI\Models\RESTAPIKey";
    public array $title_path = ["System", "REST API", "Keys"];
    public array $tabs = [
        ["Settings", false, "/system_restapi_settings.php"],
        ["Keys", true, "/system_restapi_key.php"],
        ["Updates", false, "/system_restapi_updates.php"],
        ["Documentation", false, "/api/v2/documentationopenapi/"]
    ];
    public array $sections = [
        "Key Settings" => [
            "Fields" => ["hash_algo", "length_bytes", "descr"]
        ]
    ];
    
    /**
     * Overwrite the on_save() method to include the generated API key.
     */
    public function on_save() {
        # Update settings with the newly provided values
        $representation_data = ($this->id) ? ["id" => $this->id] : [];
        $representation_data += $this->form_data_to_model_representation(form_data: $_POST, model: $this->model);
        $this->model->from_representation(data: $representation_data);

        # When an ID was found or the assigned model is not a `many` model, update the object. Otherwise create it.
        try {
            ($this->id or !$this->model->many) ? $this->model->update() : $this->model->create();
        }
        catch (Response $resp_error) {
            $error_message = $resp_error->getMessage();
            $this->print_input_error_banner(message: $error_message);
            return;
        }
        
        # Print the real API key when the object is initially created.
        if ($this->model->key->value) {
            $this->print_success_banner(
                message: "Store this API key in a secure location. Once you navigate away from this page, this key ".
                    "cannot be recovered!<br><br>Your API key is: <code>{$this->model->key->value}</code>"
            );
        }
        
        $this->model = new $this->model();
    }
}
