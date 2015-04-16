# TODO: use the id from pid() commented below instead and remove the site_id
# once the admin URL has been migrated.
BACKCOMPAT_ID_MAIN = 17732923532771328
BACKCOMPAT_ID_POS_ELEM = 17732923532771330
BACKCOMPAT_ID_TREE_ELEM = 17732923532771329

pages = [{
    'id': pid(0),
    'up_id': pid(0, package_name='core'),
    'title': 'admin',
    'content1': '',
}, {
    #'id': pid(1),
    'id': BACKCOMPAT_ID_MAIN,
    'up_id': pid(0),
    'rank': 0,
    'smart_url_path': ':admin_main',
    'content1': 'file:admin_main.html',
    'do_not_use_template': True,
}, {
    #'id': pid(2),
    'id': BACKCOMPAT_ID_POS_ELEM,
    'up_id': pid(0),
    'rank': 1,
    'smart_url_path': ':admin_pos_elem',
    'content1': 'file:admin_pos_elem.html',
    'do_not_use_template': True,
}, {
    #'id': pid(3),
    'id': BACKCOMPAT_ID_TREE_ELEM,
    'up_id': pid(0),
    'rank': 2,
    'smart_url_path': ':admin_tree_elem',
    'content1': 'file:admin_tree_elem.html',
    'do_not_use_template': True,
}]
