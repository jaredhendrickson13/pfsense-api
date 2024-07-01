<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecApply;
use RESTAPI\Models\IPsecHostOverride;
use RESTAPI\Models\IPsecPhase1;

class APIModelsIPsecApplyTestCase extends TestCase {
    /**
     * Ensure the Model is correctly able to apply changes to the IPsec configuration and determine if there
     * are any pending changes.
     */
    public function test_apply(): void {
        # Make a change to the IPsec configuration and ensure we can see that there are pending changes
        $phase1 = new IPsecPhase1();
        $phase1->replace_all(data: []);
        $ipsec_apply = new IPsecApply(async: false);
        $this->assert_is_false($ipsec_apply->applied->value);

        # Apply the change and ensure that the applied status is now true
        $ipsec_apply->apply();
        $ipsec_apply = new IPsecApply(async: false);
        $this->assert_is_true($ipsec_apply->applied->value);
    }
}
