<?php

namespace RESTAPI\Forms;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Form;
use RESTAPI\Core\Response;
use RESTAPI\Models\RESTAPISettings;

/**
 * Defines the Form for creating and editing REST API Access List entries.
 */
class SystemRESTAPIAccessListEntryForm extends Form {
    public string $url = '/system_restapi_acl_entry_edit.php';
    public string $model_class = 'RESTAPIAccessListEntry';
    public array $title_path = ['System', 'REST API', 'Access List Entry'];
    public array $tabs = [
        ['Settings', false, '/system_restapi_settings.php'],
        ['Keys', false, '/system_restapi_key.php'],
        ['Access List', true, '/system_restapi_acl.php'],
        ['Updates', false, '/system_restapi_updates.php'],
        ['Documentation', false, '/api/v2/documentation'],
    ];
    public array $sections = [
        'General' => [
            'fields' => ['type', 'weight', 'network', 'users', 'sched', 'descr'],
        ],
    ];
}
