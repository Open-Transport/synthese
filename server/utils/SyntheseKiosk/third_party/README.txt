chromedriver/lin64/chromedriver
  # NOTE: downloaded at install time. See install.py
  http://chromedriver.googlecode.com/files/chromedriver_linux64_23.0.1240.0.zip

chromedriver/win/chromedriver.exe
  http://chromedriver.googlecode.com/files/chromedriver_win_23.0.1240.0.zip

distribute-0.6.24.tar.gz:
  http://pypi.python.org/packages/source/d/distribute/distribute-0.6.24.tar.gz
  Note: the distribute version should match what is required in virtualenv.py

pip-1.0.2.tar.gz:
  http://pypi.python.org/packages/source/p/pip/pip-1.0.2.tar.gz#md5=47ec6ff3f6d962696fe08d4c8264ad49

r_kiosk-0.9.0-fx.xpi:
  https://addons.mozilla.org/en-US/firefox/addon/r-kiosk/
  install.rdf inside the .xpi was modified to replace:
    <em:id>{4D498D0A-05AD-4fdb-97B5-8A0AABC1FC5B}</em:id>
  with:
    <em:id>rkiosk@rkiosk.org/em:id>
  See http://code.google.com/p/selenium/issues/detail?id=3339

  Repackaging the .xpi also happens to fix: http://code.google.com/p/selenium/issues/detail?id=3204

virtualenv.py:
  Version 1.7
  Downloaded from https://raw.github.com/pypa/virtualenv/master/virtualenv.py
