<?php
/**
 * @file
 * Code for the OS2Web Meeting Views - speaker paper (bullet point) view.
 */

/////////////////////////////////////////header START /////////////////////////////////////////
function os2dagsorden_meeting_views_bullet_point_breadcrumb() {
  global $base_url;
  $q = explode('/', $_GET['q']);
  $breadcrumb[] = l('Forsiden', $base_url);
  $breadcrumb[] .= l('Mødedetaljer', 'meeting/' . $q[1]);
  $breadcrumb[] .= '<span class="breadcrumb-active">Dagsordenspunkt</span>';
  drupal_set_breadcrumb($breadcrumb);
}

function os2dagsorden_meeting_views_bullet_point_print_send_to_friend() {
  if (module_exists('os2web_print_send_to_friend')) {
    global $base_url;
    $q = explode('/', $_GET['q']);
    $meeting_id = $q[1];
    $bp_id = $q[3];
    print '<div class="controls-right">';
    if (os2dagsorden_access_helper_check_user_can_send_bullet_point($meeting_id, $bp_id))
      print '<a href="' . $base_url . '?q=meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/send_to_friend_form" class="form-submit" rel="lightframe">Send til en ven</a>';

    print '<a href="' . $base_url . '?q=/print/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/print" class="form-submit print-button" target="_blank">Print</a>';
    print '</div>';
    print '<div class="clearfix"></div>';
  }
}

function os2dagsorden_meeting_views_bullet_point_print_to_pdf() {
  global $base_url;
  $q = explode('/', $_GET['q']);
  $meeting_id = $q[1];
  $bp_id = $q[3];
  print '<div class="controls-right">';
  print '<a href="' . $base_url . '?q=/printpdf/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/print" class="form-submit" target="_blank">PDF</a>';
  print '</div>';
}

/////////////////////////////////////////header END /////////////////////////////////////////

/////////////////////////////////////////PAGE START /////////////////////////////////////////
function os2dagsorden_meeting_views_bullet_point_page_access() {
  $q = explode("/", $_GET['q']);

  $meeting_id = $q[1];
  $bullet_point_id = $q[3];

  return os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);
}

function os2dagsorden_meeting_views_bullet_point_fields_indicator_bullet_point_has_notes($bp_id) {
  os2dagsorden_meeting_views_common_bullet_point_has_notes($bp_id);
}

function os2dagsorden_meeting_views_bullet_point_fields_title_navigation_buttons($bp_id) {
  $q = explode("/", $_GET['q']);

  // Extracting the current meetings id from the url
  $meeting_id = $q[1];

  // Loading the bullet point being view right now
  $bullet_point = node_load($bp_id);

  // Get all bullet points
  $bulletPoints = find_all_bullets($meeting_id);

  // Now lets find out wich bullet point that should be pointed to on prev and next arrow
  $prevNext = check_bullet_points($bulletPoints, $bp_id);

  global $base_url;

  if ($prevNext['prev'])
    print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $prevNext['prev'] . '" class="form-submit nav-button left"><</a>';
  if ($prevNext['next'])
    print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $prevNext['next'] . '" class="form-submit nav-button right">></a>';

  if ($bullet_point->field_os2web_meetings_bul_closed['und'][0][value] == 1)
    print '<div class="indicator-unlocked"></div>';
  if ($bullet_point->field_os2web_meetings_bul_pers['und'][0][value] == 1)
    print '<div class="indicator-personal"></div>';

  $title = $bullet_point->title;
  if (variable_get('os2dagsorden_show_bullet_case_nr', FALSE)) {
    $bp = node_load($bp_id);
    if ($bp->field_os2web_meetings_bul_case['und'] && !empty($bp->field_os2web_meetings_bul_case['und'][0]['value']))
      $title .= ' (' . $bp->field_os2web_meetings_bul_case['und'][0]['value'] . ')';
  }
  print $title;
}


// RINGSTED KOMMUNE
function os2dagsorden_meeting_views_bullet_point_fields_bottom_navigation_buttons($bp_id) {
  $q = explode("/", $_GET['q']);

  // Extracting the current meetings id from the url
  $meeting_id = $q[1];

  // Loading the bullet point being view right now
  $bullet_point = node_load($bp_id);

  // Get all bullet points
  $bulletPoints = find_all_bullets($meeting_id);

  // Now lets find out wich bullet point that should be pointed to on prev and next arrow
  $prevNext = check_bullet_points($bulletPoints, $bp_id);

  global $base_url;

  // Creating wrapping element
  print '<div id="bottom-bullet-navigation-buttons">';

  if ($prevNext['prev'])
    print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $prevNext['prev'] . '" class="form-submit nav-button left">Forrige punkt</a>';
  if ($prevNext['next'])
    print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $prevNext['next'] . '" class="form-submit nav-button right">Næste punkt</a>';

  // Creating wrapping element
  print '</div>';
}

// Finding all bullets based on the "meetings" could be a single meeting and/or "addendum to" and a ordinary meeting togheter.
function find_all_bullets($meeting_id) {

  $bulletPoints = array();
  
  $all_meetings = os2dagsorden_access_helper_get_meeting_and_tillaegs_meetings($meeting_id);
  foreach ($all_meetings as $nid) {
    $meeting = node_load($nid);

    foreach($meeting->field_os2web_meetings_bullets['und'] as $bullet) {

      // only saving bullets that the user have access to see
      if(os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet['target_id'])) {
          
        // Expecting that the bullets are read with the "addendum to" as the last meeting. 
          array_push($bulletPoints,  $bullet['target_id']);
      }
    }    
  }
  return $bulletPoints;
}

// Based on all bullet points, lets find the next and prev bullet point, based on all of the bullets that the user is allowed to see on this specific meeting.
function check_bullet_points($bulletPoints, $bp_id) {

  // gonna hold both values
  $prevNext = array();

  // Finding the index of the currently watch bullet point.
  $bulletPointArrayPlacement = array_search($bp_id, $bulletPoints);

  // Based on the currently watched bullet point, find the bullet right before and right after.
  if($bulletPointArrayPlacement != 0) {
    $prevNext['prev'] = $bulletPoints[$bulletPointArrayPlacement -1];
  }
  
  if($bulletPointArrayPlacement != count($bulletPoints) -1) {
    $prevNext['next'] = $bulletPoints[$bulletPointArrayPlacement +1];
  }

  return $prevNext;

}


function os2dagsorden_meeting_views_bullet_point_fields_attachments_and_content($meeting_id, $bp_id) {
  global $base_url;
  $bullet_point = node_load($bp_id);
  if (isset($bullet_point->field_os2web_meetings_attach['und'])) {
    print '<table><tr><td>';
    print '<div class="item-list item-list-dagsordenspunkt">';
    print '<ul class="ul-item-list-dagsordenspunkt pos-rel">';

    $bilags = array();

    $all_attachments = array();
    foreach ($bullet_point->field_os2web_meetings_attach['und'] as $attachment_id) {
      $hash = $attachment_id['target_id'];
      $all_attachments[$hash] = $attachment_id;
    }

    foreach ($all_attachments as $attachment_id) {
      $attachment = node_load($attachment_id['target_id']);

      if (!empty($attachment->field_os2web_meetings_bpa_file['und'])) { // if is bilag
        //saving bilags to the end
        if ($attachment->field_os2web_meetings_bpa_type['und'][0]['value'] == 'Sager') {
          $cases[] = $attachment;
        } else {
          $bilags[] = $attachment;
        }
      } else { //not bilag
        print '<li class="non-bilag">';

        $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
        print '<a href="' . $link . '" class="first-child last-child"><div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

        //attachment text - if any
        if (!empty($attachment->field_os2web_meetings_bpa_body['und'][0]['safe_value'])) {
          print '<div class="attachment_text_container">';

          print '<div class="attachment_text_full_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';
          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';

          print '</div>';
        }

        print '</li>';
      }
    }

    if ($bullet_point->title !== 'Bilag' && $bullet_point->title !== 'Sager') {
      if (!empty($cases) && !(variable_get('os2dagsorden_show_case_bullet_point', FALSE)) && variable_get('os2dagsorden_show_bilag_bullet_point', FALSE)) {
        _os2dagsorden_meeting_views_bullet_point_bilag_cases($cases, $meeting_id, $bp_id);
        if (!empty($bilags))
          _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point('Bilag', $bilags, $meeting_id, $bp_id, 'bilags');
      } else {
        if (!empty($bilags)) {
          if (variable_get('os2dagsorden_show_bilag_bullet_point', FALSE))
            _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point('Bilag', $bilags, $meeting_id, $bp_id, 'bilags');
          else
            _os2dagsorden_meeting_views_bullet_point_bilag_cases($bilags, $meeting_id, $bp_id);
        }
        if (!empty($cases)) {
          if (variable_get('os2dagsorden_show_case_bullet_point', FALSE))
            _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point(variable_get('os2dagsorden_sager_group_title', 'Sager'), $cases, $meeting_id, $bp_id, 'cases');
          else
            _os2dagsorden_meeting_views_bullet_point_bilag_cases($cases, $meeting_id, $bp_id);

        }
      }
    } else {
      _os2dagsorden_meeting_views_bullet_point_bilag_cases_with_content($bilags, $meeting_id, $bp_id);
      _os2dagsorden_meeting_views_bullet_point_bilag_cases_with_content($cases, $meeting_id, $bp_id);
    }
    print '</ul>';
    print '</div>';
    print '</td></tr></table>';
  }
}

function _os2dagsorden_meeting_views_bullet_point_bilag_cases($elements, $meeting_id, $bp_id) {
  global $base_url;
  foreach ($elements as $attachment) {
    print '<li class="bilag">';

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0]))
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    else
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);

    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">';
      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1)
        print '<div class="indicator-unlocked"></div>';
      else
        print '<div class="indicator-empty"></div>';

      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

      //attachment text - if any
      if (variable_get('os2dagsorden_show_bilag', TRUE)) {
        if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
          print '<div class="attachment_text_container">';

          print '<div class="attachment_text_trimmed_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';

          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';
          print('<a class="first-child last-child" href="' . $link . '" target="_blank">Læs mere...</a>');

          print '</div>';
        }
      }
    } else {
      print '<a class="first-child last-child">';
      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1)
        print '<div class="indicator-unlocked"></div>';
      else
        print '<div class="indicator-empty"></div>';

      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';
    }
    print '</li>';
  }
}

function _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point($bp_title, $elements, $meeting_id, $bp_id, $attachment_type) {
  global $base_url;
  print '<li class="bilags_cases ' . $attachment_type . '">';
  print '<div class="indicator-has-no-notes"></div>';
  print '<a class="first-child last-child">' . $bp_title . '</a>';
  print '<div class="bilags_cases_container">';

  //going through all bilags
  foreach ($elements as $attachment) {
    print '<div class="bilag">';

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0]))
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    else
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);

    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">';
    } else {
      print '<a class="first-child last-child">';
    }

    if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1)
      print '<div class="indicator-unlocked"></div>';
    else
      print '<div class="indicator-empty"></div>';
    print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

    print '</div>';
  }
  print '</div>';
  print '</li>';
}

function _os2dagsorden_meeting_views_bullet_point_bilag_cases_with_content($elements, $meeting_id, $bp_id) {
  global $base_url;
  //going through all bilags
  foreach ($elements as $attachment) {
    print '<li class="bilag">';

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    } else {
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);
    }

    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">';

      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }

      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

      //attachment text - if any
      if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
        //should bilag be visible
        if (variable_get('os2dagsorden_show_bilag', TRUE)) {
          print '<div class="attachment_text_container">';

          print '<div class="attachment_text_trimmed_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';

          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';
          print('<a class="first-child last-child" href="' . $link . '" target="_blank">Læs mere...</a>');

          print '</div>';
        }
      }
    } else {
      print '<a class="first-child last-child">';

      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }

      print $attachment->title . '</a>';
    }
    print '</li>';
  }
}

/////////////////////////////////////////PAGE END /////////////////////////////////////////

/////////////////////////////////////////SPEAKER PAPER START /////////////////////////////////////////
function os2dagsorden_meeting_views_bullet_point_fields_print_link($bp_id, $sp_id) {
  global $base_url;
  print "<div class='speaker-paper-print'>";
  print '<input class="form-submit print-button" type="button" onclick="window.open(';
  print(" ' " . $base_url . '/print/bullet-point/' . $bp_id . '/speaker-paper/' . $sp_id . '/print' . " ' ");
  print ')" value="Print Talepapir">';
  print "</div>";
}

function os2dagsorden_meeting_views_bullet_point_fields_create_new_speaker_paper() {
  global $base_url;
  $q = $_GET['q'];
  $q_exp = explode('/', $q);
  $bullet_point = node_load($q_exp[3]);
  print '<a href="' . $base_url . '/node/add/os2web-meetings-spaper?edit[field_os2web_meetings_sp_bullet][und][0][target_id]=' . $bullet_point->title . ' (' . $bullet_point->nid . ')&destination=' . $q . '"><div class="indicator-create-speaker-paper"></div>Nyt talepapir</a>';
}

function os2dagsorden_meeting_views_bullet_point_fields_shared_with($sp_id) {
  if (variable_get('os2dagsorden_shared_speaker_paper', TRUE)) {
    $sp = node_load($sp_id);
    $shared_with_html = null;

    $shared_with_str = null;
    $shared_with_group_str = null;
    if (isset($sp->field_os2web_meetings_sp_shared['und'])){
      foreach ($sp->field_os2web_meetings_sp_shared['und'] as $shared_with){
	    $shared_with_user = user_load($shared_with['target_id']);
	    $shared_with_str .= ($shared_with_user->field_user_full_name['und'])? $shared_with_user->field_user_full_name['und'][0]['value'] : $shared_with_user->name;

        $shared_with_str .= ', ';
      }
    }
    if (isset($sp->field_os2web_meetings_sp_shar_gr['und'])){
      foreach ($sp->field_os2web_meetings_sp_shar_gr['und'] as $shared_with){
        $shared_with_group = taxonomy_term_load($shared_with['tid']);
        $shared_with_group_str .= $shared_with_group->name;
        $shared_with_group_str .= ', ';
      }
    }
    if ($shared_with_str) {
      $shared_with_html = t('Deles med') . ': ' . substr($shared_with_str, 0, -2);
    }
    if ($shared_with_group_str) {
      if ($shared_with_html) {
        $shared_with_html .= '<br/>';
      }
      $shared_with_html .= t('Deles med grupper') . ': ' . substr($shared_with_group_str, 0, -2);
    }

    print $shared_with_html;
  }
}

function os2dagsorden_meeting_views_bullet_point_fields_filter($sp_id){
  return !os2dagsorden_access_helper_check_user_can_see_speaker_paper($sp_id, variable_get('os2dagsorden_shared_speaker_paper', TRUE));
}
/////////////////////////////////////////SPEAKER PAPER END /////////////////////////////////////////