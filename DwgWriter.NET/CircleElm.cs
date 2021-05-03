using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.ApplicationServices;
using Autodesk.AutoCAD.EditorInput;

namespace aDwgWriter
{
    class CircleElm : WriteElm
    {
        private Point3d m_ptCenter;
        private double m_dRadius;
        public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (3 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (4 == subtokens.Length)
                {
                    m_ptCenter = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), Convert.ToDouble(subtokens[2]));
                    m_dRadius = Convert.ToDouble(subtokens[3]);
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

		/// <summary>
		/// create a circle element
		/// </summary>
		/// <author>humkyung</author>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
        {
            List<Entity> res = new List<Entity>();
            List<ObjectId> retIds = new List<ObjectId>();

			try
			{
				Circle oCircle = new Circle(m_ptCenter, new Vector3d(0.0, 0.0, 1.0), m_dRadius);
				{
					oCircle.Layer = this.Layer;
					oCircle.Color = WriteElm.ParseColor(mColor);
					try
					{
						oCircle.Linetype = WriteElm.ParseLineType(mLtype);
					}
					catch
					{
						oCircle.Linetype = "ByLayer";
					}
					oCircle.LinetypeScale = ("Default" != mLtscale) ? Convert.ToDouble(mLtscale) : 1.0;
					oCircle.LineWeight = WriteElm.ParseLineWeight(mLweight);
				}
				retIds.Add(btr.AppendEntity(oCircle));
				myT.AddNewlyCreatedDBObject(oCircle, true);
			}
			catch (Autodesk.AutoCAD.Runtime.Exception ex)
			{
				Document doc = Application.DocumentManager.MdiActiveDocument;
				Autodesk.AutoCAD.EditorInput.Editor ed = doc.Editor;

				ed.WriteMessage("\nCircleElm Exception: {0}", ex.Message);
			}

            return retIds;
        }
    }
}
