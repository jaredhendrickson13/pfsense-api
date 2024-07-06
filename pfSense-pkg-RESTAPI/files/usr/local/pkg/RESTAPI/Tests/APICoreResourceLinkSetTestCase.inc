<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\ResourceLink;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\TestCase;
use const RESTAPI\Core\Globals\HAL_REL_NAMESPACE;

class APICoreResourceLinkSetTestCase extends TestCase {
    /**
     * Checks that the package's HAL namespace is prefixed to custom HAL rel links
     */
    public function test_use_namespace() {
        $namespaced_link = new ResourceLinkSet(rel: 'test', use_namespace: true);
        $non_namespaced_link = new ResourceLinkSet(rel: 'test', use_namespace: false);
        $this->assert_equals($namespaced_link->rel, HAL_REL_NAMESPACE . ':test');
        $this->assert_equals($non_namespaced_link->rel, $non_namespaced_link->rel);
    }

    /**
     * Ensures that only ResourceLink and ResourceLinkSet objects can be assigned to the ResourceLinkSet's
     * `resource_link` property.
     */
    public function test_resource_links_must_be_resource_link() {
        $this->assert_throws_response(
            response_id: 'RESOURCE_LINK_SET_CONTAINS_NON_RESOURCE_LINK',
            code: 500,
            callable: function () {
                new ResourceLinkSet(resource_links: ['not a ResourceLink object!']);
            },
        );
        new ResourceLinkSet(resource_links: [new ResourceLink('test', 'test')]);
        new ResourceLinkSet(resource_links: [new ResourceLinkSet(rel: 'test')]);
    }

    /**
     * Checks that the `to_representation()` correctly represents nested ResourceLinks and ResourceLinkSets as a HAL
     * _links array
     */
    public function test_to_representation() {
        $link1 = new ResourceLink(rel: 'test1', href: 'test1', use_namespace: false);
        $link2 = new ResourceLink(rel: 'test2', href: 'test2', use_namespace: false);
        $nested_linkset = new ResourceLinkSet(resource_links: [$link1, $link2], rel: 'nested');
        $linkset = new ResourceLinkSet(resource_links: [$link1, $link2, $nested_linkset]);
        $this->assert_equals($linkset->to_representation(), [
            '_links' => [
                'test1' => ['href' => 'test1', 'method' => 'GET'],
                'test2' => ['href' => 'test2', 'method' => 'GET'],
                'nested' => [['href' => 'test1', 'method' => 'GET'], ['href' => 'test2', 'method' => 'GET']],
            ],
        ]);
    }
}
