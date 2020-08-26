<?php
# Copyright 2020 - Jared Hendrickson
# IMPORTS
require_once("api/api_models/APIFirewallNat.inc");
(new APIFirewallNat())->listen();