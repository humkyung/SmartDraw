using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.ApplicationServices;

namespace aDwgWriter
{
    class GroupElm : WriteElm
    {
        private List<WriteElm> mElmList = new List<WriteElm>();

        public override bool Parse(string aLine, StreamReader oSR)
        {
            string sLine = oSR.ReadLine();
            while (sLine.IndexOf("</GROUP>") < 0)
            {
                           
                WriteElm oElm = ElmFactory.CreateElm(sLine);
                if(null != oElm)
                {
                   oElm.Parse(sLine, oSR);
                   mElmList.Add(oElm);
                }

                sLine = oSR.ReadLine();  
            }
           
            return true;
        }

		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
        {
            List<ObjectId> lstIds = new List<ObjectId>();
            List<ObjectId> retIds = new List<ObjectId>();           
            
            Document doc = Application.DocumentManager.MdiActiveDocument;   
            foreach (WriteElm oElm in mElmList)
            {               
				LayerTable lt = (LayerTable)myT.GetObject(bt.Database.LayerTableId, OpenMode.ForRead);
                if (!lt.Has(oElm.Layer) && oElm.Layer != null)
                {
                    LayerTableRecord ltr = new LayerTableRecord();

                    /// ... and set its properties
                    ltr.Name = oElm.Layer;
                    //ltr.Color = Color.FromColorIndex(ColorMethod.ByAci, _colorIndex);

                    // Add the new layer to the layer table
                    lt.UpgradeOpen();
                    ObjectId ltId = lt.Add(ltr);
                    myT.AddNewlyCreatedDBObject(ltr, true);

                    // Set the layer to be current for this drawing
					bt.Database.Clayer = ltId;
                }

				List<ObjectId> oObjIds = oElm.CreateEntity( bt , btr, myT);
                lstIds.AddRange(oObjIds);
            }


            //Create Group
            DBDictionary mGroupDictionary = new DBDictionary();
            string sGroupName = string.Empty;
            Group oGroup = new Group();
            ObjectIdCollection ids = new ObjectIdCollection();

            foreach (ObjectId id in lstIds)
            {
                ids.Add(id);
            }

			mGroupDictionary = (DBDictionary)myT.GetObject(bt.Database.GroupDictionaryId, OpenMode.ForWrite);
            sGroupName = lstIds[0].ToString();
            oGroup = new Group(sGroupName, true);
            mGroupDictionary.UpgradeOpen();
            mGroupDictionary.SetAt(sGroupName, oGroup);
            myT.AddNewlyCreatedDBObject(oGroup, true);
            oGroup.InsertAt(0, ids);            
            return retIds;

        }
    }
}
