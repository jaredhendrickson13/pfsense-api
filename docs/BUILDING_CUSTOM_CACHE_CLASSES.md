# Building Custom Cache Classes

[Caches](https://pfrest.org/php-docs/classes/RESTAPI-Core-Cache.html) are responsible for storing large datasets which
are typically time-consuming to generate. These are an extension of the Dispatcher system and help speed up the process
of generating these datasets by populating the cache in the background and only refreshing them on a specific schedule.
A list of all current Cache classes can be found in the [PHP reference](https://pfrest.org/php-docs/namespaces/restapi-caches.html).
This document will guide you through the process of building a custom Cache class as well as options for interacting with
the Cache system when necessary.

## Getting Started

To create a custom Cache class, you will need to create a new PHP file in the `/usr/local/pkg/RESTAPI/Caches` directory.
Use the following class template to initialize your custom Cache class:

```php
<?php

namespace RESTAPI\Caches;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Cache;

/**
 * TODO: Add a description of your Cache class here.
 */
class MyCustomCache extends Cache {

}
```

!!! Important
    Be sure to place this class file within `/usr/local/pkg/RESTAPI/Caches/` directory and name the file after your
    Cache class name with a `.inc` extension.

## Defining Cache Properties

There are a few properties that you can define in your Cache class to customize the behavior of the cache. These properties
can either be set in the `__construct` method or directly in the class definition. 

!!! Note
    Because the Cache class is an extension of the Dispatcher class, the Cache class shares the same properties as the
    Dispatcher class. See the [Building Custom Dispatcher Classes](BUILDING_CUSTOM_DISPATCHER_CLASSES.md#defining-dispatcher-properties)
    document for a full list of properties.

The following properties are available that are commonly used for Cache classes:

### schedule

The `schedule` property is used to define the schedule on which the cache should be refreshed. This property should be
set to either `hourly`, `daily` or `weekly`.

### timeout

The `timeout` property is used to define the maximum amount of time in seconds that the cache operation should run before
timing out. This property is optional and defaults to 300 seconds (5 minutes).

### required_packages

The `required_packages` property is used to define an array of pfSense package names (including the `pfSense-pkg- prefix)
that are required for the cache operation. If any of the required packages are not installed, the cache operation will
not run. This property is optional.

### package_includes

The `package_includes` property is used to define an array of package PHP files that should be included before the cache
operation runs. This is useful for including package-specific functions or classes that are required for the cache operation.
This property is optional.

## Defining the get_data_to_cache() method

The `get_data_to_cache()` method is used to define the operation that will generate the data to be stored in the cache.
This method should return an array of data that will be stored in the cache file. This method should be defined in your
Cache class and will be called automatically by the Cache system.

```php
/**
 * Method to fetch the data to cache
 * @return array The data to cache
 */
protected function get_data_to_cache(): array {
    # TODO: Add your data fetching logic here
}
```

!!! Note
    This method will automatically be called in the background according to the schedule defined in the Cache class to
    update the cache file.

## Refreshing a cache

Caches are designed to automatically refresh on a schedule defined in the Cache class. This schedule is typically set to
refresh the cache hourly. However, you can manually refresh a cache by calling the `pfsense-restapi refreshcache <CACHE CLASS NAME>`
command from the command line.

## Cache files

Cache files are stored in the `/usr/local/pkg/RESTAPI/.resources/cache` directory. These files are typically JSON
datasets and you should not modify them directly. In rare cases, you many need to delete the cache file to force a
full refresh of the cache file. This can be done by deleting the cache file from the cache directory:

```shell
rm /usr/local/pkg/RESTAPI/.resources/cache/<CACHE CLASS NAME>.json
```

!!! Caution
    Do not manually modify cache files as this may cause unexpected behavior in the API. If you need to modify the data
    in a cache file, you should do so by modifying the relevant Cache class and [refreshing the cache](#refreshing-a-cache).

## Examples

You can find examples of fully implemented Cache classes in the [PHP reference](https://pfrest.org/php-docs/namespaces/restapi-caches.html).
Select the Cache class you are interested in to view the class's PHPDoc documentation, and then click on the
`<>` symbol next to the class name to view the class's source code.