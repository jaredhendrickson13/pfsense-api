<?php
# Copyright 2020 - Jared Hendrickson
# IMPORTS
require_once("api/api_calls/APIAccessToken.inc");

# RUN API CALL
(new APIAccessToken())->listen();
