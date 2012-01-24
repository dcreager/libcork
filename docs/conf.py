# -*- coding: utf-8 -*-

import sys, os

extensions = ['sphinx.ext.mathjax']
source_suffix = '.rst'
master_doc = 'index'
project_name = u'libcork'
project_slug = u'libcork'
company = u'RedJack, LLC'
copyright_years = u'2011-2012'

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
    ('index', 'libcork', u'libcork documentation',
     [u'RedJack, LLC'], 1)
]

texinfo_documents = [
  ('index', 'libcork', u'libcork documentation',
   u'RedJack, LLC', 'libcork', 'One line description of project.',
   'Miscellaneous'),
]
