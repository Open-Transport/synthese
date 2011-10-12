# TODO: use the id from pid() commented below instead and remove the site_id
# once the admin URL has been migrated.
BACKCOMPAT_ID_MAIN = 177329235327713281
BACKCOMPAT_ID_POS_ELEM = 177329235327713283
BACKCOMPAT_ID_TREE_ELEM = 177329235327713282
BACKCOMPAT_ADMIN_SITE_ID = 7036874417766401

pages = [{
    # TODO: make all other pages children of this one.
    'id': pid(0),
    'title': 'admin',
    'content1': '',
}, {
    #'id': pid(0),
    'id': BACKCOMPAT_ID_MAIN,
    'site_id': BACKCOMPAT_ADMIN_SITE_ID,
    'rank': 0,
    'smart_url_path': ':admin_main',
    'content1': 'file:admin_main.html',
    'do_not_use_template': True,
}, {
    #'id': pid(1),
    #'up_id': pid(0),
    'id': BACKCOMPAT_ID_POS_ELEM,
    'up_id': BACKCOMPAT_ID_MAIN,
    'site_id': BACKCOMPAT_ADMIN_SITE_ID,
    'rank': 0,
    'smart_url_path': ':admin_pos_elem',
    'content1': 'file:admin_pos_elem.html',
    'do_not_use_template': True,
}, {
    #'id': pid(2),
    #'up_id': pid(0),
    'id': BACKCOMPAT_ID_TREE_ELEM,
    'up_id': BACKCOMPAT_ID_MAIN,
    'site_id': BACKCOMPAT_ADMIN_SITE_ID,
    'rank': 1,
    'smart_url_path': ':admin_tree_elem',
    'content1': 'file:admin_tree_elem.html',
    'do_not_use_template': True,
}]
