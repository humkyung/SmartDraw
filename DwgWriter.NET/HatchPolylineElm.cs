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
    class HatchPolylineElm : WriteElm
    {
        private List<Point2d> mPointList = new List<Point2d>();
        private string mPattern = "SOLID", mAngle ="45",mDeleteSource = "No";
        private double mScale = 0;


        public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (3 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (0 == (subtokens.Length % 2))
                {
                    for (int i = 0; i < subtokens.Length; )
                    {
                        mPointList.Add(new Point2d(Convert.ToDouble(subtokens[i]), Convert.ToDouble(subtokens[i + 1])));
                        i += 2;
                    }
                }
                else
                {
                    return false;
                }

                subtokens = tokens[2].Split(",".ToCharArray());
                if (7 == subtokens.Length)
                {
                    mLayer = subtokens[0];
                    mColor = subtokens[1];
                    mPattern = subtokens[3];
					mAngle = subtokens[4];
					if (("NULL" == mPattern) && ("45" == mAngle))
					{
						mPattern = "ANSI31";
					}
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

            Polyline oPolyline = new Polyline(mPointList.Count);
            {
                foreach (Point2d pt in mPointList)
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

            if ("NULL" != mPattern)
            {
				try
				{
					Hatch oHatch = new Hatch();
					oHatch.Normal = new Vector3d(0.0, 0.0, 1.0);
					oHatch.SetDatabaseDefaults();
					oHatch.PatternScale = mScale*0.1;
					oHatch.SetHatchPattern(HatchPatternType.PreDefined, mPattern);
					oHatch.Color = WriteElm.ParseColor(mColor);
					oHatch.AppendLoop(HatchLoopTypes.Default, ids);
					oHatch.EvaluateHatch(true);
					retIds.Add(btr.AppendEntity(oHatch));
					myT.AddNewlyCreatedDBObject(oHatch, true);
				}
				catch (Autodesk.AutoCAD.Runtime.Exception)
				{
				}
            }                       
         
            return retIds;
        }
    }
}
