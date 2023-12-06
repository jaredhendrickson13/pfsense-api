<?php

namespace API\Tests;

use API\Core\Command;
use API\Core\TestCase;
use API\Models\InterfaceGroup;

class APIModelsInterfaceGroupTestCase extends TestCase
{
    /**
     * Ensures the interface groups members are properly assigned the group when created and updated, and removed from  
     * group when deleted.
     */
    public function test_crud() {
        # Create an InterfaceGroup object to use for testing
        $if_group = new InterfaceGroup(data: ["ifname" => "test", "members" => ["lan"], "descr" => "test"]);
        $if_group->create();
        
        # Check ifconfig and ensure the member interfaces are assigned to proper groups
        $real_members_create = [];
        foreach ($if_group->members->get_interface_obj()->model_objects as $member) {
            # Keep track of the real interface names
            $real_members_create[] = $member->if->value;
            
            # Check ifconfig for this member interface using its real if name
            $ifconfig = new Command("ifconfig ".$member->if->value);
            
            # Ensure the group is correctly assigned
            $this->assert_str_contains($ifconfig->output, "groups: test");
        }
        
        # Update the interface members
        $if_group->members->value = ["wan"];
        $if_group->update();

        # Check ifconfig and ensure the new member interfaces are assigned to proper groups
        $real_members_update = [];
        foreach ($if_group->members->get_interface_obj()->model_objects as $member) {
            # Keep track of the real interface names
            $real_members_update[] = $member->if->value;
            
            # Check ifconfig for this member interface using its real if name
            $ifconfig = new Command("ifconfig ".$member->if->value);

            # Ensure the group is correctly assigned
            $this->assert_str_contains($ifconfig->output, "groups: test");
        }
        
        # Ensure the old member interfaces are no longer assigned the group
        foreach ($real_members_create as $member_real) {
            # Check ifconfig for this member interface using its real if name
            $ifconfig = new Command("ifconfig ".$member_real);

            # Ensure the group is correctly assigned
            $this->assert_str_does_not_contain($ifconfig->output, "groups: test");
        }
        
        # Delete the interface group
        $if_group->delete();
        
        # Ensure the member interfaces are no longer assigned the group
        foreach ($real_members_update as $member_real) {
            # Check ifconfig for this member interface using its real if name
            $ifconfig = new Command("ifconfig ".$member_real);

            # Ensure the group is correctly assigned
            $this->assert_str_does_not_contain($ifconfig->output, "groups: test");
        }
    }
}
