#!/bin/sh

# This is a helper script for \RESTAPI\Core\Dispatcher.inc. The Dispatcher class will spawn processes using this helper
# script in the background. This scripts requires three arguments which are provided by the Dispatcher:
# $1: The shortname of the Dispatcher class being called.
# $2: The PID file associated with the spawned Dispatcher process.
# $3: The number of seconds the Dispatcher process can run before timing out.

# Start the Dispatcher process using manage.php (pfsense-restapi CLI), enforce timeout
timeout --foreground $3 php -f /usr/local/pkg/RESTAPI/.resources/scripts/manage.php notifydispatcher $1;

# Remove the PID file associated with this Dispatcher process
rm $2;
