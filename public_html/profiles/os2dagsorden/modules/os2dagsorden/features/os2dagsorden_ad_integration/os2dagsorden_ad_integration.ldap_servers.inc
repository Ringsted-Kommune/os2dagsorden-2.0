<?php
/**
 * @file
 * os2dagsorden_ad_integration.ldap_servers.inc
 */

/**
 * Implements hook_default_ldap_servers().
 */
function os2dagsorden_ad_integration_default_ldap_servers() {
  $export = array();

  $ldap_servers_conf = new stdClass();
  $ldap_servers_conf->disabled = FALSE; /* Edit this to true to make a default ldap_servers_conf disabled initially */
  $ldap_servers_conf->api_version = 1;
  $ldap_servers_conf->sid = 'ldap_syddjursnet';
  $ldap_servers_conf->name = '706DC02';
  $ldap_servers_conf->status = TRUE;
  $ldap_servers_conf->ldap_type = 'ad';
  $ldap_servers_conf->address = '10.1.1.22';
  $ldap_servers_conf->port = 389;
  $ldap_servers_conf->tls = FALSE;
  $ldap_servers_conf->bind_method = TRUE;
  $ldap_servers_conf->binddn = 'DrupalAdLookup';
  $ldap_servers_conf->bindpw = 'w0b8cBKkYDs0FIUIKnWuRg==';
  $ldap_servers_conf->basedn = array(
    0 => 'DC=intern,DC=syddjurs,DC=dk',
  );
  $ldap_servers_conf->user_attr = 'sAMAccountName';
  $ldap_servers_conf->account_name_attr = '';
  $ldap_servers_conf->mail_attr = 'mail';
  $ldap_servers_conf->mail_template = '';
  $ldap_servers_conf->allow_conflicting_drupal_accts = FALSE;
  $ldap_servers_conf->unique_persistent_attr = '';
  $ldap_servers_conf->user_dn_expression = '';
  $ldap_servers_conf->ldap_to_drupal_user = '';
  $ldap_servers_conf->testing_drupal_username = 'petsoe';
  $ldap_servers_conf->group_object_category = '';
  $ldap_servers_conf->search_pagination = TRUE;
  $ldap_servers_conf->search_page_size = 1000;
  $ldap_servers_conf->weight = 0;
  $export['ldap_syddjursnet'] = $ldap_servers_conf;

  return $export;
}
