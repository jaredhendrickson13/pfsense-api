<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\ResourceLink;
use RESTAPI\Core\TestCase;
use const RESTAPI\Core\Globals\HAL_REL_NAMESPACE;

class APICoreResourceLinkTestCase extends TestCase
{
    /**
     * Checks that the package's HAL namespace is prefixed to custom HAL rel links
     */
    public function test_use_namespace()
    {
        $namespaced_link = new ResourceLink('test', 'test', use_namespace: true);
        $non_namespaced_link = new ResourceLink('test', 'test', use_namespace: false);
        $this->assert_equals($namespaced_link->rel, HAL_REL_NAMESPACE . ':test');
        $this->assert_equals($non_namespaced_link->rel, $non_namespaced_link->rel);
    }

    /**
     * Ensure custom attributes are populated by extra unnamed parameters during construction.
     */
    public function test_custom_attributes()
    {
        $link = new ResourceLink('test', 'test', some_custom_param: true);
        $this->assert_equals($link->custom_attributes, ['some_custom_param' => true]);
    }

    /**
     * Checks that the `to_representation()` method correctly converts the ResourceLink into a HAL resource link item.
     */
    public function test_to_representation()
    {
        $link = new ResourceLink(rel: 'test', href: 'test', use_namespace: false, example_attr: true);
        $this->assert_equals($link->to_representation(), [
            'href' => $link->href,
            'method' => $link->method,
            'example_attr' => true,
        ]);
    }
}
