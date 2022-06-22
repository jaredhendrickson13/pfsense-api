<?php
//   Copyright 2022 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.


# This script is intended to help API endpoints that need to reload pending interfaces changes. This allows
# interfaces to be reloaded in the background instead of waiting for all interfaces to be applied preventing
# API calls from receiving a 504 gateway timeout waiting for interfaces to be applied.
# ---------------------------------------------------------------------------------------------------------------
# Example: php -f apply_interfaces.php

require_once("api/framework/APITools.inc");

# Apply pending interface changes.
APIInterfaceApplyCreate::apply();
