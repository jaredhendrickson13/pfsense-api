#!/bin/sh

# Set build variables
FREEBSD_VERSION=${FREEBSD_VERSION:-"freebsd/FreeBSD-14.0-CURRENT"}
BUILD_VERSION=${BUILD_VERSION:-"0.0_0-dev"}

# Start the vagrant box
FREEBSD_VERSION=${FREEBSD_VERSION} vagrant up
vagrant ssh -c "rm -rf /home/vagrant/build"

# Obtain the SSH config for the vagrant box
SSH_CONFIG_FILE=$(mktemp)
vagrant ssh-config > "$SSH_CONFIG_FILE"

# Copy the source code to the vagrant box using SCP (vagrant upload skips hidden files)
rsync -avz --progress -e "ssh -F $SSH_CONFIG_FILE" ../pfsense-api vagrant@default:/home/vagrant/build/ --exclude node_modules --exclude .git --exclude .phpdoc --exclude ./vendor --exclude .vagrant

# Run the build script on the vagrant box
cat << END | vagrant ssh
composer install --working-dir /home/vagrant/build/pfsense-api
cp -r /home/vagrant/build/pfsense-api/vendor/* /home/vagrant/build/pfsense-api/pfSense-pkg-RESTAPI/files/usr/local/pkg/RESTAPI/.resources/vendor/
python3.8 /home/vagrant/build/pfsense-api/tools/make_package.py -t $BUILD_VERSION
END

# Copy the built package back to the host using SCP
scp -F $SSH_CONFIG_FILE vagrant@default:/home/vagrant/build/pfsense-api/pfSense-pkg-RESTAPI/work/pkg/pfSense-pkg-RESTAPI-$BUILD_VERSION.pkg .
rm $SSH_CONFIG_FILE