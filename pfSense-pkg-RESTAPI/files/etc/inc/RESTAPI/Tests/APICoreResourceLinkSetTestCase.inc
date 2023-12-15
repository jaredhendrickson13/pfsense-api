<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\ResourceLink;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\TestCase;

class APICoreResourceLinkSetTestCase extends TestCase
{
    /**
     * Ensures that only ResourceLink objects can be assigned to the ResourceLinkSet's `resource_link` property.
     */
    public function test_resource_links_must_be_resource_link() {
        $this->assert_throws_response(
            response_id: "RESOURCE_LINK_SET_CONTAINS_NON_RESOURCE_LINK",
            code: 500,
            callable: function () {
                new ResourceLinkSet(resource_links: ["not a ResourceLink object!"]);
            }
        );
        new ResourceLinkSet(resource_links: [new ResourceLink("test", "test")]);
    }
    
    /**
     * Checks that the `to_representation()` correctly represents nested ResourceLinks as a HAL _links array
     */
    public function test_to_representation() {
        $link1 = new ResourceLink(rel: "test1", href: "test1", use_namespace: false);
        $link2 = new ResourceLink(rel: "test2", href: "test2", use_namespace: false);
        $linkset = new ResourceLinkSet(resource_links: [$link1, $link2]);
        $this->assert_equals(
            $linkset->to_representation(),
            [
                "_links" => [
                    "test1" => ["href" => "test1"],
                    "test2" => ["href" => "test2"],
                ]
            ]
        );
    }

}
