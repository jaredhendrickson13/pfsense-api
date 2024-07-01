#!/bin/sh

# Set build variables
FREEBSD_VERSION=${FREEBSD_VERSION:-"freebsd/FreeBSD-14.0-STABLE"}
BUILD_VERSION=${BUILD_VERSION:-"0.0_0-dev"}

# Start the vagrant box
FREEBSD_VERSION=${FREEBSD_VERSION} vagrant up
vagrant ssh -c "rm -rf /home/vagrant/build/"
vagrant upload ./ /home/vagrant/build/pfsense-api --compress

cat << END | vagrant ssh
composer install --working-dir /home/vagrant/build/pfsense-api
rm -rf /home/vagrant/build/pfsense-api/vendor/composer && rm /home/vagrant/build/pfsense-api/vendor/autoload.php
cp -r /home/vagrant/build/pfsense-api/vendor/* /home/vagrant/build/pfsense-api/pfSense-pkg-RESTAPI/files/usr/local/pkg/RESTAPI/.resources/includes/
python3.8 /home/vagrant/build/pfsense-api/tools/make_package.py -t $BUILD_VERSION
END

SSH_CONFIG_FILE=$(mktemp)
vagrant ssh-config > "$SSH_CONFIG_FILE"
scp -F $SSH_CONFIG_FILE vagrant@default:/home/vagrant/build/pfsense-api/pfSense-pkg-RESTAPI/work/pkg/pfSense-pkg-RESTAPI-$BUILD_VERSION.pkg .
rm $SSH_CONFIG_FILE