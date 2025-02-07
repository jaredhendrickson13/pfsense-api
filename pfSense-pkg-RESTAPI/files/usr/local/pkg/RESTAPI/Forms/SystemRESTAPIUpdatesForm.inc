<?php

namespace RESTAPI\Forms;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Form;

/**
 * Defines the Form for viewing or applying REST API updates in the pfSense webConfigurator at /system_restapi_updates.php.
 */
class SystemRESTAPIUpdatesForm extends Form {
    public string $url = '/system_restapi_updates.php';
    public string $model_class = 'RESTAPIVersion';
    public array $title_path = ['System', 'REST API', 'Updates'];
    public array $tabs = [
        ['Settings', false, '/system_restapi_settings.php'],
        ['Keys', false, '/system_restapi_key.php'],
        ['Access List', false, '/system_restapi_acl.php'],
        ['Updates', true, '/system_restapi_updates.php'],
        ['Documentation', false, '/api/v2/documentation'],
    ];
    public array $sections = [
        'Version' => [
            'fields' => ['current_version', 'latest_version', 'latest_version_release_date'],
        ],
        'Install a different version' => [
            'fields' => ['install_version'],
        ],
    ];

    public function on_save(string $success_banner_msg = ''): void {
        parent::on_save(
            success_banner_msg: 'The requested version is being installed in the background. During the update, the ' .
                "REST API may be intermittently unavailable. Attempts to access the REST API's endpoints and web " .
                'pages during the update may result in errors until it completes. Check this page again later to see ' .
                'the status.',
        );
    }
}
