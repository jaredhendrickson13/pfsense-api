# Building Custom Query Filter Classes

For advanced users, the REST API's framework allows for custom query filter classes to be added using PHP. To add a 
custom query filter class, extend the `\RESTAPI\Core\QueryFilter` class and implement the `evaulate` method. This 
method takes the targetted field's value and a filter value and returns a boolean value indicating if the field
matches the filter. Below is an example of a custom QueryFilter class:

```php
<?php

namespace RESTAPI\QueryFilters;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\QueryFilter;

class MyCustomQueryFilter extends QueryFilter {
    # Sets the name of the query filter that will be used in the query string
    public string $name = 'customfilter';

    # Sets the logic for the query filter. In this example, we are checking if the field value is equal to the filter value.
    public function evaluate(mixed $field_value, mixed $filter_value): bool {
        return $field_value == $filter_value;
    }
}
```

Once your custom QueryFilter class is created, simply place it at `/usr/local/pkg/RESTAPI/QueryFilters/MyCustomQueryFilter.inc`
and it will automatically become an available query filter that can be used by the REST API. 

An example usage of this custom query filter would be `/api/v2/firewall/rules?fieldname__customfilter=filter_value`.

!!! Tip
    You can rename `MyCustomQueryFilter` to any name you like, but make sure the class name matches the file name.