# Building Custom Dispatcher Classes

[Dispatchers](https://pfrest.org/php-docs/classes/RESTAPI-Core-Dispatcher.html) are responsible for
executing long-running operations in the background. These work by executing a handler script in a separate process
which calls the Dispatcher's primary function. This allows the main process (your API call) to continue without having
to wait for the operation to complete. A list of all current Dispatcher classes can be found in the
[PHP reference](https://pfrest.org/php-docs/namespaces/restapi-dispatchers.html). This document will guide you through the process of building a custom Dispatcher class as well as
options for interacting with the Dispatcher system when necessary.

!!! Note
    You can bypass the Dispatcher process spawning in the background in your API calls by setting the ['async' control parameter](https://pfrest.org/COMMON_CONTROL_PARAMETERS/#async)
    to `false`. This will cause the Dispatcher to execute the operation within the main process and wait for the operation to
    complete before returning a response.

## Getting Started

```php
<?php

namespace RESTAPI\Dispatchers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Dispatcher;

/**
 * TODO: Add a description of your Dispatcher class here.
 */
class MyCustomDispatcher extends Dispatcher {

}
```

!!! Important
    Be sure to place this class file within `/usr/local/pkg/RESTAPI/Dispatchers/` directory and name the file after your
    Dispatcher class name with a `.inc` extension.

## Defining Dispatcher Properties

There are a few properties that you can define in your Dispatcher class to customize the behavior of the Dispatcher. These properties
can either be set in the `__construct` method or directly in the class definition:

### timeout

The `timeout` property is used to define the maximum amount of time in seconds that the Dispatcher operation should run before
timing out. This property is optional and defaults to 300 seconds (5 minutes).

### max_queue

The `max_queue` property is used to define the maximum number of instances of the operation that can be running at a time. If the
queue limit is exceeded, the Dispatcher will trigger a 503 Service Unavailable error. This property is optional and defaults to 10.

!!! Warning
    Be sure to set the `max_queue` property to a reasonable number to prevent the server from running out of resources.

### schedule

The `schedule` property is used to define the schedule on which the Dispatcher should run automatically. This property 
should be set to either `hourly`, `daily` or `weekly`. This property is optional.

!!! Note
    This property is primarily intended for Cache classes, but can be used in Dispatcher classes as well.

### required_packages

The `required_packages` property is used to define an array of pfSense package names (including the `pfSense-pkg- prefix)
that are required for the Dispatcher operation. If any of the required packages are not installed, the Dispatcher operation will
not run. This property is optional.

### package_includes

The `package_includes` property is used to define an array of package PHP files that should be included before the Dispatcher
operation runs. This is useful for including package-specific functions or classes that are required for the Dispatcher operation.
This property is optional.

## Defining the _process() method

The `_process()` method is used to define the operation that the Dispatcher will execute. This method should contain the
code that will be run in the background. This method should be defined in your Dispatcher class like this:

```php
/**
 * Method to execute the Dispatcher operation
 */
protected function _process(): void {
    # TODO Add your operation code here
}
```

!!! Note
    There is currently no way to pass in arguments to the `_process()` method when it is called in the background.
    If you need to pass arguments to the `_process()` method, you will need to set them as properties of the Dispatcher class
    and access them within the `_process()` method.

## Spawning Processes

Once you have defined your Dispatcher class, you can spawn a new process by calling the `spawn_process()` method from the
main process. This method will create a new background process that will execute the `_process()` method. Here is an example:

```php
$dispatcher = new MyCustomDispatcher(async: true);
$dispatcher->spawn_process();
```

!!! Notes
    - The `async` parameter is used to determine if the Dispatcher should run in the background or in the main process. If
    `async` is set to `true`, the Dispatcher will run in the background. If `async` is set to `false`, the Dispatcher will
    run in the main process and wait for the operation to complete before returning a response.
    - If you attempt to spawn more processes than the `max_queue` property allows, the Dispatcher will trigger a 503 Service
    Unavailable error.

## Queues

In addition to running processes in the background,
Dispatchers also ensure that only one instance of a given operation is running at a time using a queue system. The
whole process looks like this:

1. An API call is made which requires a long-running operation. (i.e. applying interfaces)
2. The Dispatcher checks how many instances of the operation are currently in the queue. If the queue limit is exceeded,
   the Dispatcher will trigger a 503 Service Unavailable error. If the queue limit is not exceeded, the Dispatcher will
   add the operation to the queue and spawn a background process.
3. The background process will check for a Dispatcher lock file. If the lock file already exists, the process will continue to
   wait until the lock file is removed or the Dispatcher's timeout is exceeded. If the lock file does not exist, the
   process will create a new lock file and begin executing the operation.
4. After the operation has completed, the lock file is removed so the next process in the queue can begin.

!!! Important
    The Dispatcher queue is designed to be a safety mechanism to prevent the server from running conflicting processes or
    running out of resources. You should still design your applications to keep track of the state of these operations by
    polling the applicable endpoints and waiting until the operation is complete before proceeding. Typically, this can be
    done by making a `GET` request to applicable 'apply' endpoints and checking the `applied` field.

## Manually Running a Dispatcher

Dispatchers are intended to be called automatically by the relevant Models so there should be no need to
manually call a Dispatcher. However, if you do need to manually call a Dispatcher, you can do so using the
`pfsense-restapi notifydispatcher <DISPATCHER CLASS NAME>` command from the command line.