<?php

namespace API\Core;

require_once("api/auto_loader.inc");

class ModelForm
{
    public string $url;
    public array $title_path = [];
    public array $tabs;
    public string $model_name = "";
    public string $custom_css = "";
    public string $custom_js = "";
    public array $sections = ["General" => ["fields" => [], "classes" => []]];
    private Model $model;

    /**
     * Construct the ModelFrom object
     */
    public function __construct() {
        $this->model = new $this->model_name();

        foreach ($this->title_path as &$path) {
            $path = gettext($path);
        }
    }

    /**
     * Populates the form sections with the desired fields and returns an array of
     * @return object The pfSense webConfigurator \Form object.
     */
    final public function get_form() : object {
        # Create the Form object to assign sections and fields to
        $form = new \Form();
        $inputs = $this->get_inputs_from_model_fields();

        # Loop through each of the requested sections populate them
        foreach ($this->sections as $section_name => $section_params) {
            # Create the section object
            $section = new \Form_Section($section_name);

            # Add any custom classes defined
            foreach ($section_params["classes"] as $section_class) {
                $section->addClass($section_class);
            }

            # Add any defined Fields to this section
            foreach ($section_params["fields"] as $section_field) {
                # Make this a password field if it is write only
                if ($this->model->$section_field->write_only) {
                    $section->addPassword($inputs[$section_field]);
                }
                # Otherwise, just add it as a normal input
                else {
                    $section->addInput($inputs[$section_field]);
                }
            }

            # Add this section to the form
            $form->add($section);
        }

        return $form;
    }

    final public function print_form() {
        global $config;
        $pgtitle = $this->title_path;
        include('head.inc');
        echo "<link rel='stylesheet' href='$this->custom_css'/>";
        echo "<script type='application/javascript' src='$this->custom_js'></script>";
        \display_top_tabs($this->tabs, true);
        print($this->get_form());
        include('foot.inc');
    }

    /**
     * Converts each of the assigned Model's Fields into pfSense form inputs.
     * @return array an array of pfSense Form Input objects.
     * @link https://github.com/pfsense/pfsense/tree/master/src/usr/local/www/classes/Form
     */
    final public function get_inputs_from_model_fields() : array {
        # Variables
        $inputs = [];

        # Loop through each of the Model's Fields and convert it to a pfSense form input
        foreach ($this->model->get_fields() as $field) {
            $inputs[$field] = $this->model->$field->to_form_input();
        }

        return $inputs;
    }
}
