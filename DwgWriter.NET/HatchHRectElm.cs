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
    class HatchHRecElm : WriteElm
    {
        private List<Point2d> mPointList1 = new List<Point2d>();
        private List<Point2d> mPointList2 = new List<Point2d>();
        private string mPattern = "SOLID", mDeleteSource = "No";
        private double mScale = 0;
        
        public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (3 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (0 == (subtokens.Length % 2))
                {                   
                    mPointList1.Add(new Point2d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1])));
                    mPointList1.Add(new Point2d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[3])));
                    mPointList1.Add(new Point2d(Convert.ToDouble(subtokens[2]), Convert.ToDouble(subtokens[3])));
                    mPointList1.Add(new Point2d(Convert.ToDouble(subtokens[2]), Convert.ToDouble(subtokens[1])));
                    
                    mPointList2.Add(new Point2d(Convert.ToDouble(subtokens[4]), Convert.ToDouble(subtokens[5])));
                    mPointList2.Add(new Point2d(Convert.ToDouble(subtokens[4]), Convert.ToDouble(subtokens[7])));
                    mPointList2.Add(new Point2d(Convert.ToDouble(subtokens[6]), Convert.ToDouble(subtokens[7])));
                    mPointList2.Add(new Point2d(Convert.ToDouble(subtokens[6]), Convert.ToDouble(subtokens[5])));
                }
                else
                {
                    return false;
                }

                subtokens = tokens[2].Split(",".ToCharArray());
                if (7 == subtokens.Length)
                {
                    mColor = subtokens[0];
                    mLayer = subtokens[1];
                    mPattern = subtokens[3];
                    mScale = Convert.ToDouble(subtokens[5]);
                    mDeleteSource = subtokens[6];
                }
                else
                {
                    return false;
                }

                return true;
            }

            return false;
        }


		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
        {
            List<Entity> res = new List<Entity>();
            ObjectIdCollection ids = new ObjectIdCollection();
            List<ObjectId> retIds = new List<ObjectId>();

            Polyline oPolyline = new Polyline(mPointList1.Count);
            {
                foreach (Point2d pt in mPointList1)
                {
                    oPolyline.AddVertexAt(oPolyline.NumberOfVertices, pt, 0, 0, 0);
                }
                oPolyline.Elevation = 0;
                oPolyline.Closed = true;
                oPolyline.Layer = this.Layer;
                oPolyline.Color = WriteElm.ParseColor(mColor);
				oPolyline.LinetypeScale = ("Default" != mLtscale) ? Convert.ToDouble(mLtscale) : 1.0;
				oPolyline.LineWeight = WriteElm.ParseLineWeight(mLweight);

                ids.Add( btr.AppendEntity(oPolyline));
                retIds.Add(ids[ids.Count - 1]);
                myT.AddNewlyCreatedDBObject(oPolyline, true);                
            }

            oPolyline = new Polyline(mPointList2.Count);
            {
                foreach (Point2d pt in mPointList2)
                {
                    oPolyline.AddVertexAt(oPolyline.NumberOfVertices, pt, 0, 0, 0);
                }
                oPolyline.Elevation = 0;
                oPolyline.Closed = true;
                oPolyline.Layer = this.Layer;
                oPolyline.Color = WriteElm.ParseColor(mColor);
				oPolyline.LinetypeScale = ("Default" != mLtscale) ? Convert.ToDouble(mLtscale) : 1.0;
				oPolyline.LineWeight = WriteElm.ParseLineWeight(mLweight);

                ids.Add(btr.AppendEntity(oPolyline));
                retIds.Add(ids[ids.Count - 1]);
                myT.AddNewlyCreatedDBObject(oPolyline, true);
            }
            return retIds;
        }
    }
}
