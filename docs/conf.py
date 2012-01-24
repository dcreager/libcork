# -*- coding: utf-8 -*-

import sys, os

extensions = []
source_suffix = '.rst'
master_doc = 'index'
project_name = u'APPNAME'
project_slug = u'appname'
company = u'COMPANY'
copyright_years = u'2012'

default_role = 'c:func'
primary_domain = 'c'

# Grab version from git

import re
import subprocess
release = subprocess.check_output(["git", "describe"]).rstrip()
version = re.sub(r"-dev.*$", "-dev", release)

# Stuff below here probably doesn't need to be changed

project = project_name
copyright = copyright_years+u', '+company
templates_path = ['_templates']
exclude_patterns = ['_build']
pygments_style = 'sphinx'

html_theme = 'default'
html_style = 'docco-sphinx.css'
html_static_path = ['_static']
htmlhelp_basename = project_slug+'-doc'


latex_documents = [
  ('index', project_slug+'.tex', project_name+u' Documentation',
   company, 'manual'),
]

man_pages = [
    ('index', project_slug, project_name+u' Documentation',
     [company], 1)
]

texinfo_documents = [
  ('index', project_slug, project_name+u' Documentation',
   company, project_name, 'One line description of project.',
   'Miscellaneous'),
]
