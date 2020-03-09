# $FreeBSD$

PORTNAME=	pfSense-pkg-API
PORTVERSION=	0.0
PORTREVISION=	1
CATEGORIES=	sysutils
MASTER_SITES=	# empty
DISTFILES=	# empty
EXTRACT_ONLY=	# empty
MAINTAINER=	jaredhendrickson13@gmail.com
COMMENT=	pfSense API package
LICENSE=	APACHE20
NO_BUILD=	yes
NO_MTREE=	yes
SUB_FILES=	pkg-install pkg-deinstall
SUB_LIST=	PORTNAME=${PORTNAME}

do-extract:
	${MKDIR} ${WRKSRC}

do-install:
	# INSTALL OUR API INCLUDE FILE
	${MKDIR} ${STAGEDIR}/etc/inc
	${INSTALL_DATA} ${FILESDIR}/etc/inc/api.inc \
		${STAGEDIR}/etc/inc
	${INSTALL_DATA} ${FILESDIR}/etc/inc/apicalls.inc \
		${STAGEDIR}/etc/inc
	${INSTALL_DATA} ${FILESDIR}/etc/inc/apiresp.inc \
		${STAGEDIR}/etc/inc

	# INSTALL OUR PFSENSE PKG
	${MKDIR} ${STAGEDIR}${PREFIX}/pkg
	${INSTALL_DATA} -m 0644 ${FILESDIR}${PREFIX}/pkg/api.xml \
		${STAGEDIR}${PREFIX}/pkg

    # INSTALL OUR API ENDPOINTS
	# API Base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/index.php \
    	${STAGEDIR}${PREFIX}/www/api
    # API version
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1
	# USERS API ENDPOINTS------------------------------------------
	# Users base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users
	# Users add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/add
	# Users priv add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/add/privs
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/add/privs/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/add/privs
	# Users group add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/add/groups
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/add/groups/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/add/groups
	# Users delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/delete
	# Users priv delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/delete/privs
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/delete/privs/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/delete/privs
	# Users group delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/delete/groups
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/delete/groups/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/delete/groups
	# Users modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/modify
	# Authservers base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/authservers
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/authservers/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/authservers
	# Authservers delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/authservers/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/delete
	# Authservers ldap
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/ldap
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/authservers/ldap/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/ldap
	# Authservers ldap add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/ldap/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/authservers/ldap/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/ldap/add
	# Authservers ldap delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/ldap/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/authservers/ldap/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/ldap/delete
	# Authservers radius
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/radius
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/authservers/radius/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/radius
	# Authservers radius delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/radius/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/users/authservers/radius/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/users/authservers/radius/delete
	# SYSTEM API ENDPOINTS----------------------------------------
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/
	# Version base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/version
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/version/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/version
	# API base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/api
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/api/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/api
	# API errors base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/api/errors
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/api/errors/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/api/errors
	# ARP base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/arp
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/arp/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/arp
	# ARP delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/arp/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/arp/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/arp/delete
	# Config base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/config
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/config/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/config
	# Hostname base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/hostname
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/hostname/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/hostname
	# Hostname modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/hostname/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/hostname/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/hostname/modify
	# DNS base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/dns
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/dns/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/dns
	# DNS modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/dns/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/dns/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/dns/modify
	# DNS delete servers
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/dns/delete
	# DNS delete servers
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/dns/delete/servers
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/dns/delete/servers/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/dns/delete/servers
	# Certificates base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/certificates
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/certificates/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/certificates
	# Certificates add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/certificates/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/certificates/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/certificates/add
	# Certificates delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/system/certificates/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/system/certificates/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/system/certificates/delete
	# STATUS API ENDPOINTS----------------------------------------
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/status/
	# CARP base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/status/carp
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/status/carp/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/status/carp
	# CARP base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/status/carp/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/status/carp/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/status/carp/modify

	# INTERFACES API ENDPOINTS----------------------------------------
	# Interfaces base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/interfaces
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/interfaces/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/interfaces
	# Interfaces add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/interfaces/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/interfaces/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/interfaces/add
	# Interfaces delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/interfaces/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/interfaces/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/interfaces/delete
	# Vlans base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/interfaces/vlans/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans
	# Vlans add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/interfaces/vlans/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans/add
	# Vlans delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/interfaces/vlans/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans/delete
	# Vlans modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/interfaces/vlans/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/interfaces/vlans/modify

	# ROUTING API endpoints----------------------------------------
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/routing
	# Gateways base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/routing/gateways
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/routing/gateways/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/routing/gateways

	# FIREWALL API ENPOINTS
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall
	# STATES base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/states
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/states/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/states
	# STATES size
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/states/size
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/states/size/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/states/size
	# STATES size modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/states/size/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/states/size/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/states/size/modify
	# NAT base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/nat/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat
	# NAT port forwards base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards
	# NAT port forwards add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards/add
	# NAT port forwards add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/nat/portforwards/delete
	# Virtual IP base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/virtualips
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/virtualips/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/virtualips
	# Virtual IP add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/virtualips/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/virtualips/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/virtualips/add
	# Virtual IP delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/virtualips/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/virtualips/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/virtualips/delete
	# Alias base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/aliases/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases
	# Alias add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/aliases/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/add
	# Alias add address
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/add/address
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/aliases/add/address/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/add/address
	# Alias delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/aliases/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/delete
	# Alias delete address
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/delete/address
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/aliases/delete/address/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/delete/address
	# Alias modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/aliases/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/aliases/modify
	# Rules base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/rules
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/rules/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/rules
	# Rules add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/rules/add
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/rules/add/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/rules/add
	# Rules delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/firewall/rules/delete
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/firewall/rules/delete/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/firewall/rules/delete

	# SERVICE API ENDPOINTS----------------------------------------
	# Services base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services
	# Base start
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/start
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/start/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/start
	# Base stop
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/stop
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/stop/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/stop
	# Base restart
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/restart
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/restart/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/restart
	# DHCPD base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dhcpd
	# DHCPD start
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dhcpd/start
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/dhcpd/start/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/dhcpd/start
	# DHCPD stop
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dhcpd/stop
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/dhcpd/stop/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/dhcpd/stop
	# DHCPD restart
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dhcpd/restart
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/dhcpd/restart/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/dhcpd/restart
	# DPINGER base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dpinger
	# DPINGER start
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dpinger/start
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/dpinger/start/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/dpinger/start
	# DPINGER stop
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dpinger/stop
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/dpinger/stop/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/dpinger/stop
	# DPINGER restart
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/dpinger/restart
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/dpinger/restart/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/dpinger/restart
	# NTPD base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/ntpd
	# NTPD start
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/ntpd/start
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/ntpd/start/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/ntpd/start
	# NTPD stop
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/ntpd/stop
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/ntpd/stop/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/ntpd/stop
	# NTPD restart
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/ntpd/restart
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/ntpd/restart/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/ntpd/restart
	# SSHD base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/sshd
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/sshd/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/sshd
	# SSHD modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/modify
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/sshd/modify/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/modify
	# SSHD start
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/start
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/sshd/start/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/start
	# SSHD stop
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/stop
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/sshd/stop/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/stop
	# SSHD restart
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/restart
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/sshd/restart/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/sshd/restart
	# SYSLOGD base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/syslogd
	# SYSLOGD start
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/syslogd/start
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/syslogd/start/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/syslogd/start
	# SYSLOGD stop
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/syslogd/stop
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/syslogd/stop/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/syslogd/stop
	# SYSLOGD restart
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/syslogd/restart
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/syslogd/restart/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/syslogd/restart
	# Unbound base
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/unbound/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/unbound
	# Unbound start
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/start
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/unbound/start/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/start
	# Unbound stop
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/stop
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/unbound/stop/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/stop
	# Unbound restart
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/restart
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/unbound/restart/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/restart
	# Unbound add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/add
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/add/hosts
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/unbound/add/hosts/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/add/hosts
	# Unbound delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/delete
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/delete/hosts
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/unbound/delete/hosts/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/delete/hosts
	# Unbound modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/modify
	${MKDIR} ${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/modify/hosts
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/www/api/v1/services/unbound/modify/hosts/index.php \
		${STAGEDIR}${PREFIX}/www/api/v1/services/unbound/modify/hosts

	# INSTALL OUR PKG INFO
	${MKDIR} ${STAGEDIR}${DATADIR}
	${INSTALL_DATA} ${FILESDIR}${DATADIR}/info.xml \
		${STAGEDIR}${DATADIR}
	@${REINPLACE_CMD} -i '' -e "s|%%PKGVERSION%%|${PKGVERSION}|" \
		${STAGEDIR}${DATADIR}/info.xml

.include <bsd.port.mk>