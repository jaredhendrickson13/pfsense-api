<?php
# Copyright 2020 - Jared Hendrickson
require_once("api/api_models/APISystemHostname.inc");
(new APISystemHostname())->listen();