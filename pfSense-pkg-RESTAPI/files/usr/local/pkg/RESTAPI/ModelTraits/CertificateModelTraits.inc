<?php

namespace RESTAPI\ModelTraits;

use RESTAPI\Core\Model;

trait CertificateModelTraits
{
    /**
     * Returns a list of available elliptic curve names for ECDSA key pairs.
     * @returns array The list of available elliptic curve names.
     */
    public static function get_ecname_choices(): array {
        # Obtain the available curve list from pfSense's built-in cert_build_curve_list function
        return array_keys(cert_build_curve_list());
    }

    /**
     * Returns a list of available digest algorithms for signing certificates.
     * @returns array The list of available digest algorithms.
     */
    public static function get_digest_alg_choices(): array {
        # Obtain the available digest algorithms from pfSense's built-in $openssl_digest_algs global
        global $openssl_digest_algs;
        return $openssl_digest_algs;
    }

    /**
     * Returns a list of available country codes for the certificate.
     * @returns array The list of available country codes.
     */
    public static function get_country_choices(): array {
        # Obtain the available country codes from pfSense's built-in get_cert_country_codes function
        return array_keys(get_cert_country_codes());
    }

    /**
     * Converts this Certificate object's DN values into a X509 DN array.
     * @param Model $model The Certificate* Model object to convert to a X509 DN array.
     * @returns array The X509 DN array.
     */
    private function to_x509_dn(Model $model): array {
        # Define static DN values
        $dn = ['commonName' => $model->dn_commonname->value, 'subjectAltName' => []];

        # Add countryName if it was given
        if ($model->dn_country->value) {
            $dn['countryName'] = $model->dn_country->value;
        }
        # Add stateOrProvinceName if it was given
        if ($model->dn_state->value) {
            $dn['stateOrProvinceName'] = $model->dn_state->value;
        }
        # Add localityName if it was given
        if ($model->dn_city->value) {
            $dn['localityName'] = $model->dn_city->value;
        }
        # Add organizationName if it was given
        if ($model->dn_organization->value) {
            $dn['organizationName'] = $model->dn_organization->value;
        }
        # Add organizationalUnitName if it was given
        if ($model->dn_organizationalunit->value) {
            $dn['organizationalUnitName'] = $model->dn_organizationalunit->value;
        }

        # Loop through the SAN fields and add them to the subjectAltName array accordingly
        foreach ($model->dn_dns_sans->value as $san) {
            $dn['subjectAltName'][] = "DNS:$san";
        }
        foreach ($model->dn_email_sans->value as $san) {
            $dn['subjectAltName'][] = "email:$san";
        }
        foreach ($model->dn_ip_sans->value as $san) {
            $dn['subjectAltName'][] = "IP:$san";
        }
        foreach ($model->dn_uri_sans->value as $san) {
            $dn['subjectAltName'][] = "URI:$san";
        }

        # Piece together the subjectAltName array into a comma-separated string
        $dn['subjectAltName'] = implode(',', $dn['subjectAltName']);

        return $dn;
    }
}