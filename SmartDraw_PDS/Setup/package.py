from ftplib import FTP
import os
import sys
from subprocess import *	# for call function.
import traceback
import shutil

# write version number to VersionNo.nsi file.
#f = file('.\\VersionNo.nsi' , 'w')
#print version
#f.write('!define PRODUCT_VERSION "' + version + '"')
#f.close()

# create version folder and then copy pdb files.
#versionDir = '.\\' + version.replace(',' , '.')
#if False == os.path.exists('Setup\\' + versionDir):
#	os.mkdir('Setup\\' + versionDir)

#shutil.copyfile('.\\Admin.pdb'   , 'Setup\\' + versionDir + '\\Admin.pdb')
#shutil.copyfile('.\\Module2008.pdb'  , 'Setup\\' + versionDir + '\\Module.pdb')
#shutil.copyfile('.\\ogl2dd.pdb'  , 'Setup\\' + versionDir + '\\ogl2dd.pdb')
#shutil.copyfile('.\\UserApp.pdb' , 'Setup\\' + versionDir + '\\UserApp.pdb')
#shutil.copyfile('.\\UserDLL.pdb' , 'Setup\\' + versionDir + '\\UserDLL.pdb')
# up to here

# compile nsi file.
call('"C:\\Program Files\\NSIS\\makensis.exe" "SmartDraw.nsi"')

# find version from VersionNo.nsi
f = file('VersionNo.nsi')
for line in f:
	at = line.find('PRODUCT_VERSION')
	if -1 != at:
		version = line[at + 16:]
		version = version.lstrip('"')
		version = version.rstrip('"')
		break
f.close()

# zip setup file
#param = '\"Setup\\SmartDraw for PDS.' + version + '.zip\" ' + '\"Setup\\SmartDraw for PDS.' + version + '.exe\"'
#param
#call('"C:\\Program Files\\7-Zip\\7z.exe" a -tzip ' + param)

appFileName = 'SmartDraw_PDS.' + version + '.exe'
#zipFileName = 'SmartDraw for PDS.' + version + '.zip'
# move files(Setup and zip files)
#SetupFileName = 'Setup\\' + appFileName
#ZipFilePath   = 'Setup\\' + zipFileName
#shutil.move( SetupFileName , 'Setup\\' + versionDir + '\\' + appFileName)
#shutil.move( ZipFilePath , 'Setup\\' + versionDir + '\\' + zipFileName)

# create a tag with version - 2011.08.03
#call ('svn copy file:///Z:/REPO/SolutionWare/Projects/IDMS file:///Z:/REPO/SolutionWare/tags/IDMS/' + version + ' -m' + version)

# put compiled file(setup file) on ftp site.
ftp = FTP()
host = '192.168.35.194'
ftp.connect(host)
print ftp.getwelcome()

try:
	ftp.login('humkyung' , '^04080124$')

	# upload execution(binary) file.
	f = open('.\\Installation\\' + appFileName , 'rb')
	ftp.storbinary('STOR Products/SmartDraw/' + appFileName , f , 1024)
	f.close()

	# upload xml file.
#	f = open('.\\updateinfo.xml' , 'rb')
#	ftp.storlines('STOR updateinfo.xml',f)
#	f.close()

	ftp.quit()

#	call('"..\\..\\..\\Bin\\LMS.exe" "ftp://www.solutionware.co.kr/Projects/SmartDraw for PDS/users.xml" "http://www.solutionware.co.kr/Projects/SmartDraw for PDS/users.xml" "SmartDraw for PDS" ' + version)
except:
	ftp.quit()
	traceback.print_exc()
