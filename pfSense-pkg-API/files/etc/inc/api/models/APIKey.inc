<?php

namespace API\Models;

require_once("api/auto_loader.inc");

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\IntegerField;
use API\Fields\StringField;
use API\Responses\ValidationError;

class APIKey extends Model
{
    public StringField $name;
    public StringField $username;
    public StringField $hash_algo;
    public IntegerField $length_bytes;
    public StringField $hash;
    public StringField $key;
    
    public function __construct(mixed $id = null, mixed $representation_data = [], Auth $client = null)
    {
        # Gather information about current API settings
        $api_pkg_id = APISettings::get_api_config()["id"];

        # Set Model attributes
        $this->config_path = "installedpackages/package/$api_pkg_id/conf/keys/key";
        $this->many = true;
        $this->verbose_name = "API Key";
        
        # Set Model fields
        $this->name = new StringField(
            required: true,
            unique: true,
            maximum_length: 128,
            help_text: "Sets a unique name for this API key. This is used to identify the key's purpose and cannot be ".
                "changed once created."
        );
        $this->username = new StringField(
            required: true,
            help_text: "The username this API key is issued to."
        );
        $this->length_bytes = new IntegerField(
            default: 24,
            choices: [16, 24, 32, 64],
            help_text: "The length of the API key (in bytes). Greater key lengths provide greater security, but ".
            "also increase the number of characters used in the key string."
        );
        $this->hash_algo = new StringField(
            default: "sha256",
            choices: ["sha256" => "SHA256", "sha384" => "SHA384", "sha512" => "SHA512"],
            verbose_name: "key hashing algorithm",
            help_text: "The hash algorithm used for this API key. It is recommended to increase the strength of the ".
                "algorithm for keys assigned to privileged users."
        );
        $this->key = new StringField(
            allow_null: true,
            representation_only: true,    # Don't store the actual API key internally
            help_text: "The real API key. This value is not stored internally and cannot be recovered if lost."
        );
        $this->hash = new StringField(
            allow_null: true,
            write_only: true,    # Don't allow the hash to be read via API
            help_text: "The hash of the generated API key"
        );
        
        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds custom validation to the `username` field.
     * @param string $username The incoming username value to be validated.
     * @return string The validated username value to be assigned.
     */
    public function validate_username(string $username) : string {
        # Require this username to match an existing user
        # TODO: Replace this loop with a User model query when developed
        foreach ($this->get_config("system/user", []) as $user) {
            if ($username === $user["name"]) {
                return $username;
            }
        }

        throw new ValidationError(
            message: "Field `username` must be an existing user's username, received `$username`",
            response_id: "API_KEY_USERNAME_MUST_EXIST"
        );
    }

    /**
     * Generates the API key and hash values and saves the Model to config.
     */
    public function _create()
    {
        # Generate the API key and its hash
        $this->key->value = bin2hex(random_bytes($this->length_bytes->value));
        $this->hash->value = hash($this->hash_algo->value, $this->key->value);
        
        # Call the parent _create() method to save the Model object to config
        parent::_create();
    }

    /**
     * Override the default _update() method to prevent APIKeys from being updated.
     */
    public function _update() {
        # Do not allow APIKey objects to be updated.
        throw new API\Responses\ServerError(
            message: "APIKey model objects cannot be updated.",
            response_id: "API_KEY_CANNOT_BE_UPDATED"
        );
    }

    /**
     * Attempts to authenticate an API key against this Model object.
     * @param string $key The real API key to authenticate with.
     * @return bool true if the hashed $key matches this Model object's `hash` property, otherwise false.
     */
    public function authenticate(string $key) : bool {
        # Hash the incoming key according to the algorithm of this stored key
        $hash = hash($this->hash_algo->value, $key);

        # Authentication is successful when incoming key matches the hashed key stored in config
        if ($hash === $this->hash->value) {
            return true;
        }

        return false;
    }
}
