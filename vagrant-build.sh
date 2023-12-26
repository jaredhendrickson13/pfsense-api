vagrant ssh -c "rm -rf /home/vagrant/build/"
vagrant upload ./ /home/vagrant/build/pfsense-api --compress

cat << END | vagrant ssh
composer install --working-dir /home/vagrant/build/pfsense-api
rm -rf /home/vagrant/build/pfsense-api/vendor/composer && rm /home/vagrant/build/pfsense-api/vendor/autoload.php
cp -r /home/vagrant/build/pfsense-api/vendor/* /home/vagrant/build/pfsense-api/pfSense-pkg-API/files/etc/inc/
python3.8 /home/vagrant/build/pfsense-api/tools/make_package.py -t 0.0.1
END

SSH_CONFIG_FILE=$(mktemp)
vagrant ssh-config > "$SSH_CONFIG_FILE"
scp -F $SSH_CONFIG_FILE vagrant@default:/home/vagrant/build/pfsense-api/pfSense-pkg-API/work/pkg/pfSense-pkg-API-0.0_1.pkg ./
rm $SSH_CONFIG_FILE