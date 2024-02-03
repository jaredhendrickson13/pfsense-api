<?php

namespace RESTAPI\Core;

use ReflectionClass;

require_once 'RESTAPI/autoloader.inc';

/**
 * Defines a Form object that can be used to dynamically generate custom pfSense webConfigurator pages for a specified
 * Model object.
 */
class Form
{
    use BaseTraits;

    public string $url;
    public bool $many = false;
    public bool $editable = true;
    public bool $deletable = true;
    public string $many_sub_url = '';
    public mixed $id = null;
    public array $title_path = [];
    public array $tabs;
    public string $model_name = '';
    public string $custom_css = '';
    public string $custom_js = '';
    public array $sections = ['General' => ['Fields' => [], 'classes' => []]];
    public array $table_fields = [];
    public array $buttons = [];
    public array $form_data = [];
    public string $verbose_name = '';
    protected Model $model;

    /**
     * Construct the Form object
     */
    public function __construct()
    {
        # Obtain submitted form data
        $this->form_data = $_POST;

        # Gather information about the authenticated user
        $client = new Auth();
        $client->username = $_SESSION['Username'] ?: DEFAULT_CLIENT_USERNAME;

        # Obtain the `id` from URL parameters
        $this->id = is_numeric($_GET['id']) ? intval($_GET['id']) : null;

        # For `many` Forms, specify an ID
        if ($this->many) {
            $this->model = new $this->model_name(id: $this->id, client: $client);
        }
        # For non `many` Forms, don't specify an ID when creating
        else {
            $this->model = new $this->model_name(client: $client);
        }

        foreach ($this->title_path as &$path) {
            $path = gettext($path);
        }

        # Define a verbose name for this Form if one was not specifically assigned
        $this->verbose_name = $this->verbose_name ?: $this->generate_verbose_name();
    }

    /**
     * Auto-generates a verbose name for this Form class using its class name.
     * @returns string The human-readable name for this Form class.
     */
    private function generate_verbose_name(): string
    {
        # Separate the class name's words with spaces, allow consecutive capital characters like 'DNS'
        $form_name = $this->get_class_shortname();
        return preg_replace('/(?<=[a-z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])/', ': ', $form_name);
    }

    /**
     * Populates the form sections with the desired Fields and returns an array of
     * @return \Form The pfSense webConfigurator \Form object.
     */
    final public function get_form(): \Form
    {
        # Create the Form object to assign sections and Fields to
        $form = new \Form();

        # Loop through each of the requested sections populate them
        foreach ($this->sections as $section_name => $section_params) {
            # Create the section object
            $section = new \Form_Section($section_name);

            # Add any custom classes defined
            $section->addClass(implode(' ', $section_params['classes'] ?? []));

            # Add any defined Fields to this section
            foreach ($section_params['Fields'] as $section_field) {
                # Make this a password field if it is a sensitive field
                if ($this->model->$section_field->sensitive) {
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
            $form
                ->addGlobal(
                    new \Form_Button(
                        name: $button_name,
                        title: $button_params['title'] ?: $button_name,
                        link: $button_params['link'],
                        icon: $button_params['icon'],
                    ),
                )
                ->addClass(implode(' ', $button_params['classes'] ?? []))
                ->setOnclick($button_params['on_click']);
        }

        return $form;
    }

    /**
     * Obtains a PHP table that contains all current Model objects for `many` Forms.
     */
    public function get_table()
    {
        # Setup the table structure
        $tb = "<div class='panel panel-default'>";
        $tb .=
            "<div class='panel-heading'><h2 class='panel-title'>" .
            gettext($this->model->verbose_name_plural) .
            '</h2></div>';
        $tb .= "<div class='panel-body table-responsive'>";
        $tb .=
            "<table class='table table-striped table-hover table-condensed table-rowdblclickedit sortable-theme-bootstrap' data-sortable>";
        $tb .= '<thead>';
        $tb .= '<tr>';

        # Include table header Fields for each `table_field` requested.
        foreach ($this->table_fields as $table_field) {
            $tb .= '<th>' . gettext(ucwords($this->model->$table_field->verbose_name)) . '</th>';
        }

        # Close the table header
        $tb .= '</tr>';
        $tb .= '</thead>';

        # Add table entries for each existing model object
        foreach ($this->model->read_all()->model_objects as $model_object) {
            # Add a row for this object
            $tb .= '<tr>';

            # Add table data for each `table_field` this object has
            foreach ($this->table_fields as $table_field) {
                # Determine the value of this field
                $value = $model_object->$table_field->value;

                # Use the choice verbose value if present
                if (
                    $model_object->$table_field->verbose_choices and
                    array_key_exists($value, $model_object->$table_field->verbose_choices)
                ) {
                    $value = $model_object->$table_field->verbose_choices[$value];
                }

                $tb .= '<td>' . $value . '</td>';
            }

            # Add the edit and delete buttons to this row if allowed
            $tb .= '<td>';
            if ($this->editable) {
                $tb .=
                    "<a class='fa fa-pencil' title='Edit " .
                    gettext($model_object->verbose_name) .
                    "' href='" .
                    $this->many_sub_url .
                    '?id=' .
                    $model_object->id .
                    "'></a>";
            }
            if ($this->deletable) {
                $tb .=
                    "<a class='fa fa-trash' title='Delete " .
                    gettext($model_object->verbose_name) .
                    "' href='" .
                    $this->url .
                    '?act=del&amp;id=' .
                    $model_object->id .
                    "' usepost></a>";
            }
            $tb .= '</td>';
            $tb .= '</tr>';
        }

        # Close remaining tags
        $tb .= '</table>';
        $tb .= '</div>';
        $tb .= '</div>';
        # Place 'Add' button
        $tb .=
            "<nav class='action-buttons'><a href='" .
            $this->many_sub_url .
            "' class='btn btn-sm btn-success'><i class='fa fa-plus icon-embed-btn'></i>" .
            gettext('Add') .
            '</a></nav>';
        $tb .= '</div>';

        return $tb;
    }

    /**
     * Prints this form as a web page. This method is intended to be called by a PHP endpoint somewhere in the pfSense
     * web root.
     */
    final public function print_form(): void
    {
        # Print the static pfSense UI and include any custom CSS or JS files
        global $config;
        $pgtitle = $this->title_path;
        include 'head.inc';
        echo "<link rel='stylesheet' href='$this->custom_css'/>";
        echo "<script type='application/javascript' src='$this->custom_js'></script>";
        \display_top_tabs($this->tabs, true);

        # Call on_load to load any custom components requested by the child class
        $this->on_load();

        # Perform the save action if the save button was submitted
        if ($this->form_data['save']) {
            $this->on_save();
        }

        # Delete the associated model object when requested
        if ($this->form_data['act'] === 'del') {
            $this->on_delete($this->form_data['id']);
        }

        # Check if a button press was submitted that is assigned a specific callable
        foreach ($this->buttons as $button_name => $button_params) {
            if (array_key_exists($button_name, $this->form_data) and $button_params['callable']) {
                $callable = $button_params['callable'];
                $this->$callable();
            }
        }

        # Print the auto-generated form
        if ($this->many) {
            print $this->get_table();
        } else {
            print $this->get_form();
        }

        include 'foot.inc';
    }

    /**
     * Performs tasks when the Form page is initially loaded. This method is intended to be overridden by the child
     * class.
     */
    public function on_load(): void
    {
    }

    /**
     * Performs validation and saves the Model assigned to this ModelForm.
     */
    public function on_save(): void
    {
        # Update settings with the newly provided values
        $representation_data = $this->id ? ['id' => $this->id] : [];
        $representation_data += $this->form_data_to_model_representation(form_data: $_POST, model: $this->model);
        $this->model->from_representation(data: $representation_data);

        # If any 'sensitive' fields are assigned to this Model, ensure the confirmed value matches
        foreach ($this->model->get_fields() as $field) {
            if ($this->model->$field->sensitive and $_POST[$field] !== $_POST[$field . '_confirm']) {
                $this->print_input_error_banner(message: "Field '$field' confirm value does not match.");
                return;
            }
        }

        # When an ID was found or the assigned model is not a `many` model, update the object. Otherwise create it.
        try {
            ($this->id or !$this->model->many) ? $this->model->update() : $this->model->create();
            $this->print_success_banner();
        } catch (Response $resp_error) {
            $error_message = $resp_error->getMessage();
            $this->print_input_error_banner(message: $error_message);
        }
    }

    /**
     * Tries to delete the Model object when requested.
     * @param int|string $id The ID of the Model object to delete.
     */
    public function on_delete(int|string $id): void
    {
        # Convert the ID to an integer if it is numeric
        if (is_numeric($id)) {
            $id = (int) $id;
        }

        try {
            (new $this->model(id: $id))->delete();
            $this->print_success_banner("Deleted {$this->model->verbose_name} with ID $id.");
        } catch (Response $resp_error) {
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
    protected function form_data_to_model_representation(array $form_data, Model $model): array
    {
        # Variables
        $representation = [];

        # Loop through each Field available to this Model
        foreach ($model->get_fields() as $field) {
            # Determine the class of this Field.
            $class = get_class($model->$field);

            # For boolean Fields, assume the value is false if it does not match the true value exactly
            if ($class === 'RESTAPI\Fields\BooleanField' and $form_data[$field] !== $model->$field->indicates_true) {
                $form_data[$field] = $model->$field->indicates_false;
            }

            # For sensitive fields, do not update the value if the value matches the masked value
            if ($model->$field->sensitive and $form_data[$field] === Field::SENSITIVE_MASK) {
                unset($form_data[$field]);
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
    public function print_success_banner(string $message = ''): void
    {
        print_apply_result_box(retval: 0, extra_text: $message);
    }

    /**
     * Prints the input error banner at the top of the form page.
     * @param string $message Adds an extra message to the banner.
     */
    public function print_input_error_banner(string $message = ''): void
    {
        print_input_errors([$message]);
    }

    /**
     * Prints an info/warning banner at the top of the form page.
     * @param string $message The message to display on the banner.
     * @param string $class The CSS class to use for the banner.
     */
    public function print_info_banner(string $message = '', string $class = 'alert-warning'): void
    {
        print_info_box(msg: $message, class: $class);
    }

    /**
     * Obtains the pfSense privilege name for this Form. This privilege name is automatically generated using the
     * $url attribute and formats it to be consistent with the other webConfigurator page privileges.
     * @returns string The pfSense privilege name for this Form (i.e. page-test-form)
     */
    public function get_priv_name(): string
    {
        # Obtain the PHP file name for this Form using it's webroot URL
        $name = basename($this->url);

        # Remove the .php extension and convert underscores to hyphens
        $name = str_replace('.php', '', $name);
        $name = str_replace('_', '-', $name);

        # Return the privilege name prefixed with the `page-` privilege type
        return "page-$name";
    }

    /**
     * Utilizes the $url properties of this Form to generate pfSense privileges specific to this Form class.
     * The resulting array is then supplied to the pfSense privilege system located at /etc/inc/priv/.
     * @returns array The pfSense priv list entry array corresponding to the privileges of this Endpoint.
     */
    public function generate_pfsense_privs(): array
    {
        return [
            $this->get_priv_name() => [
                'name' => 'WebCfg - ' . $this->verbose_name,
                'descr' => "Allow access to the $this->url page.",
            ],
        ];
    }

    /**
     * Generate an endpoint for this Form in the pfSense webroot.
     * @returns bool Returns true if the form was successfully built, otherwise false.
     */
    public function build_form_url(): bool
    {
        # Get the fully qualified and non qualified class names for this object
        $fq_class_name = get_class($this);
        $nq_class_name = $this->get_class_shortname();

        # Specify the PHP code to write to the Endpoints index.php file
        $code =
            "<?php
            require_once('RESTAPI/Forms/" .
            $nq_class_name .
            ".inc');
            require_once('guiconfig.inc');
            (new " .
            $fq_class_name .
            '())->print_form();';

        # Assign the absolute path to the file. Assume index.php filename if not specified.
        $filename = "/usr/local/www/$this->url";
        $filename = str_ends_with($filename, '.php') ? $filename : "$filename/index.php";
        mkdir(dirname($filename), 0644, true);
        file_put_contents($filename, $code);

        # Return true if the form was successfully built
        if (is_file($filename)) {
            return true;
        }

        return false;
    }
}
