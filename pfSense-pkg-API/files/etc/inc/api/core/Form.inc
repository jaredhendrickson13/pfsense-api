<?php

namespace API\Core;

use API\Responses\ServerError;

require_once("api/auto_loader.inc");

/**
 * Defines a Form object that can be used to dynamically generate custom pfSense webConfigurator pages for a specified
 * Model object.
 */
class Form {
    public string $url;
    public bool $many = false;
    public bool $editable = true;
    public bool $deletable = true;
    public string $many_sub_url = "";
    public mixed $id = null;
    public array $title_path = [];
    public array $tabs;
    public string $model_name = "";
    public string $custom_css = "";
    public string $custom_js = "";
    public array $sections = ["General" => ["fields" => [], "classes" => []]];
    public array $table_fields = [];
    public array $buttons = [];
    public array $form_data = [];
    protected Model $model;

    /**
     * Construct the Form object
     */
    public function __construct() {
        # Obtain submitted form data
        $this->form_data = $_POST;

        # Gather information about the authenticated user
        $client = new Auth();
        $client->username = $_SESSION["Username"];

        # Obtain the `id` from URL parameters
        $this->id = (is_numeric($_GET["id"])) ? intval($_GET["id"]) : null;

        # Create a new object for the assigned Model
        $this->model = new $this->model_name(id: $this->id, client: $client);

        foreach ($this->title_path as &$path) {
            $path = gettext($path);
        }
    }

    /**
     * Populates the form sections with the desired fields and returns an array of
     * @return \Form The pfSense webConfigurator \Form object.
     */
    final public function get_form() : \Form {
        # Create the Form object to assign sections and fields to
        $form = new \Form();

        # Loop through each of the requested sections populate them
        foreach ($this->sections as $section_name => $section_params) {
            # Create the section object
            $section = new \Form_Section($section_name);

            # Add any custom classes defined
            $section->addClass(implode(" ", $section_params["classes"] ?? []));

            # Add any defined Fields to this section
            foreach ($section_params["fields"] as $section_field) {
                # Make this a password field if it is write only
                if ($this->model->$section_field->write_only) {
                    $section->addPassword($this->model->$section_field->to_form_input());
                }
                # Otherwise, just add it as a normal input
                else {
                    $section->addInput($this->model->$section_field->to_form_input());
                }
            }

            # Add this section to the form
            $form->add($section);
        }

        # Add any defined buttons to the form globally
        foreach ($this->buttons as $button_name => $button_params) {
            $form->addGlobal(
                new \Form_Button(
                    name: $button_name,
                    title: ($button_params["title"]) ?: $button_name,
                    link: $button_params["link"],
                    icon: $button_params["icon"]
                )
            )->addClass(
                implode(
                    " ", $button_params["classes"] ?? []
                )
            )->setOnclick($button_params["on_click"]);
        }

        return $form;
    }

    /**
     * Obtains a PHP table that contains all current Model objects for `many` forms.
     */
    public function get_table() {
        # Setup the table structure
        $tb = "<div class='panel panel-default'>";
        $tb .= "<div class='panel-heading'><h2 class='panel-title'>".gettext($this->model->verbose_name_plural)."</h2></div>";
        $tb .= "<div class='panel-body table-responsive'>";
        $tb .= "<table class='table table-striped table-hover table-condensed table-rowdblclickedit sortable-theme-bootstrap' data-sortable>";
        $tb .= "<thead>";
        $tb .= "<tr>";
        
        # Include table header fields for each `table_field` requested.
        foreach ($this->table_fields as $table_field) {
            $tb .= "<th>".gettext(ucwords($this->model->$table_field->verbose_name))."</th>";
        }

        # Close the table header
        $tb .= "</tr>";
        $tb .= "</thead>";
        
        # Add table entries for each existing model object
        foreach ($this->model->read_all()->model_objects as $model_object) {
            # Add a row for this object
            $tb .= "<tr>";
            
            # Add table data for each `table_field` this object has
            foreach ($this->table_fields as $table_field) {
                # Determine the value of this field
                $value = $model_object->$table_field->value;
                
                # Use the choice verbose value if present
                if ($model_object->$table_field->verbose_choices and array_key_exists($value, $model_object->$table_field->verbose_choices)) {
                    $value = $model_object->$table_field->verbose_choices[$value];
                }
                
                $tb .= "<td>".$value."</td>";
            }
            
            # Add the edit and delete buttons to this row if allowed
            $tb .= "<td>";
            if ($this->editable) {
                $tb .= "<a class='fa fa-pencil' title='Edit ".gettext($model_object->verbose_name)."' href='".$this->many_sub_url."?id=".$model_object->id."'></a>";
            }
            if ($this->deletable) {
                $tb .= "<a class='fa fa-trash' title='Delete ".gettext($model_object->verbose_name)."' href='".$this->url."?act=del&amp;id=".$model_object->id."' usepost></a>";
            }
            $tb .= "</td>";
            $tb .= "</tr>";
        }
        
        # Close remaining tags
        $tb .= "</table>";
        $tb .= "</div>";
        $tb .= "</div>";
        # Place 'Add' button
        $tb .= "<nav class='action-buttons'><a href='".$this->many_sub_url."' class='btn btn-sm btn-success'><i class='fa fa-plus icon-embed-btn'></i>".gettext('Add')."</a></nav>";
        $tb .= "</div>";
        
        return $tb;
    }

    /**
     * Prints this form as a web page. This method is intended to be called by a PHP endpoint somewhere in the pfSense
     * web root.
     */
    final public function print_form() {
        # Print the static pfSense UI and include any custom CSS or JS files
        global $config;
        $pgtitle = $this->title_path;
        include('head.inc');
        echo "<link rel='stylesheet' href='$this->custom_css'/>";
        echo "<script type='application/javascript' src='$this->custom_js'></script>";
        \display_top_tabs($this->tabs, true);

        # Perform the save action if the save button was submitted
        if ($this->form_data["save"]) {
            $this->on_save();
        }
        
        # Delete the associated model object when requested
        if ($this->form_data["act"] === "del") {
            $this->on_delete($this->form_data["id"]);
        }

        # Check if a button press was submitted that is assigned a specific callable
        foreach ($this->buttons as $button_name => $button_params) {
            if (array_key_exists($button_name, $this->form_data) and $button_params["callable"]) {
                $callable = $button_params["callable"];
                $this->$callable();
            }
        }

        # Print the auto-generated form
        if ($this->many) {
            print($this->get_table());
        } else {
            print($this->get_form());
        }

        include('foot.inc');
    }

    /**
     * Performs validation and saves the Model assigned to this ModelForm.
     */
    public function on_save() {
        # Update settings with the newly provided values
        $representation_data = ($this->id) ? ["id" => $this->id] : [];
        $representation_data += $this->form_data_to_model_representation(form_data: $_POST, model: $this->model);
        $this->model->from_representation(data: $representation_data);

        # When an ID was found or the assigned model is not a `many` model, update the object. Otherwise create it.
        try {
            ($this->id or !$this->model->many) ? $this->model->update() : $this->model->create();
            $this->print_success_banner();
        }
        catch (Response $resp_error) {
            $error_message = $resp_error->getMessage();
            $this->print_input_error_banner(message: $error_message);

        }
    }

    /**
     * Tries to delete the Model object when requested.
     * @param int|string $id The ID of the Model object to delete.
     */
    public function on_delete(int|string $id) {
        # Convert the ID to an integer if it is numeric
        if (is_numeric($id)) {
            $id = (int)$id;
        }
        
        try {
            (new $this->model(id: $id))->delete();
            $this->print_success_banner("Deleted {$this->model->verbose_name} with ID $id.");
        }
        catch (Response $resp_error) {
            $error_message = $resp_error->getMessage();
            $this->print_input_error_banner(message: $error_message);
        }
    }

    /**
     * Converts POST form data submitted by the pfSense webConfigurator to a valid representation of a provided Model object
     * return array The POST form data in an acceptable representation for the provided Model object.
     * @param array $form_data The incoming $_POST or $_GET form data from the pfSense webConfigurator.
     * @param Model $model The Model object to represent.
     * @return array The representation of the Model object from the form data.
     */
    protected function form_data_to_model_representation(array $form_data, Model $model) : array {
        # Variables
        $representation = [];

        # Loop through each Field available to this Model
        foreach ($model->get_fields() as $field) {
            # Determine the class of this Field.
            $class = get_class($model->$field);

            # For boolean fields, assume the value is false if it does not match the true value exactly
            if ($class === "API\Fields\BooleanField" and $form_data[$field] !== $model->$field->indicates_true) {
                $form_data[$field] = $model->$field->indicates_false;
            }

            # Skip this field if it is not found in the form data
            if (!array_key_exists($field, $form_data)) {
                continue;
            }

            # If the value is an array, convert it to its internal value first
            if (is_array($form_data[$field])) {
                $form_data[$field] = implode($model->$field->delimiter, $form_data[$field]);
            }

            # Convert this value to its representation value from its internal value
            $model->$field->from_internal($form_data[$field]);
            $representation[$field] = $model->$field->value;
        }

        return $representation;
    }

    /**
     * Prints the green success banner at the top of the form page.
     * @param string $message Adds an extra message to the banner.
     */
    public function print_success_banner(string $message = "") {
        print_apply_result_box(retval: 0, extra_text: $message);

    }

    /**
     * Prints the input error banner at the top of the form page.
     * @param string $message Adds an extra message to the banner.
     */
    public function print_input_error_banner(string $message = "") {
        print_input_errors([$message]);
    }

    /**
     * Generate an endpoint for this Form in the pfSense webroot.
     */
    public function build_form_url() {
        # Get the fully qualified and non qualified class names for this object
        $fq_class_name = get_class($this);
        $nq_class_name = (new \ReflectionClass($this))->getShortName();

        # Specify the PHP code to write to the endpoints index.php file
        $code = "<?php
            require_once('api/forms/".$nq_class_name.".inc');
            require_once('guiconfig.inc');
            (new ".$fq_class_name."())->print_form();";

        # Assign the absolute path to the file. Assume index.php filename if not specified.
        $filename = "/usr/local/www/$this->url";
        $filename = (str_ends_with($filename, ".php")) ? $filename : "$filename/index.php";
        echo "Building ".$fq_class_name." at URL \"".$this->url."\"... ";
        mkdir(dirname($filename), 0755, true);
        file_put_contents($filename, $code);

        # Print success output if file now exists, otherwise output error and exit on non-zero code
        if (is_file($filename)) {
            echo "done.".PHP_EOL;
        }
        else {
            echo "failed.".PHP_EOL;
            exit(1);
        }
    }
}
