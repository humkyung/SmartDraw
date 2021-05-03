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
    class LeaderElm : WriteElm
    {
        private List<Point2d> mPointList = new List<Point2d>();
        private string mArrowType = "CLOSE";
        private double mArrowWidth = 0 , mArrowHeight = 0;

        /// <summary>
        /// parse leader element
        /// </summary>
        /// <param name="aLine"></param>
        /// <param name="oSR"></param>
        /// <returns></returns>
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
				if ((5 == subtokens.Length) || (6 == subtokens.Length))
                {
                    mLayer = subtokens[0];
                    mColor = subtokens[1];
                    mArrowType = subtokens[2];
                    mArrowWidth = Convert.ToDouble(subtokens[3]);
                    mArrowHeight = Convert.ToDouble(subtokens[4]);
					mLweight = (6 == subtokens.Length) ? subtokens[5] : "0";
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
            List<ObjectId> retids = new List<ObjectId>();

            Polyline oPolyline = new Polyline(mPointList.Count);
            {
                foreach (Point2d pt in mPointList)
                {
                    oPolyline.AddVertexAt(oPolyline.NumberOfVertices, pt, 0, 0, 0);
                }
                oPolyline.Elevation = 0;
                oPolyline.Closed = false;
                oPolyline.Layer = this.Layer;
                oPolyline.Color = WriteElm.ParseColor(mColor);
                if(!string.IsNullOrEmpty(mLtype)) oPolyline.Linetype = mLtype;
				oPolyline.LinetypeScale = ("Default" != mLtscale) ? Convert.ToDouble(mLtscale) : 1.0;
				oPolyline.LineWeight = WriteElm.ParseLineWeight(mLweight);

                retids.Add(btr.AppendEntity(oPolyline));
                myT.AddNewlyCreatedDBObject(oPolyline, true);
                //res.Add(oPolyline);
            }

            Point2d[] oCorner = new Point2d[3];
            oCorner[0] = mPointList[0];
            Vector2d aVec = new Vector2d(mPointList[0].X - mPointList[1].X, mPointList[0].Y - mPointList[1].Y);
            aVec = aVec.GetNormal();
            Vector2d aPer = aVec.GetPerpendicularVector();
            Point2d aPos = oCorner[0] - aVec * mArrowWidth;
            oCorner[1] = aPos + aPer * mArrowHeight * 0.5;
            oCorner[2] = aPos - aPer * mArrowHeight * 0.5;
            Polyline oArrow = new Polyline(oCorner.Length);
            {
                foreach (Point2d pt in oCorner)
                {
                    oArrow.AddVertexAt(oArrow.NumberOfVertices, pt, 0, 0, 0);
                }
                oArrow.Elevation = 0;
                oArrow.Closed = true;
                oArrow.Layer = this.Layer;
                oArrow.Color = WriteElm.ParseColor(mColor);
				try
				{
					oArrow.Linetype = WriteElm.ParseLineType(mLtype);
				}
				catch
				{
					oArrow.Linetype = "ByLayer";
				}
				oArrow.LinetypeScale = ("Default" != mLtscale) ? Convert.ToDouble(mLtscale) : 1.0;
				oArrow.LineWeight = WriteElm.ParseLineWeight(mLweight);

                ObjectIdCollection ids = new ObjectIdCollection();
                ids.Add(btr.AppendEntity(oArrow));
                retids.Add(ids[0]);

                myT.AddNewlyCreatedDBObject(oArrow, true);
                //res.Add(oArrow);

				try
				{
					Hatch oHatch = new Hatch();
					oHatch.Normal = new Vector3d(0.0, 0.0, 1.0);
					oHatch.SetDatabaseDefaults();
					oHatch.SetHatchPattern(HatchPatternType.PreDefined, "SOLID");
					oHatch.Color = oArrow.Color;
					oHatch.AppendLoop(HatchLoopTypes.Default, ids);
					oHatch.EvaluateHatch(true);

					retids.Add(btr.AppendEntity(oHatch));
					myT.AddNewlyCreatedDBObject(oHatch, true);
				}
				catch(Autodesk.AutoCAD.Runtime.Exception)
				{
				}

                //Hatch oHatch = new Hatch();
                //oHatch.Normal = new Vector3d(0.0, 0.0, 1.0);
                //oHatch.SetDatabaseDefaults();
                //oHatch.SetHatchPattern(HatchPatternType.PreDefined, "SOLID");
                //oHatch.Color = oArrow.Color;

                //HatchLoop oHatchLoop = new HatchLoop(HatchLoopTypes.Polyline);
                //Autodesk.AutoCAD.Geometry.PolylineCurve2d oPolylineCurve2d = new PolylineCurve2d(new Point2dCollection(oCorner));
                //oHatchLoop.Curves.Add(oPolylineCurve2d);
                
                //oHatch.AppendLoop(oHatchLoop);
                ////    HatchLoop oHatchLoop = new HatchLoop(HatchLoopTypes.Default);
                ////    oHatch.AppendLoop(oHatchLoop);
                ////}
                //oHatch.EvaluateHatch(true);

                //res.Add(oHatch);
            }

         
            
            return retids;
        }
    }
}
