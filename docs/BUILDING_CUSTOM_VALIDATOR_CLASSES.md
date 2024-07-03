# Building Custom Validator Classes

Validator classes are reusable components that are used to quickly validate Field object values assigned to Model classes.
Simply assign Validator objects to the Field's `validators` property to ensure that the Field's value meets the specified
validation criteria defined in the Validator class. A full list of available Validator classes can be found in the
[PHP reference](https://pfrest.org/php-docs/namespaces/restapi-validators.html). This document will guide you through the
process of building a custom Validator class as well as options for interacting with the Validator.

!!! Tip
    Validators should only be created for general validations that apply to more than one Field object across multiple Model classes.
    If your validation logic is specific to a single Field object or Model class, consider adding the validation logic to the
    [Model class's `validate_FIELD_NAME()` or `validate_extra()`](BUILDING_CUSTOM_MODEL_CLASSES.md#adding-extra-validation) 
    methods instead.

## Getting Started

Use the following class template to initialize your custom Validator class:

```php
<?php

namespace RESTAPI\Validators;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Validator;

/**
TODO: Add a description of your Validator class here.
*/
class MyCustomValidator extends Validator {

}
```

!!! Important
    Be sure to place this class file within `/usr/local/pkg/RESTAPI/Validators/` directory and name the file after your
    Validator class name with a `.inc` extension.

## Implementing the validate() Method

The `validate()` method is used to validate the Field object value. This method is required and should typically throw a
[\RESTAPI\Responses\ValidationError](https://pfrest.org/php-docs/classes/RESTAPI-Responses-ValidationError.html) exception if the
Field object value does not meet the specified validation criteria, or any other [Response class](https://pfrest.org/php-docs/namespaces/restapi-responses.html)
that is relevant to the validation error.

```php
/**
 * Checks if a given value is valid.
 * @param mixed $value The value to validate.
 * @param string $field_name The field name of the value being validated. This is used for error messages.
 * @throws \RESTAPI\Responses\ValidationError When the value is not valid.
 */
public function validate(mixed $value, string $field_name = '') {
    # Add your validation logic here
    if ($value !== "a valid value") {
        throw new ValidationError(
            message: "Field `$field_name` must be a valid $allowed_options_str, received `$value`.",
            response_id: 'IP_ADDRESS_VALIDATOR_FAILED',
        );
    }
}
```

!!! Notes
    - In the case that the Field object this Validator is assigned to is `many` enabled, each value in the array will be
      validated individually. The full array will not be passed into the `validate()` method.
    - Null values will never be passed into the `validate()` method as null values require the Field's `allow_null` property to be set to `true`,
      which bypasses the validation process entirely.

## Defining Custom Validation Parameters

While not required, you can define the `__construct()` method to initialize the Validator class with any
custom properties or parameters that control the behavior of the Validator. For example, an IPAddressValidator class may
have an `allow_ipv6` parameter/property that can optionally be set to `true` to allow IPv6 addresses during validation.

```php
/**
 * Initialize the Validator class with custom properties.
 * @param bool $allow_ipv6 Whether to allow IPv6 addresses during validation.
 */
<?php

namespace RESTAPI\Validators;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Validator;

/**
TODO: Add a description of your Validator class here.
*/
class MyCustomValidator extends Validator {
    public bool $allow_ipv6;

    /**
    * Initialize the Validator class with custom properties.
    * @param bool $allow_ipv6 Whether to allow IPv6 addresses during validation.
    */
    public function __construct(bool $allow_ipv6 = false) {
        $this->allow_ipv6 = $allow_ipv6;
    }
    
        
    /**
    * Validate the Field object value.
     * @param mixed $value The Field object value to validate.
     * @param string $field_name The name of the Field object being validated.
     */
    public function validate(mixed $value, string $field_name = ''): void {
        if ($this->allow_ipv6) {
            // Validate IPv6 address
        } else {
            // Validate IPv4 address
        }
    }
```

## Assigning Labels

Labels are used to describe the value of the Field object value that has been validated, or more specifically, what specific validation
caused the Validator to accept that value. For example, an IPAddressValidator class may have a `label` property that sets
the `is_ipaddrv4` label when the Field object value is a valid IPv4 address, or the `is_ipaddrv6` label when the Field
object value is a valid IPv6 address. Multiple labels can be assigned at once as well. This allows you to easily 
identify validations that have already been performed on the Field object value, thus preventing redundant validations 
in the [Model's validate_*() methods](BUILDING_CUSTOM_MODEL_CLASSES.md#adding-extra-validation).

Labels can be any arbitrary value, but a few common labels are:

- `is_ipaddrv4`: The Field object value is a valid IPv4 address.
- `is_ipaddrv6`: The Field object value is a valid IPv6 address.
- `is_ipaddr`: The Field object value is a valid IPv4 or IPv6 address.
- `is_fqdn`: The Field object value is a valid fully-qualified domain name.
- `is_alias`: The Field object value is a valid firewall alias name.
- `is_subnetv4`: The Field object value is a valid IPv4 subnet.
- `is_subnetv6`: The Field object value is a valid IPv6 subnet.

You can assign a label by calling the `set_label()` method in the `validate()` method:

```php
/**
 * Validate the Field object value.
 * @param mixed $value The Field object value to validate.
 * @param string $field_name The name of the Field object being validated.
 */
public function validate(mixed $value, string $field_name = ''): void {
    if ($this-is_ipaddrv4($value)) {
        $this->set_label('is_ipaddr');
        $this->set_label('is_ipaddrv4');
    } elseif ($this-is_ipaddrv6($value)) {
        $this->set_label('is_ipaddr');
        $this->set_label('is_ipaddrv6');
    }
    else {
        throw new ValidationError(
            message: "Field `$field_name` must be a valid IP address, received `$value`.",
            response_id: 'IP_ADDRESS_VALIDATOR_FAILED',
        );
    }
```

Then, from the Model class, you can check if a specific label has been assigned to the Field object value by calling
the Field's `has_label()` method:

```php
# Returns `true` if the Field object value has the `is_ipaddrv4` label assigned to it currently.
$this->field_name->has_label('is_ipaddrv4');
```

!!! Important
    - Ensure your labels are well documented in the Validator class's PHPDoc block.
    - The Field object's `has_label()` method should only be called from the Model class's `validate_FIELD_NAME()` and `validate_extra()` methods.
    - Labels are automatically reset whenever the Model's `validate()` method is called.

## Obtaining the Referencing Field and Model Objects

In the event that you need to access the entire Field object that is being validated, or the Model object that Field
object belongs to, you can use the `$this->field_context` and `$this->model_context` properties, respectively.
