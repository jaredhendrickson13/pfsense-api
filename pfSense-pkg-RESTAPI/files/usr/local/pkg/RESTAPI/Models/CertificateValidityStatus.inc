<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\Base64Field;
use RESTAPI\Fields\StringField;
use RESTAPI\Fields\DateTimeField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\UIDField;
use RESTAPI\Responses\ForbiddenError;
use RESTAPI\Responses\ValidationError;
use RESTAPI\Validators\RegexValidator;
use RESTAPI\Validators\X509Validator;

/**
 * Defines a Model for interacting with the system's certificates.
 */
class CertificateValidityStatus extends Model {

    
    public DateTimeField $valid_from;
    public DateTimeField $valid_until;
    public IntegerField $valid_days_left;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # Set model attributes
        $this->parent_model_class = 'Certificate';
        $this->internal_callable = 'get_certificate_validity';
        $this->many = true;

        $this->valid_from = new DateTimeField(
            help_text: 'The start date from which this certificate is valid.',
        );
        $this->valid_until = new DateTimeField(
            help_text: 'The date until which this certificate is valid.',
        );
        $this->valid_days_left = new IntegerField(
            help_text: 'The number of days remaining until this certificate expires.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    public function get_certificate_validity(): array {
        $certificates = config_get_path('cert', []);
        foreach ($certificates as &$certificate) {
            
            list($from, $until) = cert_get_dates($certificate['crt'], true, false);

            //from returns DateTime Object ( [date] => 2026-01-15 16:04:55.000000 [timezone_type] => 3 [timezone] => Etc/UTC ) 

            $certificate['valid_from'] = $from->format('Y-m-d H:i:s');
            $certificate['valid_until'] = $until->format('Y-m-d H:i:s');
            $certificate['valid_days_left'] = cert_get_lifetime($certificate, true);
        }
        return $certificates;

    }
       
}
