import os
import ConfigParser
import abc
import xml.etree.cElementTree as ElementTree
import dra
from xml.dom import minidom

class AbstractConverter:
    __metaclass__ = abc.ABCMeta
    @abc.abstractmethod
    def convert(self, view):
        pass

class DraConverter(AbstractConverter):
    def __init__(self , filepath):
        self.filepath = filepath
        self.root = ElementTree.Element('DRA')
        self.root.set('Setting' , 'Default')

    """
    @brief  save xml string to file
    @author humkyung
    @date   2014.05.04
    """
    def save(self):
        f = open(self.filepath , 'w')
        f.write(prettify(self.root))
        f.close()

    """
    @brief  convert given view to xml
    @brief  humkyung
    @date   2014.05.04
    """
    def convert(self,view):
        viewelm = ElementTree.SubElement(self.root,'VIEW')
        viewelm.set('Name', view.name)
        viewelm.set('Volume', view.From + ',' + view.To)
        viewelm.set('Direction' , view.Direction)
        tokens = view.Scale.split('/')
        viewelm.set('Scale' , tokens[len(tokens) - 1])
        viewelm.set('Rotate' , view.Angle)
        viewelm.set('ViewOrigin' , view.point_2d)
        viewelm.set('MatchLineUpData' , view.MatchLineData_Up)
        viewelm.set('MatchLineDownData' , view.MatchLineData_Down)
        viewelm.set('MatchLineEastData' , view.MatchLineData_East)
        viewelm.set('MatchLineWestData' , view.MatchLineData_West)
        viewelm.set('MatchLineNorthData' , view.MatchLineData_North)
        viewelm.set('MatchLineSouthData' , view.MatchLineData_South)
        viewelm.set('ViewInfo' , '')

        oFileNames = view.Referenced_List.split(',')
        oFileNames = filter(None , oFileNames)      #remove empty string from array
        oTypes = view.Section_List.split(',')
        oTypes = filter(None , oTypes)              #remove empty string from array
        for i in range(0,len(oFileNames)):
            ref = ElementTree.SubElement(viewelm , "REF")
            ref.set('Type' , oTypes[i])
            ref.set('Name', oFileNames[i])

class vad:
    class view:
        def __init__(self, view_name):
            self.name = view_name

        """
        @brief  parse view which is PlantWAVE file format
        @author humkyung
        @date   2014.05.04
        """
        def parse(self, config):
            self.Dgn_Center = config.get(self.name, 'Dgn_Center')
            self.Dgn_Scale = config.get(self.name, 'Dgn_Scale')
            self.Dgn_Angle = config.getfloat(self.name, 'Dgn_Angle')
            self.Direction = config.get(self.name, 'Direction')
            self.From = config.get(self.name, 'From')
            self.To = config.get(self.name, 'To')
            self.point_2d = config.get(self.name, '2D_Point')
            self.Scale = config.get(self.name, 'Scale')
            self.Angle = config.get(self.name, 'Angle')
            self.Referenced_List = config.get(self.name, 'Referenced_Lists')
            self.Section_List = config.get(self.name, 'Section_Lists')
            self.MatchLineData_Up = config.get(self.name, 'MatchLineData(Up)')
            self.MatchLineData_Down = config.get(self.name, 'MatchLineData(Down)')
            self.MatchLineData_East = config.get(self.name, 'MatchLineData(East)')
            self.MatchLineData_West = config.get(self.name, 'MatchLineData(West)')
            self.MatchLineData_South = config.get(self.name, 'MatchLineData(South)')
            self.MatchLineData_North = config.get(self.name, 'MatchLineData(North)')

            self.Referenced_List = config.get(self.name, 'Referenced_Lists')
            self.Section_List = config.get(self.name, 'Section_Lists')

    def __init__(self):
        pass

        """
        @brief  parse vad file which is PlantWAVE file format
        @author humkyung
        @date   2014.05.04
        """
    def parse(self, filepath):
        config = ConfigParser.RawConfigParser()
        config.read(filepath)
        self.dwg_uor = config.get('GENERNAL','DRW_UOR')
        self.dwg_no = config .get('GENERNAL' , 'DWG_NO')

        view_names = config.get('VIEW_LIST' , 'VIEW_NAMES')
        all_referenced_list = config.get('VIEW_LIST' , 'ALL_REFERENCED_LISTS')
        all_section_list = config.get('VIEW_LIST' , 'ALL_SECTION_LISTS')

        view_name_array = view_names.split(',')
        view_name_array = filter(None , view_name_array)    #remove empty string from array
        self.view_obj_list = []
        for view_name in view_name_array:
            view_ = vad.view(view_name)
            view_.parse(config)
            self.view_obj_list.append(view_)

	"""
	@brief  convert vad file to dra file
	@author humkyung
	@date   2014.05.04
	"""
    def convert(self, converter):
        for view_obj in self.view_obj_list:
            converter.convert(view_obj)
