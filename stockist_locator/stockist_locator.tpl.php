<?php
/**
 * @file created
 * Created date July 01, 2014
 * @author Rahul Sharma <rahulsh@hcl.com>
 */
global $base_url, $lng, $lat;
?>
<script type="text/javascript">
function hideText(){
	var msg = '<?php echo t("Please enter your postal code, and your city");?>';
	if(document.getElementById('edit-zipcode').value==msg) {
		document.getElementById('edit-zipcode').value='';
	} else if(document.getElementById('edit-zipcode').value==''){
		document.getElementById('edit-zipcode').value=msg;
	}
}
</script>
		<section>
			<div>
				<fieldset>
					<div class="form-verticle border-all fill-lightgray">
						<div class="form-item">
							<?php print drupal_render($form['stockist_locator_frontend_category']); ?>
							<?php print drupal_render($form['zipcode']); ?>
							<?php print drupal_render($form['add_button']); ?>
						</div>
						<div style="bottom: 24px; padding-left: 320px; height:4px; position: relative;" id='processing'></div>
					</div>
				</fieldset>
			</div>
			<div class="google-map-wrapper margin-top-10">
				<script type="text/javascript" src="http://maps.googleapis.com/maps/api/js?sensor=false"></script>
				<script type="text/javascript" src="http://google-maps-utility-library-v3.googlecode.com/svn/trunk/markermanager/src/markermanager.js"></script>
				<div id="map_canvas" style="width:100%; height:400px;"></div>
			</div>
		</section>		
   
<?php

$modulePath = drupal_get_path('module', 'stockist_locator');
$ajaxImagePath = $base_url.'/'.$modulePath;
require_once($modulePath.'/mobiledetect.php');
$mobileDetect = locator_mobile_device_detect(true,true,true,true,true,true,false,false,false);
$mobileDetects = isset($mobileDetect[0]) ? $mobileDetect[0]:'';	
$mobile = '';
if($mobileDetects) {
	$mobile = 'yes';
} else {
	$mobile = 'no';
}
 ?>	
<script type="text/javascript">

var map;
var mgr;
var markers = [];

function initialize() {
	<?php
	$error = form_set_error();
	if (empty($error)) {
	?>	
		
		if (navigator.geolocation) {
			navigator.geolocation.getCurrentPosition(successFunction, errorFunction);
		}
		
	<?php } ?>
	
	var lat = '<?php echo variable_get('stockist_locator_latitude', '');?>';
	var lng = '<?php echo variable_get('stockist_locator_longitude', '');?>';
	
	//default map display as per defined in admin configuration settings
	var myOptions = {
		zoom:4,
		center: new google.maps.LatLng(lat, lng),
		mapTypeId: google.maps.MapTypeId.ROADMAP
	};
	map = new google.maps.Map(document.getElementById("map_canvas"),myOptions);
}

//To get latitude and longitude
function successFunction(position) {
    var lat = position.coords.latitude;
    var lng = position.coords.longitude;
	var is_mobile = '<?php echo $mobile;?>';
	if(is_mobile=='no') {
		var lat = '<?php echo variable_get('stockist_locator_latitude', '');?>';
		var lng = '<?php echo variable_get('stockist_locator_longitude', '');?>';
	}
	var myOptions = {
        zoom:4,
        center: new google.maps.LatLng(lat, lng),
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById("map_canvas"),myOptions);
    codeLatLng(lat, lng, is_mobile);
}


// error function if failure occurs.
function errorFunction(error){
	switch(error.code){
		case error.TIMEOUT:
		break;
        case error.POSITION_UNAVAILABLE:
        break;
        case error.PERMISSION_DENIED:                        
        break;
        case error.UNKNOWN_ERROR:
        break;
    }   
}    

function codeLatLng(lat, lng, is_mobile) {
	var latlng = new google.maps.LatLng(lat, lng);
	var geocoder= new google.maps.Geocoder();
	var resultxml;
	if (window.XMLHttpRequest) {// code for IE7+, Firefox, Chrome, Opera, Safari
	  	xmlhttp=new XMLHttpRequest();
	}
	else{// code for IE6, IE5
	  xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	var img = '<img alt="loading.." src="<?php echo $ajaxImagePath; ?>/images/ajax_progress.gif">';
	document.getElementById('processing').innerHTML = img;
	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState==4){
			var resultxml = xmlhttp.responseText;
			//alert(xmlhttp.responseText);
			var msg='';
			if (window.DOMParser) {
				parser=new DOMParser();
				xmlDoc=parser.parseFromString(resultxml,"text/xml");
			}else { // Internet Explorer
				xmlDoc=new ActiveXObject("Microsoft.XMLDOM");
				xmlDoc.async=false;
				xmlDoc.loadXML(resultxml);
			}
			
			var markers = xmlDoc.documentElement.getElementsByTagName('marker');
			var bounds =  new google.maps.LatLngBounds();
			if(markers.length == 0) {
				bounds.extend(latlng);
				msg = 'No stores found near you.';
			}
		
			geocoder.geocode({'latLng': latlng}, function(results, status) {
				if (status == google.maps.GeocoderStatus.OK) {
					if (results[1]) {
						mgr = new MarkerManager(map);
						google.maps.event.addListener(mgr, 'loaded', function() {		
							//current location marker
							if(is_mobile=='yes') {
								var current_marker = add_marker('<?php echo t("You are here.");?>', latlng, '<?php echo t("You are here.");?>', map,'yes','');
								mgr.addMarker(current_marker, 0);
							} else  {
								var current_marker = add_marker('<?php echo t("Center Point.");?>', latlng, '<?php echo t("Center Point.");?>', map,'yes','');
								mgr.addMarker(current_marker, 0);
							}
							//					
							
							for (var i = 0; i < markers.length; i++) {
								var note = '<div><b><?php echo t("Storename");?>:</b> ' + markers[i].getAttribute("storename")
									+ '</div><div><b><?php echo t("Address");?>:</b> ' + markers[i].getAttribute("address1") +' '+markers[i].getAttribute("address2")
									+ '</div><div><b><?php echo t("City");?>:</b> ' + markers[i].getAttribute("city")
									+ '</div><div><b><?php echo t("State");?>:</b> ' + markers[i].getAttribute("state")
									+ '</div><div><b><?php echo t("Category");?>:</b> ' + markers[i].getAttribute("category_name")
									+ '</div><div><b><?php echo t("Zipcode");?>:</b> ' + markers[i].getAttribute("zipcode")
									+ '</div><div><b><?php echo t("Phone");?>:</b> ' + markers[i].getAttribute("phone")
									+ '</div><div><b><?php echo t("Email");?>:</b> ' + markers[i].getAttribute("email")
									+ '</div><div><b><?php echo t("Website Url");?>:</b> ' + markers[i].getAttribute("website_url")
									+ '</div>';
								var point = new google.maps.LatLng(parseFloat(markers[i].getAttribute("latitude")), parseFloat(markers[i].getAttribute("longitude")));
								var title = markers[i].getAttribute("storename");
								var iconUrl = markers[i].getAttribute("category_url");
								var marker = add_marker(title, point, note, map,'no',iconUrl);
								mgr.addMarker(marker, 0);
								bounds.extend(point);
							}
							map.fitBounds(bounds);
							if(markers.length == 1 || markers.length == 0){
								map.setZoom(10);
							} else {
								map.setZoom(Math.max(4, map.getZoom()));
							}
							mgr.refresh();
						});
					}
				} else {
					alert("Geocoder failed due to: " + status);
				}
			});
			document.getElementById('processing').innerHTML = msg;
		}
	}
	var url = '<?php echo $base_url; ?>';
	xmlhttp.open("POST",url+"/stockist_locator/ajax?page=first&ajax=true&lat="+lat+"&lng="+lng,true);
	xmlhttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded'); 
	xmlhttp.send();
}

//function in case of custom search
function customSearch() {
	var lat = '<?php echo variable_get('stockist_locator_latitude', '');?>';
	var lng = '<?php echo variable_get('stockist_locator_longitude', '');?>';
	//map refresh in case of custom search
	var myOptions = {
        zoom:6,
        center: new google.maps.LatLng(lat, lng),
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById("map_canvas"),myOptions);
	
	//in case of default text value should be considered as empty
	if(document.getElementById('edit-zipcode').value=='Please enter your postal code, and your city') {
		var zipcode = '';
	} else {
		var zipcode = document.getElementById('edit-zipcode').value;
	}
	
	//fetching values of checkboxes in cbResults variable
	var cbResults='';
	var form = document.forms['stockist-locator-ajax-form'];
	for (var i = 0; i < form.elements.length; i++ ) {
        if (form.elements[i].type == 'checkbox') {
            if (form.elements[i].checked == true) {
                cbResults += "'"+form.elements[i].value+"'" + ',';
            }
        }
    }
	cbResults = cbResults.substring(0, cbResults.length -1);

	var resultxml;
	var is_mobile = '<?php echo $mobile;?>';
	if (window.XMLHttpRequest) {// code for IE7+, Firefox, Chrome, Opera, Safari
	  	xmlhttp=new XMLHttpRequest();
	}
	else{// code for IE6, IE5
	  xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	var img = '<img alt="loading.." src="<?php echo $ajaxImagePath; ?>/images/ajax_progress.gif">';
	document.getElementById('processing').innerHTML = img;
	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState==4){
			var resultxml = xmlhttp.responseText;
			//alert(xmlhttp.responseText);
			var msg='';
			if (window.DOMParser) {
				parser=new DOMParser();
				xmlDoc=parser.parseFromString(resultxml,"text/xml");
			}else { // Internet Explorer
				xmlDoc=new ActiveXObject("Microsoft.XMLDOM");
				xmlDoc.async=false;
				xmlDoc.loadXML(resultxml);
			}
			
			//markers[0].setMap(null);
			var markers = xmlDoc.documentElement.getElementsByTagName('marker');
			var bounds =  new google.maps.LatLngBounds();
			if(markers.length == 0) {
				var latlng = new google.maps.LatLng(lat, lng);
				bounds.extend(latlng);
				msg = 'No stores found near you.';
			} else {
				var custom_lat = markers[0].getAttribute("custom_lat");
				var custom_lng = markers[0].getAttribute("custom_lng");
				var customlatlng = new google.maps.LatLng(custom_lat, custom_lng);
			}
			
			mgr = new MarkerManager(map);
			google.maps.event.addListener(mgr, 'loaded', function() {
				//current location marker
				if(customlatlng) {
					var current_marker = add_marker('<?php echo t("Center Point.");?>', customlatlng, '<?php echo t("Center Point.");?>', map,'yes','');
					mgr.addMarker(current_marker, 0);
				}
				
				//
				for (var i = 0; i < markers.length; i++) {
					var note = '<div><b><?php echo t("Storename");?>:</b> ' + markers[i].getAttribute("storename")
						+ '</div><div><b><?php echo t("Address");?>:</b> ' + markers[i].getAttribute("address1") +' '+markers[i].getAttribute("address2")
						+ '</div><div><b><?php echo t("City");?>:</b> ' + markers[i].getAttribute("city")
						+ '</div><div><b><?php echo t("State");?>:</b> ' + markers[i].getAttribute("state")
						+ '</div><div><b><?php echo t("Category");?>:</b> ' + markers[i].getAttribute("category_name")
						+ '</div><div><b><?php echo t("Zipcode");?>:</b> ' + markers[i].getAttribute("zipcode")
						+ '</div><div><b><?php echo t("Phone");?>:</b> ' + markers[i].getAttribute("phone")
						+ '</div><div><b><?php echo t("Email");?>:</b> ' + markers[i].getAttribute("email")
						+ '</div><div><b><?php echo t("Website Url");?>:</b> ' + markers[i].getAttribute("website_url")
						+ '</div>';
					var point = new google.maps.LatLng(parseFloat(markers[i].getAttribute("latitude")), parseFloat(markers[i].getAttribute("longitude")));
					var title = markers[i].getAttribute("storename");
					var iconUrl = markers[i].getAttribute("category_url");
					var marker = add_marker(title, point, note, map,'no',iconUrl);
					mgr.addMarker(marker, 0);
					bounds.extend(point);
				}
				map.fitBounds(bounds);
				if(markers.length == 1 || markers.length == 0){
					map.setZoom(10);
				} else {
					map.setZoom(Math.max(4, map.getZoom()));
				}
				mgr.refresh();
			});
			document.getElementById('processing').innerHTML = msg;
		}
	}
	var url = '<?php echo $base_url; ?>';
	xmlhttp.open("POST",url+"/stockist_locator/ajax?page=second&ajax=true&zipcode="+zipcode+"&categories="+cbResults,true);
	xmlhttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded'); 
	xmlhttp.send();
}


function add_marker(title, point, note, map,currentPos,iconUrl) {
	//var iconBase = 'https://maps.google.com/mapfiles/kml/shapes/';
	if(currentPos=='no') {//in case of stores
		var marker = new google.maps.Marker({
			title: title,
			map: map,
			position: point,
			clickable: true,
			icon: iconUrl
		});
	} else { //in case of current position
		var marker = new google.maps.Marker({
			title: title,
			map: map,
			position: point,
			clickable: true,
			icon: 'http://maps.google.com/mapfiles/arrow.png',
			shadow:'http://maps.google.com/mapfiles/arrowshadow.png',
		});
	}
    infowindow = new google.maps.InfoWindow();
	google.maps.event.addListener(marker, 'click', function() {
		infowindow.close();//hide the previously opened infowindow
		infowindow.setContent(note);
		//infowindow.content = note;
		infowindow.open(map, marker);
	});
    return marker;
}
initialize();
</script>