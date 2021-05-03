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
    class HatchDonutElm : WriteElm
    {       
        private Point3d m_ptCenter;
        private double m_dRadius1, m_dRadius2;

        private string mPattern = "SOLID", mDeleteSource = "No";
        private double mScale = 0;


        public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (3 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (5 == subtokens.Length)
                {
                    m_ptCenter = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), Convert.ToDouble(subtokens[2]));                  
                    m_dRadius1 = Convert.ToDouble(subtokens[3]);
                    m_dRadius2 = Convert.ToDouble(subtokens[4]);
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
            List<ObjectId> retIds = new List<ObjectId>();

            //Out Circle
            Circle oCircle = new Circle(m_ptCenter, new Vector3d(0.0, 0.0, 1.0), m_dRadius1);
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

            //In Circle
            oCircle = new Circle(m_ptCenter, new Vector3d(0.0, 0.0, 1.0), m_dRadius2);
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

            return retIds;
        }
    }
}
