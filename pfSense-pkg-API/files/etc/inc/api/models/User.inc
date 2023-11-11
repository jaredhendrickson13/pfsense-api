<?php

namespace api\models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\Base64Field;
use API\Fields\BooleanField;
use API\Fields\DateTimeField;
use API\Fields\IntegerField;
use API\Fields\StringField;
use API\Responses\ValidationError;
use api\validators\RegexValidator;

class User extends Model
{
    public StringField $username;
    public StringField $password;
    public IntegerField $uid;
    public StringField $scope;
    public StringField $priv;
    public BooleanField $disabled;
    public StringField $descr;
    public DateTimeField $expires;
    public StringField $cert;
    public Base64Field $authorizedkeys;
    public StringField $ipsecpsk;

    public function __construct(mixed $id = null, mixed $representation_data = [], Auth $client = null)
    {
        # Set model attributes
        $this->config_path = "system/user";
        $this->many = true;

        # Set model fields
        $this->username = new StringField(
            required: true,
            unique: true,
            maximum_length: 32,
            internal_name: "name",
            validators: [
                new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", error_msg: "Value contains invalid characters.")
            ],
            help_text: "The username of this local user."
        );
        $this->password = new StringField(
            required: true,
            write_only: true,
            internal_name: $this->get_config("system/webgui/pwhash", "bcrypt")."-hash",
            help_text: "The password of this local user."
        );
        $this->uid = new IntegerField(
            default: (int)$this->get_config("system/nextuid"),
            editable: false,
            read_only: true,
            help_text: "The UID of this local user. This value is automatically assigned and cannot be changed."
        );
        $this->scope = new StringField(
            default: "user",
            choices: ["user", "system"],
            editable: false,
            read_only: true,
            help_text: "The scope of this local user. This value is automatically assigned and cannot be changed."
        );
        $this->priv = new StringField(
            default: [],
            many: true,
            delimiter: null,
            help_text: "The privileges assigned to this local user."
        );
        $this->disabled = new BooleanField(
            default: false,
            help_text: "Disable this local user."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "The full descriptive name for this local user."
        );
        $this->expires = new DateTimeField(
            default: "",
            datetime_format: "m/d/Y",
            allow_empty: true,
            help_text: "The expiration date for this user in mm/dd/YYYY format. Use empty string for no ".
                "expiration"
        );
        $this->authorizedkeys = new Base64Field(
            default: "",
            allow_empty: true,
            help_text: "The SSH authorized keys to assign this user."
        );
        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds additional validation to the `username` field.
     * @param string $username The incoming value to be validated
     * @return string The validated value to be assigned.
     */
    public function validate_username(string $username) : string {
        # Do not allow users to use usernames that are reserved by the system
        if ($this->is_username_reserved($username)) {
            throw new ValidationError(
                message: "Field `username` cannot use system reserved username `$username`.",
                response_id: "USER_CANNOT_USE_RESERVED_USERNAME"
            );
        }

        return $username;
    }

    /**
     * Adds additional validation to the `password` field. This method is responsible for hashing the password.
     * @param string $password The incoming value to be validated
     * @return string The validated value to be assigned.
     */
    public function validate_password(string $password) : string {
        # Hash the password if the password has changed
        if ($this->initial_object->password->value !== $password) {
            $hash = [];
            local_user_set_password($hash, $password);
            return $hash[$this->password->internal_name];
        }

        return $password;
    }

    /**
     * Adds additional validation to the `priv` field.
     * @param string $priv The incoming value to be validated
     * @return string The validated value to be assigned.
     */
    public function validate_priv(string $priv) : string {
        global $priv_list;

        # Ensure requested privileges are valid privilege names
        if (!array_key_exists($priv, $priv_list)) {
            throw new ValidationError(
                message: "Field `priv` contains unknown privilege `$priv`",
                response_id: "USER_PRIV_IS_UNKNOWN"
            );
        }

        return $priv;
    }

    /**
     * Checks if a provided username is reserved by the system.
     * @param string $username The username to check.
     * @return bool `true` if the username is reserved, `false` if it is not.
     */
    public static function is_username_reserved(string $username) : bool {
        # Open the /etc/passwd file to read all system users
        $sys_users = explode(PHP_EOL, file_get_contents("/etc/passwd"));

        # Loop through each system user and check if the username is reserved
        foreach ($sys_users as $sys_user_ent) {
            $sys_username = explode(":", $sys_user_ent)[0];
            if ($sys_username == $username) {
                return true;
            }
        }
        return false;
    }
}
