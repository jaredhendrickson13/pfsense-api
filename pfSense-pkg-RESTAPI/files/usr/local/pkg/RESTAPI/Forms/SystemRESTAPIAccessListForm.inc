<?php

namespace RESTAPI\Forms;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Form;

/**
 * Defines the Form for creating, editing, and deleting REST API Access List entries.
 */
class SystemRESTAPIAccessListForm extends Form {
    public string $url = '/system_restapi_acl.php';
    public string $many_sub_url = '/system_restapi_acl_entry_edit.php';
    public bool $many = true;
    public bool $editable = true;
    public string $model_class = 'RESTAPIAccessListEntry';
    public array $title_path = ['System', 'REST API', 'Access List'];
    public array $table_fields = ['type', 'weight', 'network', 'users', 'sched', 'descr'];
    public array $tabs = [
        ['Settings', false, '/system_restapi_settings.php'],
        ['Keys', false, '/system_restapi_key.php'],
        ['Access List', true, '/system_restapi_acl.php'],
        ['Updates', false, '/system_restapi_updates.php'],
        ['Documentation', false, '/api/v2/documentation'],
    ];
}
