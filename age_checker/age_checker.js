var age_checker = {};

(function ( $ ) {  
  age_checker.init = function( context, settings ) { 
  if ( $.cookie("age_checker") == null ) {
    age_checker.overlay = $('<div id="age_checker_overlay"></div>');
    age_checker.popup   = $('#age_checker_verification_popup');
    age_checker.popup.css('left', '50%');
    age_checker.popup.css('top', '50%');
    age_checker.popup.css('margin-left', '-250px');
    age_checker.popup.css('margin-top', '-160px');
    if ( Drupal.settings.age_checker.background_image ) {
      $('body').append(age_checker.overlay);
      $("#age_checker_overlay").css("background-image",'url('+Drupal.settings.age_checker.background_image+')');
      age_checker.overlay.fadeIn(80, function() {
        age_checker.popup.fadeIn(250);
      });
    }
	else {
      age_checker.popup.fadeIn(500);
    }
  }
}
  /**
   * Control age limit.
   */
age_checker.verify = function () { 
  var now   = new Date();
  var date  = now.getDate();
  var month = now.getMonth() + 1;
  var year  = now.getFullYear();
  var age_checker_month = jQuery("#age_checker_month").val();
  var age_checker_day   = jQuery("#age_checker_day").val();
  var age_checker_year  = jQuery("#age_checker_year").val();
  var age               = year - age_checker_year;     
  var dobdate           = new Date(age_checker_year, age_checker_month - 1, age_checker_day).getTime();
  var today             = new Date().getTime();
  var threshold_age     = Drupal.settings.age_checker.threshold_age;
  var leapyear          = ((age_checker_year % 4 == 0) && (age_checker_year % 100 != 0)) || (age_checker_year % 400 == 0);
  var blank_err_message = Drupal.settings.age_checker.blank_err_message;
  var under_age_err_msg = Drupal.settings.age_checker.under_age_err_msg;
  var dateformat_error  = Drupal.settings.age_checker.dateformat_error;
  var date_rnge_err_msg = Drupal.settings.age_checker.date_rnge_err_msg;
  
  if ( age_checker_month > month ) {
    age--;
  } 
  else {
    if( age_checker_month == month && age_checker_day >= date ) {
	  age--;
	}	
  }
  // if current year, form not set
  if ( age_checker_month == '' || age_checker_day == '' || age_checker_year == '' || age_checker_month == Drupal.t('MM') || age_checker_day == Drupal.t('DD') || age_checker_year == Drupal.t('YYYY') ) { 
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(blank_err_message);
    return false;
  }     
  else if ( (age_checker_year < 1900) || (age_checker_year > year) ) {
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(date_rnge_err_msg);
    return false;
  }
  else if ( age_checker_year.length != 4 ) { 
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(dateformat_error);
    return false;    
  }
  else if ( (age_checker_month < 1 || age_checker_month > 12) ){
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(dateformat_error);
    return false;
  } 
  else if ( age_checker_day < 1 || age_checker_day > 31 ) {
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(dateformat_error);
    return false;        
  } 
  else if ( (age_checker_month == 4 || age_checker_month == 6 || age_checker_month == 9 || age_checker_month == 11) && age_checker_day == 31 ) {
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(dateformat_error);
    return false
  }
  else if ( age_checker_month == 2 && (age_checker_day == 29 && !leapyear || age_checker_day>29) ) {
    // check for february 29th
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(dateformat_error);
    return false;        
  } 
  else if( today - dobdate < 0 ) {
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(date_rnge_err_msg);
    return false;
  }
  else if ( !parseInt(age_checker_month) || !parseInt(age_checker_day) || !parseInt(age_checker_year) ) { 
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(dateformat_error);
    return false;    
  } 
  else if ( age < threshold_age ) {
    document.getElementById('age_checker_error_message').innerHTML = Drupal.t(under_age_err_msg);
    setTimeout("age_checker.deny()", 2000);      
    return false;
  } 
  else { 
    // age limit ok
    age_checker.popup.fadeOut(250);
    age_checker.overlay.fadeOut(500);
    $.cookie('age_checker', '1', { path: "/", expires: Drupal.settings.age_checker.cookie_expiration });
    var langcode = $('#languagecode').val();
    if ( $('#agecheckerformredirecuri').val() ) {
      $('#agecheckerformredirecuri').val( window.location.pathname + window.location.search );
    }
	else {
      document.location = Drupal.settings.basePath + langcode;
    }      
  }
  return true;
}
age_checker.selectcountryonchange = function( selectval ){ 
  document.location = Drupal.settings.basePath + $('#languagecode').val(); 
}
  
age_checker.nextbox = function( fldobj, nbox )
{ 
  var language_selectvalue = $('#languagecode').val();
  if( language_selectvalue === undefined || language_selectvalue == null || language_selectvalue.length <= 0 )
    {
      if ( fldobj.value.length > 1 && nbox == 1 ) {
        document.forms['age_checker_form'].elements[1].focus();
      }
	  else if ( fldobj.value.length > 1 && nbox == 2 ) {
        document.forms['age_checker_form'].elements[2].focus();
      }
    }
    else {
      if ( fldobj.value.length > 1 && nbox == 1 ) { 
        document.forms['age_checker_form'].elements[2].focus();
      }
	  else if ( fldobj.value.length > 1 && nbox == 2 ) {
        document.forms['age_checker_form'].elements[3].focus();
      }
    }
}

age_checker.deny = function() {  
  // Redirect user elsewhere.
  var language_code = $('#languagecode').val();
  var language_value;
  if( language_code === undefined || language_code == null || language_code.length <= 0 ) {
    language_value = '';
  }
  else {
    language_value = language_code + "/";
  }
  var redirect_url = Drupal.settings.age_checker.redirecturl;
  if ( redirect_url.indexOf('http://') > -1 || redirect_url.indexOf('https://') > -1 ) { 
    window.location = redirect_url;
    return false;
  }     
  else if ( redirect_url.indexOf('node') > -1 ) { 
    window.location = Drupal.settings.basePath + redirect_url;    
    return false;
  } 
  else {
    window.location = Drupal.settings.basePath + language_value  +  redirect_url;    
    return false;
  }  
}
Drupal.behaviors.age_checker = {
 attach : age_checker.init
}
})(jQuery);
