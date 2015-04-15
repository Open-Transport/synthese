# Example import configuration.

import_templates = [{
    'id': 'my_import',
    'label': 'My Import (Trident)',
    'defaults': [
        ('ds', '16607027920896001'),
        ('itt', '1'),
        ('mr', '1'),
        ('impstp', '1'),
        ('asa', '1'),
        ('impjun', '0'),
        ('dtd', '5'),
        {
            'id': 'dr',
            'label': 'Directory containing files to import',
            'type': 'directory',
            'default': 'files',
        },
        ('clean_old_data', '1'),
        ('from_today', '1'),
    ],

    'admins': [('root', 'admin@example.com')],
    'uploaders': [('uploader', ['uploader@rcsmobility.com'])],

    'run_results_notifications': {
         'load': ('alice@example.com',),
         'warn': ('bob@example.com',),
     },
}]
