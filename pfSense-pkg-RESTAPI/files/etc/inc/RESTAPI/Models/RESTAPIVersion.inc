<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\StringField;

class RESTAPIVersion extends Model
{
    const RELEASES_API_URL = "https://api.github.com/repos/jaredhendrickson13/pfsense-api/releases";
    const RELEASES_API_MAX_TIME = 10;
    const RELEASES_CACHE_FILE = "/usr/local/share/pfSense-pkg-RESTAPI/releases.json";
    const RELEASES_CACHE_EXPIRATION_SECONDS = 60 * 60;    // Cache expires hourly
    
    public StringField $current_version;
    public StringField $latest_version;
    public StringField $latest_version_release_date;
    public BooleanField $update_available;
    public StringField $install_version;

    public function __construct(mixed $data = [], Auth $client = null)
    {
        # Set model attributes
        $this->internal_callable = "get_api_version_details";

        # Set model Fields
        $this->current_version = new StringField(
            read_only: true,
            help_text: "The current API version installed on this system."
        );
        $this->latest_version = new StringField(
            read_only: true,
            help_text: "The latest API version available to this system."
        );
        $this->latest_version_release_date = new StringField(
            read_only: true,
            verbose_name: "Release date",
            help_text: "The latest API version's release date."
        );
        $this->update_available = new BooleanField(
            read_only: true,
            help_text: "Indicates if an API update is available for this system."
        );
        $this->install_version = new StringField(
            choices_callable: "get_all_available_versions",
            allow_null: true,
            write_only: true,
            help_text: "Set the API version to update or rollback to."
        );

        parent::__construct(data: $data, client: $client);
    }

    /**
     * Obtains API version details. This method is the internal callable for this model.
     * @return array An array of API version details that correspond with this model's Fields.
     */
    protected function get_api_version_details() : array {
        return [
            "current_version" => $this->get_api_version(),
            "latest_version" => $this->get_latest_api_version(),
            "latest_version_release_date" => $this->get_latest_api_release_date(),
            "update_available" => $this->is_update_available(),
            "install_version" => array_keys($this->get_all_available_versions())
        ];
    }

    /**
     * Obtains the current API version installed on the system.
     * @return string The current API version installed in vX.X.X format
     */
    public static function get_api_version() : string {
        # Pull the raw pkg info for the API package into an array for each line
        $pkg_info = explode(PHP_EOL, shell_exec("pkg-static info pfSense-pkg-RESTAPI"));

        # Loop through each line and check the version
        foreach ($pkg_info as $pkg_line) {
            if (str_starts_with($pkg_line, "Version")) {
                # Locate the version and format it to a standard semantic version format (x.x.x)
                $version = str_replace(" ", "", $pkg_line);
                $version = explode(":", $version)[1];
                $version = (strlen($version) === 3) ? $version.".0" : $version;
                $version = str_replace("_", ".", $version);
                return "v$version";
            }
        }

        return "unknown";
    }

    /**
     * Obtains the latest API version available to this system.
     * @return string The latest API version available to this system.
     */
    public static function get_latest_api_version() : string {
        # Fetch our latest version and format it semantically (x.x.x)
        return array_key_first(self::get_all_available_versions());
    }

    /**
     * Obtains the release date of the latest API version available to this system.
     * @return string The release date of the latest API version available to this system.
     */
    public static function get_latest_api_release_date() : string {
        # Loop through each release and locate the latest available releases creation date
        foreach (self::get_github_releases()["releases"] as $release) {
            # Check if this releases is latest available for our platform
            if ($release["tag_name"] === self::get_latest_api_version()) {
                return $release["created_at"];
            }
        }

        return "unknown";
    }

    /**
     * Determines if an API update is available to this system.
     * @return bool `true` if there is an API update available to this system. `false` if it is up-to-date.
     */
    public static function is_update_available() : bool {
        return version_compare(self::get_api_version(), self::get_latest_api_version(), operator: "<");
    }

    /**
     * Obtains all releases for the pfsense-RESTAPI package via GitHub API. To avoid GitHub API rate limits, this
     * method also creates a local cache of obtained releases. All subsequent calls of this method will simply
     * return the cached releases. The cache can be renewed every 2 minutes by rerunning this method.
     * @return array An array of all releases from the pfsense-RESTAPI repository on GitHub via GitHub API.
     * @link https://docs.github.com/en/rest/releases/releases?apiVersion=2022-11-28#list-releases
     */
    public static function get_github_releases() : array {
        # Variables
        $releases = json_decode(file_get_contents(self::RELEASES_CACHE_FILE), true);
        $fetch_releases_cmd = "curl -s ".self::RELEASES_API_URL." -m ".self::RELEASES_API_MAX_TIME;
        $current_time = time();

        # Check if our previous releases data is older than 120 seconds
        if ($releases["last_updated"] + self::RELEASES_CACHE_EXPIRATION_SECONDS < $current_time) {
            # Pull the releases data from Github and rewrite the releases file
            $api_resp = shell_exec($fetch_releases_cmd);
            $api_resp = json_decode($api_resp, true);
            $releases = ["last_updated" => $current_time, "releases" => $api_resp];
            file_put_contents(self::RELEASES_CACHE_FILE, json_encode($releases));
        }
        return $releases;
    }

    public static function get_all_available_versions() {
        # Variables
        $pf_version = new SystemVersion();
        $api_settings = new RESTAPISettings();
        $versions = [];
        $count = 0;
        $releases = self::get_github_releases()["releases"];

        # Loop through each version and populate our version data
        foreach ($releases as $release) {
            # Loop through the assets of each release and check if our version of pfSense is supported
            foreach ($release["assets"] as $asset) {
                # Skip pre-releases if pre-releases are not allowed
                if ($release["prerelease"] and !$api_settings->allow_pre_releases->value) {
                    continue;
                }

                # Only include releases for this version of pfSense
                if (str_starts_with($asset["name"], "pfSense-" . $pf_version->base->value)) {
                    # The first item of our list is the latest release, mark it as such.
                    if ($count === 0) {
                        $versions[$release["tag_name"]] = $release["name"] . " - Latest";
                        $count++;
                    } else {
                        $versions[$release["tag_name"]] = $release["name"];
                    }
                }
            }
        }
        return $versions;
    }
}
