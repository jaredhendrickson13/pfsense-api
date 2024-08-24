<?php

namespace RESTAPI\Core\Globals;

# Define global constants
const HAL_REL_NAMESPACE = 'pfsense';
const RESERVED_FIELD_NAMES = [
    'id',
    'parent_id',
    '_links',
    'apply',
    'async',
    'limit',
    'offset',
    'extra',
    'placement',
    'append',
    'remove',
    'reverse',
];

/**
 * @global array $mock_internal_objects Allows TestCases to define a mock internal data array that will be used by Models
 * instead of the actual internal data array.
 */
$mock_internal_objects = null;
