Stockist Locator
==============================

The module creates a content type Stockist locator in drupal admin interface. 
Site administrators can add/edit/delete stockist entries. On front end it 
displays stockist information in Google Map. Users has to option to filter the 
stockist based on category and location on Google Map.

The module provides a settings page where the Google map specific setting can be updated. 
There is also option to import and export stockist data.

Installation.
=============

1. Copy folder 'stockist_locator' to the "sites/all/modules" directory and enable the module.

2. If desired, give the administer  permissions that allow users of certain roles access 
the administration settings. You can do so on the admin/user/permissions page.

3. Go to the admin/config/system/stockist_locator page to update settings

4. Go to node/add/stockist-locator page from admin interface to add/edit delete Stockists

5. Use Sample CSV file for Data import, use "" for column data to avoid any data mismatch. 

6. Stockists can be viewed on google map from frontend menu url "http//yoursitename.com/stockist_locator".

7. To show separate images for Categories, add  images for taxonomies. You need to create a field name
"field_category_icon" of type image in taxonomy with CCK.
Uninstall
===============
During un installation, this module deletes all Stockist nodes, Stockist locator content type and delete all the variables 
used for this module.
