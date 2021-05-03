#! -*- Encoding: Latin-1 -*-

import sys
import os
import locale
import math
import re
import win32com.client
import traceback
import pythoncom
import re
import glob
import time
import SmartDrawApp
from win32com.shell import shellcon,shell
from vad2dra import *
from dra import *
#from CADInteractive import *

"""
@brief  convert vad file to dra file
@author humkyung
@date   2014.05.04
"""
def vad2dra(drafilepath , vadfilepath):
    try:
        vadfile = vad()
        vadfile.parse(vadfilepath)
        converter = DraConverter(drafilepath)
        vadfile.convert(converter)
        converter.save()

        return True
    except Exception , e:
        print e.message

    return False

"""
@brief  fill match line data
@author humkyung
@date   2014.11.19
"""
def fillmatchlinedata(drafilepattern):
	try:
		dralist = []
		drafilelist = glob.glob(drafilepattern)
		for dra in drafilelist:
			aDra = cDra(dra)
			aDra.parse()
			dralist.append(aDra)

		for i in range(len(dralist)):
			for j in range(len(dralist)):
				if i == j: continue
				dralist[i].fillmatchlinedata(dralist[j])

		for i in range(len(dralist)):
			dralist[i].save()
		
		SmartDrawApp.UpdateDraFiles()

	except Exception , e:
		print e.message

	return False

"""
@brief  call MSTNJ and run commands
@author humkyung
@date   2015.01.06
"""
def CallMSTNJ(argv):
    app = win32com.client.Dispatch('Microstation.Application')
    app.Visible = 1
    app.MbeSendCommand("mdl load aDgnWriter \"" + argv + "\"")

    return True

# brief		load autocad
# author	humkyung
# date		2014.06.27
# param		none
def PyLoadAutoCAD():
	acad = win32com.client.Dispatch('AutoCAD.Application')
	acad.Visible = True

# brief		write output file to autocad
# author	humkyung
# date		2014.06.26
# param		argv[0] = file path will be saved , argv[1] = output file path
def PyWriteToAutoCAD(argv):
	acad = win32com.client.Dispatch('AutoCAD.Application')
	
	while 1:
		state = acad.GetAcadState()
		if state.IsQuiescent:
			break

	acad.Visible = True
	ver = re.findall('^[0-9.]+',acad.Version)[0]

	homedir = shell.SHGetFolderPath(0,shellcon.CSIDL_COMMON_APPDATA,0,0)
	plugin = homedir + '\\Autodesk\\ApplicationPlugins\\SmartDraw\\' + ver + '\\DwgWriter.NET.dll'
	plugin = plugin.replace('\\' , '\\\\')
	# load .net dll
	command = '(command \"netload\" \"' + plugin + '\") '

	f = open(argv)
	for line in f:
		line = line.rstrip('\r\n')
		if 0 == line.find('NEWFILE',0):
			filepath = line[8:]
			PyOpenDwgFile([filepath])
			doc = acad.ActiveDocument
			doc.SendCommand(command)
		elif 'SAVE DESIGN' == line:
			doc.Save()
		else:
			# run user command
			doc.SendCommand(line + ' ')
	f.close()

# brief		open given dwg file
# author	humkyung
# param		argv[0] = dwg file path
# date		2014.06.26
def PyOpenDwgFile(argv):
	try:
		if (type(argv) == type([])) and (1 == len(argv)):
			acad = win32com.client.Dispatch('AutoCAD.Application')
			acad.Visible = True
			docs = acad.Documents
			bFound = False
			for i in range(docs.Count):
				doc = docs.Item(i)
				if argv[0] == (doc.Path + '\\' + doc.Name):
					bFound = True
					break

			if False == bFound:
				docs.Open(unicode(argv[0] , 'euc_kr'), False)
				doc = docs.Item(0)
				doc.SendCommand('-units 2 4 1 2 0 N ')
				doc.SendCommand('-vports si ')
				acad.ZoomExtents()	# zoom extents

			acad.WindowState = 3	# maximize acad window.
			return 'SUCCESS'
	except:
		print str(traceback.format_exc())

	return 'FAIL'

# brief		get a point
# author	humkyung
# param		
# date		2014.06.24
def PyGetPoint(argv):
	try:
		acad = win32com.client.Dispatch('AutoCAD.Application')
		acad.Visible = True
		shell = win32com.client.Dispatch("WScript.Shell")
		shell.AppActivate(acad.Caption)
		doc = acad.ActiveDocument
		util = doc.Utility
		
		# Nearest & Endpoint
		doc.SetVariable('osmode', 513)

		pt = util.GetPoint()

		res = []
		res.append(pt[0])
		res.append(pt[1])
		return res
	except:
		print str(traceback.format_exc())

if __name__ == "__main__":
    CallMSTNJ('C:\\Users\\h2012202.DAELIM\\AppData\\Local\\Temp\\run.cmd')
    #PyWriteToAutoCAD('D:\\Project\\SmartDraw\\SmartDraw_PDS\\Data\\AutoCAD\\Working\\pds.cmd')
    #vadfilepath = 'D:\\Project\\SmartDraw\\SmartDraw_PDS\\Data\\DEMO_A\Dra\\6000-004.Vad'
    #drafilepath = 'D:\\Project\\SmartDraw\\SmartDraw_PDS\\Data\\DEMO_A\Dra\\6000-004.dra'
    #vad2dra(drafilepath , vadfilepath)
