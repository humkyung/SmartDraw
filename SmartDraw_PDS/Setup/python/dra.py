import os
import ConfigParser
import abc
import glob
import math
import xml.etree.cElementTree as ElementTree
from xml.dom import minidom

# brief		convert string to float. return 0 when error happend
# author	humkyung
def SAFE_FLOAT(arg):
	try:
		res = float(arg)
		return res
	except ValueError:
		return 0
	except TypeError:
		return 0

	return 0

'''
def prettify(elem):
	reparsed = parseString(elem)
	return '\n'.join([line for line in reparsed.toprettyxml(indent=' '*2).split('\n') if line.strip()])
'''

def prettify(elem):
	rough_string = ElementTree.tostring(elem, encoding='UTF-8')
	reparsed = minidom.parseString(rough_string)
	list = reparsed.toprettyxml(indent=' '*2).split('\n')
	a = []
	for i in range(len(list)):
		list[i] = list[i].strip()
		if(list[i] != ''): a.append(list[i])
	prettified = '\n'.join(a)
		
	return prettified

def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    reverse = dict((value, key) for key, value in enums.iteritems())
    enums['reverse_mapping'] = reverse
    return type('Enum', (), enums)

class cPoint:
	def __init__(self):
		self.x = 0
		self.y = 0
		self.z = 0

	def __init__(self , x , y , z):
		self.x = x
		self.y = y
		self.z = z
		
class cView:
	def __init__(self , name):
		self.name = name
		self.min = cPoint(0,0,0)
		self.max = cPoint(0,0,0)
		self.matchlineindex = enum('NONE' , 'UP' , 'DOWN' , 'EAST' , 'WEST' , 'NORTH' , 'SOUTH')
		self.UP = 1
		self.DOWN = 2
		self.LEFT = 3
		self.RIGHT = 4
		self.FRONT = 5
		self.BACK = 6
		self.matchlinedata = {'UP':'' , 'DOWN':'' , 'EAST':'' , 'WEST':'' , 'NORTH':'' , 'SOUTH':''}

	def center(self):
		aCenter = cPoint((self.min.x + self.max.x)*0.5 , (self.min.y + self.max.y)*0.5 , (self.min.z + self.max.z)*0.5)
		return aCenter

	'''
	@brief	check two views are adjacent or not
	@author	humkyung
	@date	2014.11.20
	'''
	def isAdjacent(self , aView , toler):
		if(self.max.x + toler < aView.min.x): return self.matchlineindex.NONE
		if(self.max.y + toler < aView.min.y): return self.matchlineindex.NONE
		if(self.max.z + toler < aView.min.z): return self.matchlineindex.NONE
		if(self.min.x - toler > aView.max.x): return self.matchlineindex.NONE
		if(self.min.y - toler > aView.max.y): return self.matchlineindex.NONE
		if(self.min.z - toler > aView.max.z): return self.matchlineindex.NONE

		if(math.fabs(self.min.x - aView.max.x) < toler):
			return self.matchlineindex.WEST
		if(math.fabs(self.max.x - aView.min.x) < toler):
			return self.matchlineindex.EAST
		if(math.fabs(self.min.y - aView.max.y) < toler):
			return self.matchlineindex.SOUTH
		if(math.fabs(self.max.y - aView.min.y) < toler):
			return self.matchlineindex.NORTH
		if(math.fabs(self.min.z - aView.max.z) < toler):
			return self.matchlineindex.DOWN
		if(math.fabs(self.max.z - aView.min.z) < toler):
			return self.matchlineindex.UP
		
		return self.matchlineindex.NONE

class cDra:
	def __init__(self , filepath):
		self.filepath = filepath
		self.name = os.path.basename(filepath)
		self.name = self.name.split('.')[:-1][0]
		self.views = []
		self.nodes = []

	"""
	@brief  parse given dra file
	@brief  humkyung
	@date   2014.11.19
	"""
	def parse(self):
		tree = ElementTree.parse(self.filepath)
		self.root = tree.getroot()
		self.setting = self.root.attrib['Setting']

		for child in self.root.iter('VIEW'):
			aTokens = child.attrib['Volume'].split(',')
			if len(aTokens) == 6:
				aView = cView(child.attrib['Name'])
				aView.min = cPoint(SAFE_FLOAT(aTokens[0]) , SAFE_FLOAT(aTokens[1]) , SAFE_FLOAT(aTokens[2]))
				aView.max = cPoint(SAFE_FLOAT(aTokens[3]) , SAFE_FLOAT(aTokens[4]) , SAFE_FLOAT(aTokens[5]))
				aView.matchlinedata['UP'] = child.attrib['MatchLineUpData']
				aView.matchlinedata['DOWN'] = child.attrib['MatchLineDownData']
				aView.matchlinedata['EAST'] = child.attrib['MatchLineEastData']
				aView.matchlinedata['WEST'] = child.attrib['MatchLineWestData']
				aView.matchlinedata['NORTH'] = child.attrib['MatchLineNorthData']
				aView.matchlinedata['SOUTH'] = child.attrib['MatchLineSouthData']
				self.views.append(aView)
				self.nodes.append(child)

	'''
	@brief	save element to file
	@author	humkyung
	@date	2014.11.19
	'''
	def save(self):
		for i in range(len(self.views)):
			self.nodes[i].attrib['MatchLineUpData'] = self.views[i].matchlinedata['UP']
			self.nodes[i].attrib['MatchLineDownData'] = self.views[i].matchlinedata['DOWN']
			self.nodes[i].attrib['MatchLineEastData'] = self.views[i].matchlinedata['EAST']
			self.nodes[i].attrib['MatchLineWestData'] = self.views[i].matchlinedata['WEST']
			self.nodes[i].attrib['MatchLineNorthData'] = self.views[i].matchlinedata['NORTH']
			self.nodes[i].attrib['MatchLineSouthData'] = self.views[i].matchlinedata['SOUTH']

		# save to file
		f = open(self.filepath , 'w')
		str = prettify(self.root)
		f.write(str)
		f.close()
			
	def fillmatchlinedata(self , aDra):
		for lhs in self.views:
			for rhs in aDra.views:
				res = lhs.isAdjacent(rhs , 100)

				if lhs.matchlineindex.NONE != res:
					str = 'SEE DWG. ' + aDra.name

				if lhs.matchlineindex.UP == res and lhs.matchlinedata['UP'] == '':
					lhs.matchlinedata['UP'] = str

				if lhs.matchlineindex.DOWN == res and lhs.matchlinedata['DOWN'] == '':
					lhs.matchlinedata['DOWN'] = str

				if lhs.matchlineindex.EAST == res and lhs.matchlinedata['EAST'] == '':
					lhs.matchlinedata['EAST'] = str

				if lhs.matchlineindex.WEST == res and lhs.matchlinedata['WEST'] == '':
					lhs.matchlinedata['WEST'] = str

				if lhs.matchlineindex.NORTH == res and lhs.matchlinedata['NORTH'] == '':
					lhs.matchlinedata['NORTH'] = str

				if lhs.matchlineindex.SOUTH == res and lhs.matchlinedata['SOUTH'] == '':
					lhs.matchlinedata['SOUTH'] = str
	
if __name__ == "__main__":
	FillMatchLineData('F:\\Temp\\JIJEL\\Dra\\*.dra')
	#dra = cDra('F:\\Temp\\JIJEL\\Dra\\40901.dra')
	#dra.parse()
	print 'done'
