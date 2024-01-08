<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Fields\StringField;

class CronJob extends Model
{
    public StringField $minute;
    public StringField $hour;
    public StringField $mday;
    public StringField $month;
    public StringField $wday;
    public StringField $who;
    public StringField $command;

    /**
     * @param mixed|null $id Specify the ID of a CronJob object to obtain. Leave as null to create a new one.
     * @param mixed $data Pass in representation data to populate field values during construction.
     * @param Auth|null $client Pass in a custom Auth object for this Model to reference
     * @param bool $async Apply changes to this Model asynchronously (unused)
     * @param bool $bypass_package Do not require the pfSense-pkg-Cron package to be installed to use this Model.
     * All the Cron components required for this Model to work are included in the base pfSense installation and
     * technically does not need the package to be installed. However, since the webConfigurator does not allow
     * Cron to be managed without the package installed, the API will do the same.
     */
    public function __construct(
        mixed $id = null, mixed $data = [], Auth $client = null, bool $async = true, bool $bypass_package = false
    )
    {
        # Set model attributes
        $this->config_path = "cron/item";
        $this->many = true;
        $this->packages = ($bypass_package) ? [] : ["pfSense-pkg-Cron"];
        parent::__construct(id: $id, data: $data, client: $client, async: $async);
    }

}