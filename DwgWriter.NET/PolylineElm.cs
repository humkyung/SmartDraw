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
    class PolylineElm : WriteElm
    {
        private List<Point2d> mPointList = new List<Point2d>();
        public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (3 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (0 == (subtokens.Length%2))
                {
                    for (int i = 0; i < subtokens.Length; )
                    {
                        mPointList.Add(new Point2d(Convert.ToDouble(subtokens[i]), Convert.ToDouble(subtokens[i+1])));
                        i += 2;
                    }
                }
                else
                {
                    return false;
                }

                subtokens = tokens[2].Split(",".ToCharArray());
                if (5 == subtokens.Length)
                {
                    mLayer = subtokens[0];
                    mColor = subtokens[1];
                    mLtype = subtokens[2];
                    mLtscale = subtokens[3];
                    mLweight = subtokens[4];
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
            List<ObjectId> retIds = new List<ObjectId>();

			try
			{
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
					try
					{
						oPolyline.Linetype = WriteElm.ParseLineType(mLtype);
					}
					catch
					{
						oPolyline.Linetype = "ByLayer";
					}
					oPolyline.LinetypeScale = ("Default" != mLtscale) ? Convert.ToDouble(mLtscale) : 1.0;
					oPolyline.LineWeight = WriteElm.ParseLineWeight(mLweight);

					retIds.Add(btr.AppendEntity(oPolyline));
					myT.AddNewlyCreatedDBObject(oPolyline, true);
				}
			}
			catch (Autodesk.AutoCAD.Runtime.Exception ex)
			{
				Application.DocumentManager.MdiActiveDocument.Editor.WriteMessage("\nPolylineElm Exception: {0}", ex.Message);
			}

            return retIds;
        }
    }
}
