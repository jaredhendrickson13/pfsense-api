# $FreeBSD$

PORTNAME=pfSense-pkg-API
PORTVERSION=0.0
PORTREVISION=3
CATEGORIES=sysutils
MASTER_SITES=# empty
DISTFILES=# empty
EXTRACT_ONLY=# empty
MAINTAINER=	jaredhendrickson13@gmail.com
COMMENT=pfSense API package
LICENSE=APACHE20
NO_BUILD=yes
NO_MTREE=yes
SUB_FILES=pkg-install pkg-deinstall
SUB_LIST=PORTNAME=${PORTNAME}

do-extract:
	 ${MKDIR} ${WRKSRC}

do-install:
	 ${MKDIR} ${STAGEDIR}/usr/local
	 ${MKDIR} ${STAGEDIR}/usr/local/www
	 ${MKDIR} ${STAGEDIR}/usr/local/pkg
	 ${MKDIR} ${STAGEDIR}/usr/local/share
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/status
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/routing
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/access_token
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/interfaces
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/status/carp
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/status/carp/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/nat
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/virtualips
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/states
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/rules
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/nat/portforwards
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/nat/portforwards/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/nat/portforwards/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/virtualips/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/virtualips/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/states/size
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/states/size/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/rules/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/rules/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/delete/address
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/add/address
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/config
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/certificates
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/version
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/api
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/hostname
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/arp
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/dns
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/certificates/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/certificates/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/api/errors
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/hostname/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/arp/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/dns/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/dns/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/system/dns/delete/servers
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/authservers
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/delete/privs
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/delete/groups
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/authservers/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/authservers/ldap
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/authservers/radius
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/authservers/ldap/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/authservers/ldap/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/authservers/radius/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/add/privs
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/users/add/groups
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/routing/gateways
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/ntpd
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/sshd
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/syslogd
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/start
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/stop
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/restart
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dhcpd
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dpinger
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/ntpd/start
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/ntpd/stop
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/ntpd/restart
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/sshd/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/sshd/start
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/sshd/stop
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/sshd/restart
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/syslogd/start
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/syslogd/stop
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/syslogd/restart
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dhcpd/start
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dhcpd/stop
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dhcpd/restart
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dpinger/start
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dpinger/stop
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/dpinger/restart
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/apply
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/start
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/stop
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/restart
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/delete/hosts
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/modify/hosts
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/services/unbound/add/hosts
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/interfaces/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/interfaces/add
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans/delete
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans/modify
	 ${MKDIR} ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans/add
	 ${MKDIR} ${STAGEDIR}/usr/local/share/pfSense-pkg-API
	 ${MKDIR} ${STAGEDIR}/etc/inc
	 ${MKDIR} ${STAGEDIR}/etc/inc/php-jwt
	 ${MKDIR} ${STAGEDIR}/etc/inc/api
	 ${MKDIR} ${STAGEDIR}/etc/inc/php-jwt/src
	 ${MKDIR} ${STAGEDIR}/etc/inc/api/framework
	 ${MKDIR} ${STAGEDIR}/etc/inc/api/api_models
	 
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/index.php ${STAGEDIR}/usr/local/www/api
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/api_return_codes.txt ${STAGEDIR}/usr/local/www/api/v1
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/status/carp/index.php ${STAGEDIR}/usr/local/www/api/v1/status/carp
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/status/carp/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/status/carp/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/nat/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/nat
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/nat/portforwards/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/nat/portforwards
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/nat/portforwards/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/nat/portforwards/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/nat/portforwards/add/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/nat/portforwards/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/virtualips/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/virtualips
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/virtualips/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/virtualips/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/virtualips/add/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/virtualips/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/states/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/states
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/states/size/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/states/size
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/states/size/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/states/size/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/rules/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/rules
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/rules/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/rules/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/rules/add/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/rules/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/aliases/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/aliases/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/aliases/delete/address/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/delete/address
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/aliases/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/aliases/add/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/firewall/aliases/add/address/index.php ${STAGEDIR}/usr/local/www/api/v1/firewall/aliases/add/address
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/config/index.php ${STAGEDIR}/usr/local/www/api/v1/system/config
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/certificates/index.php ${STAGEDIR}/usr/local/www/api/v1/system/certificates
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/certificates/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/system/certificates/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/certificates/add/index.php ${STAGEDIR}/usr/local/www/api/v1/system/certificates/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/version/index.php ${STAGEDIR}/usr/local/www/api/v1/system/version
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/api/index.php ${STAGEDIR}/usr/local/www/api/v1/system/api
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/api/errors/index.php ${STAGEDIR}/usr/local/www/api/v1/system/api/errors
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/hostname/index.php ${STAGEDIR}/usr/local/www/api/v1/system/hostname
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/hostname/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/system/hostname/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/arp/index.php ${STAGEDIR}/usr/local/www/api/v1/system/arp
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/arp/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/system/arp/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/dns/index.php ${STAGEDIR}/usr/local/www/api/v1/system/dns
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/dns/delete/servers/index.php ${STAGEDIR}/usr/local/www/api/v1/system/dns/delete/servers
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/system/dns/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/system/dns/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/index.php ${STAGEDIR}/usr/local/www/api/v1/users
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/users/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/delete/privs/index.php ${STAGEDIR}/usr/local/www/api/v1/users/delete/privs
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/delete/groups/index.php ${STAGEDIR}/usr/local/www/api/v1/users/delete/groups
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/users/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/authservers/index.php ${STAGEDIR}/usr/local/www/api/v1/users/authservers
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/authservers/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/users/authservers/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/authservers/ldap/index.php ${STAGEDIR}/usr/local/www/api/v1/users/authservers/ldap
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/authservers/ldap/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/users/authservers/ldap/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/authservers/ldap/add/index.php ${STAGEDIR}/usr/local/www/api/v1/users/authservers/ldap/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/authservers/radius/index.php ${STAGEDIR}/usr/local/www/api/v1/users/authservers/radius
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/authservers/radius/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/users/authservers/radius/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/add/index.php ${STAGEDIR}/usr/local/www/api/v1/users/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/add/privs/index.php ${STAGEDIR}/usr/local/www/api/v1/users/add/privs
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/users/add/groups/index.php ${STAGEDIR}/usr/local/www/api/v1/users/add/groups
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/routing/gateways/index.php ${STAGEDIR}/usr/local/www/api/v1/routing/gateways
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/access_token/index.php ${STAGEDIR}/usr/local/www/api/v1/access_token
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/index.php ${STAGEDIR}/usr/local/www/api/v1/services
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/ntpd/start/index.php ${STAGEDIR}/usr/local/www/api/v1/services/ntpd/start
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/ntpd/stop/index.php ${STAGEDIR}/usr/local/www/api/v1/services/ntpd/stop
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/ntpd/restart/index.php ${STAGEDIR}/usr/local/www/api/v1/services/ntpd/restart
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/sshd/index.php ${STAGEDIR}/usr/local/www/api/v1/services/sshd
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/sshd/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/services/sshd/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/sshd/start/index.php ${STAGEDIR}/usr/local/www/api/v1/services/sshd/start
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/sshd/stop/index.php ${STAGEDIR}/usr/local/www/api/v1/services/sshd/stop
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/sshd/restart/index.php ${STAGEDIR}/usr/local/www/api/v1/services/sshd/restart
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/syslogd/start/index.php ${STAGEDIR}/usr/local/www/api/v1/services/syslogd/start
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/syslogd/stop/index.php ${STAGEDIR}/usr/local/www/api/v1/services/syslogd/stop
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/syslogd/restart/index.php ${STAGEDIR}/usr/local/www/api/v1/services/syslogd/restart
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/start/index.php ${STAGEDIR}/usr/local/www/api/v1/services/start
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/stop/index.php ${STAGEDIR}/usr/local/www/api/v1/services/stop
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/restart/index.php ${STAGEDIR}/usr/local/www/api/v1/services/restart
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/dhcpd/start/index.php ${STAGEDIR}/usr/local/www/api/v1/services/dhcpd/start
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/dhcpd/stop/index.php ${STAGEDIR}/usr/local/www/api/v1/services/dhcpd/stop
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/dhcpd/restart/index.php ${STAGEDIR}/usr/local/www/api/v1/services/dhcpd/restart
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/dpinger/start/index.php ${STAGEDIR}/usr/local/www/api/v1/services/dpinger/start
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/dpinger/stop/index.php ${STAGEDIR}/usr/local/www/api/v1/services/dpinger/stop
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/dpinger/restart/index.php ${STAGEDIR}/usr/local/www/api/v1/services/dpinger/restart
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/apply/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound/apply
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/delete/hosts/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound/delete/hosts
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/modify/hosts/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound/modify/hosts
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/start/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound/start
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/stop/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound/stop
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/restart/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound/restart
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/services/unbound/add/hosts/index.php ${STAGEDIR}/usr/local/www/api/v1/services/unbound/add/hosts
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/interfaces/index.php ${STAGEDIR}/usr/local/www/api/v1/interfaces
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/interfaces/vlans/index.php ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/interfaces/vlans/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/interfaces/vlans/modify/index.php ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans/modify
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/interfaces/vlans/add/index.php ${STAGEDIR}/usr/local/www/api/v1/interfaces/vlans/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/interfaces/delete/index.php ${STAGEDIR}/usr/local/www/api/v1/interfaces/delete
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/www/api/v1/interfaces/add/index.php ${STAGEDIR}/usr/local/www/api/v1/interfaces/add
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/pkg/api.xml ${STAGEDIR}/usr/local/pkg
	 ${INSTALL_DATA} ${FILESDIR}/usr/local/share/pfSense-pkg-API/info.xml ${STAGEDIR}/usr/local/share/pfSense-pkg-API
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/LICENSE ${STAGEDIR}/etc/inc/php-jwt
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/README.md ${STAGEDIR}/etc/inc/php-jwt
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/composer.json ${STAGEDIR}/etc/inc/php-jwt
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/src/SignatureInvalidException.php ${STAGEDIR}/etc/inc/php-jwt/src
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/src/BeforeValidException.php ${STAGEDIR}/etc/inc/php-jwt/src
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/src/JWK.php ${STAGEDIR}/etc/inc/php-jwt/src
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/src/ExpiredException.php ${STAGEDIR}/etc/inc/php-jwt/src
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/php-jwt/src/JWT.php ${STAGEDIR}/etc/inc/php-jwt/src
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/framework/APITools.inc ${STAGEDIR}/etc/inc/api/framework
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/framework/APIAuth.inc ${STAGEDIR}/etc/inc/api/framework
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/framework/APIBaseModel.inc ${STAGEDIR}/etc/inc/api/framework
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/framework/APIResponse.inc ${STAGEDIR}/etc/inc/api/framework
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAuthservers.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnboundRestart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAuthserversLDAPDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallAliasesAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIStatusCarpModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallNatPortForwardsDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallVirtualIPs.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIInterfacesAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnbound.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSyslogdRestart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemDNSModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesRestart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSSHdModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnboundStop.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSSHdRestart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesNtpdStart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIInterfacesVLANs.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSSHdStop.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIInterfacesVLANsModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIAccessToken.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnboundDeleteHosts.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemARP.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemCertificates.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesDpingerStop.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallRulesAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemARPDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSyslogdStop.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAuthserversLDAPAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIInterfacesDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSyslogdStart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAuthserversRADIUS.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemConfig.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemAPI.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallAliasesModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesNtpdRestart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnboundStart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemHostnameModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesDHCPdStart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAddPrivs.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesStart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallNat.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersDeletePrivs.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallVirtualIPsAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallAliases.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemDNS.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIInterfaces.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallStates.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnboundModifyHosts.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesDpingerStart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAuthserversDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersDeleteGroups.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemCertificatesAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSSHdStart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallVirtualIPsDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemVersion.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallAliasesDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIInterfacesVLANsAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallStatesSizeModify.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemDNSDeleteServers.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallNatPortForwards.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIRoutingGateways.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIStatusCarp.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAuthserversLDAP.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallStatesSize.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemCertificatesDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemHostname.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesDpingerRestart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallAliasesAddAddress.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallAliasesDeleteAddress.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAuthserversRADIUSDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnboundApply.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesUnboundAddHosts.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesNtpdStop.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesDHCPdStop.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APISystemAPIErrors.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIInterfacesVLANsDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallRulesDelete.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesSSHd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallNatPortForwardsAdd.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesDHCPdRestart.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsers.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIFirewallRules.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServices.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIServicesStop.inc ${STAGEDIR}/etc/inc/api/api_models
	 ${INSTALL_DATA} ${FILESDIR}/etc/inc/api/api_models/APIUsersAddGroups.inc ${STAGEDIR}/etc/inc/api/api_models
	 
.include <bsd.port.mk>