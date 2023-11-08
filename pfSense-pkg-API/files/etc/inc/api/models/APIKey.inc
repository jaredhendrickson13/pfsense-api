<?php

namespace API\Models;

require_once("api/auto_loader.inc");

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\IntegerField;
use API\Fields\StringField;

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
        
        # Set Model fields
        $this->name = new StringField(
            required: true,
            unique: true,
            editable: false,
            maximum_length: 128,
            help_text: "Sets a unique name for this API key. This is used to identify the key's purpose and cannot be ".
                "changed once created."
        );
        $this->username = new StringField(
            required: true,
            editable: false,
            help_text: "The username this API key is issued to."
        );
        $this->length_bytes = new IntegerField(
            default: 24,
            choices: [16, 24, 32, 64],
            editable: false,
            help_text: "The length of the API key (in bytes). Greater key lengths provide greater security, but ".
            "also increase the number of characters used in the key string."
        );
        $this->hash_algo = new StringField(
            default: "sha256",
            choices: ["sha256" => "SHA256", "sha384" => "SHA384", "sha512" => "SHA512"],
            editable: false,
            verbose_name: "key hashing algorithm",
            help_text: "The hash algorithm used for this API key. It is recommended to increase the strength of the ".
                "algorithm for keys assigned to privileged users."
        );
        $this->key = new StringField(
            allow_null: true,
            editable: false,
            representation_only: true,    # Don't store the actual API key internally
            help_text: "The real API key. This value is not stored internally and cannot be recovered if lost."
        );
        $this->hash = new StringField(
            allow_null: true,
            editable: false,
            write_only: true,    # Don't allow the hash to be read via API
            help_text: "The hash of the generated API key"
        );
        
        parent::__construct($id, $representation_data, $client);
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
