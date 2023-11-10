<?php

namespace API\Forms;

require_once("api/auto_loader.inc");

use API\Core\Auth;
use API\Core\Form;
use API\Core\Response;
use API\Models\APIJWT;

class SystemAPIKey extends Form {
    public string $url = "/system_api_key_edit.php";
    public string $model_name = "\API\Models\APIKey";
    public array $title_path = ["System", "API", "Keys"];
    public string $custom_css = "/css/api/api.css";
    public array $tabs = [
        ["Settings", false, "/system_api_settings.php"], 
        ["Keys", true, "/system_api_key.php"],
        ["Updates", false, "/api/update/"],
        ["Documentation", false, "/api/documentation/"]
    ];
    public array $sections = [
        "Key Settings" => [
            "fields" => ["hash_algo", "length_bytes", "descr"]
        ]
    ];
    
    /**
     * Overwrite the on_save() method to include the generated API key.
     */
    public function on_save() {
        # Update settings with the newly provided values
        $representation_data = ($this->id) ? ["id" => $this->id] : [];
        $representation_data += $this->form_data_to_model_representation(form_data: $_POST, model: $this->model);
        $this->model->from_representation(representation_data: $representation_data);

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
