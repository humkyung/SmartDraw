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
    class HatchCircleElm : WriteElm
    {
        private Point3d m_ptCenter;
        private double m_dRadius;
        private string mPattern = "SOLID", mAngle="45",mDeleteSource = "No";
        private double mScale = 0;

		/// <summary>
		/// parse hatch circle
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
                if (3 == subtokens.Length)
                {
                    m_ptCenter = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), 0);
                    m_dRadius = Convert.ToDouble(subtokens[2]);
                }
				else if (4 == subtokens.Length)
				{
					m_ptCenter = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), Convert.ToDouble(subtokens[2]));
					m_dRadius = Convert.ToDouble(subtokens[3]);
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

		/// <summary>
		/// create a hatch circle
		/// </summary>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
        {
            List<Entity> res = new List<Entity>();
            
            ObjectIdCollection ids = new ObjectIdCollection();
            List<ObjectId> retIds = new List<ObjectId>();

            Circle oCircle = new Circle(m_ptCenter , new Vector3d(0.0,0.0,1.0) , m_dRadius);
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
            ids.Add( btr.AppendEntity(oCircle));
            retIds.Add(ids[0]);
            myT.AddNewlyCreatedDBObject(oCircle, true);

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
            
       