<?php

function api_plugin_nginx($pluginparams) {
    global $g;
    /* Caller did not properly define data, exit early */
    if (!is_array($pluginparams) ||
        empty($pluginparams) ||
        empty($pluginparams['section'])) {
        return "";
    }

    switch ($pluginparams['section']) {
        case 'location':
            $result = "
		# This block is required to allow URI's without trailing slash or .php extension.
		# This block also enables PHP to handle PUT and DELETE requests for API functionality.
		location /api/v1/ {
			index index.php index.html index.htm;
			try_files \$uri/index.php =404; #  This line closes a potential security hole
			fastcgi_pass   unix:{$g['varrun_path']}/php-fpm.socket;
			fastcgi_index  index.php;
			fastcgi_param  SCRIPT_FILENAME  \$document_root\$fastcgi_script_name;
			fastcgi_param  HTTP_PROXY  \"\";
			fastcgi_read_timeout 180;
			include        /usr/local/etc/nginx/fastcgi_params;
		}";
            break;
        case 'server':
            $result = "";
            break;
        default:
            $result = "";
    }
    return $result;
}