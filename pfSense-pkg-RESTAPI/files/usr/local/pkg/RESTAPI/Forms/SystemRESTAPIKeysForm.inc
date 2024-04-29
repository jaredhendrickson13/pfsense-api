<?php

namespace RESTAPI\Forms;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Form;
use RESTAPI\Models\RESTAPISettings;

class SystemRESTAPIKeysForm extends Form {
    public string $url = '/system_restapi_key.php';
    public string $many_sub_url = '/system_restapi_key_edit.php';
    public bool $many = true;
    public bool $editable = false;
    public string $model_class = 'RESTAPIKey';
    public array $title_path = ['System', 'REST API', 'Keys'];
    public array $table_fields = ['username', 'hash_algo', 'length_bytes', 'descr'];
    public array $tabs = [
        ['Settings', false, '/system_restapi_settings.php'],
        ['Keys', true, '/system_restapi_key.php'],
        ['Updates', false, '/system_restapi_updates.php'],
        ['Documentation', false, '/api/v2/documentation'],
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
}
