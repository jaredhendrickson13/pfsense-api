# Dispatchers and Caching

The REST API is designed to be as fast as possible. This is largely achieved by executing long-running operations in the
background and caching large datasets which are typically time-consuming to generate. The package automatically handles
these operations using Dispatchers and Caches. You can use this document to learn more about how these systems work and
how to interact with them, if necessary.

## Dispatchers

[Dispatchers](https://pfrest.org/php-docs/classes/RESTAPI-Core-Dispatcher.html) are responsible for 
executing long-running operations in the background. These work by executing a handler script in a separate process 
which calls the Dispatcher's primary function. This allows the main process (your API call) to continue without having 
to wait for the operation to complete. A list of all current Dispatcher classes can be found in the 
[PHP reference](https://pfrest.org/php-docs/namespaces/restapi-dispatchers.html).

!!! Note
    You can bypass the Dispatcher process spawning in the background in your API calls by setting the ['async' control parameter](https://pfrest.org/COMMON_CONTROL_PARAMETERS/#async)
    to `false`. This will cause the Dispatcher to execute the operation within the main process and wait for the operation to
    complete before returning a response.

### Queues

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

### Manually running a Dispatcher

Dispatchers are intended to be called automatically by the relevant REST API endpoints so there should be no need to
manually call a Dispatcher. However, if you do need to manually call a Dispatcher, you can do so using the 
`pfsense-restapi notifydispatcher <DISPATCHER CLASS NAME>` command from the command line.

## Caching

[Caches](https://pfrest.org/php-docs/classes/RESTAPI-Core-Cache.html) are responsible for storing large datasets which 
are typically time-consuming to generate. These are an extension of the Dispatcher system and help speed up the process 
of generating these datasets by populating the cache in the background and only refreshing them on a specific schedule.
A list of all current Cache classes can be found in the [PHP reference](https://pfrest.org/php-docs/namespaces/restapi-caches.html).

### Refreshing a cache

Caches are designed to automatically refresh on a schedule defined in the Cache class. This schedule is typically set to
refresh the cache hourly. However, you can manually refresh a cache by calling the `pfsense-restapi refreshcache <CACHE CLASS NAME>`
command from the command line.

### Cache files

Cache files are stored in the `/usr/local/pkg/RESTAPI/.resources/cache` directory. These files are typically JSON 
datasets and you should not interact with them directly.

!!! Caution
    Do not manually modify cache files as this may cause unexpected behavior in the API. If you need to modify the data
    in a cache file, you should do so by modifying the relevant Cache class and allowing the cache to refresh on its own.