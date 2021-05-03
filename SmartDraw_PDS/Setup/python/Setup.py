# ...
# ModuleFinder can't handle runtime changes to __path__, but win32com uses them
#try:
	# py2exe 0.6.4 introduced a replacement modulefinder.
	# This means we have to add package paths there, not to the built-in
	# one.  If this new modulefinder gets integrated into Python, then
	# we might be able to revert this some day.
	# if this doesn't work, try import modulefinder
#	try:
#		import py2exe.mf as modulefinder
#	except ImportError:
#		import modulefinder
#	import win32com, sys
#	for p in win32com.__path__[1:]:
#		modulefinder.AddPackagePath("win32com", p)
#	for extra in ["win32com.shell"]:
#		__import__(extra)
#		m = sys.modules[extra]
#		for p in m.__path__[1:]:
#			modulefinder.AddPackagePath(extra, p)
#except ImportError:
	# no build path setup, no worries.
#	pass

from distutils.core import setup
import sys, os, py2exe
from glob import glob

#data_files = [(glob(r'C:\Python27\Lib\site-packages\win32comext\shell\*.*'))]
packages = ['win32com' , 'win32api' , 'pythoncom']
setup(	
#	data_files=data_files,
	options={'py2exe':{
		"packages":packages,
		"includes":['sysconfig','modulefinder','sets','code','site','os','locale','math','re','ConfigParser','abc','xml','xml.dom.*','xml.etree.*','glob'],
		}},
	scripts=['App.py' , 'vad2dra.py' , 'dra.py']
	)
