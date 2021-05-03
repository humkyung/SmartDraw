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
    class LineElm : WriteElm
    {
        private Point3d mptStart, mptEnd;

        public override bool Parse(string aLine, StreamReader oSR)
        { 
            string[] tokens = aLine.Split("|".ToCharArray());
            if(3 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (6 == subtokens.Length)
                {
                    mptStart = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), Convert.ToDouble(subtokens[2]));
                    mptEnd = new Point3d(Convert.ToDouble(subtokens[3]), Convert.ToDouble(subtokens[4]), Convert.ToDouble(subtokens[5]));
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
            List<ObjectId> res = new List<ObjectId>();

			try
			{
				Line oLine = new Line(mptStart, mptEnd);
				{
					oLine.Color = WriteElm.ParseColor(mColor);
					try
					{
						oLine.Linetype = WriteElm.ParseLineType(mLtype);
					}
					catch
					{
						oLine.Linetype = "ByLayer";
					}
					oLine.LinetypeScale = this.ParseLineScale(mLtscale);
					oLine.LineWeight = WriteElm.ParseLineWeight(mLweight);

					res.Add(btr.AppendEntity(oLine));
					myT.AddNewlyCreatedDBObject(oLine, true);
					oLine.Layer = this.Layer;
				}
			}
			catch (Autodesk.AutoCAD.Runtime.Exception ex)
			{
				Application.DocumentManager.MdiActiveDocument.Editor.WriteMessage("\nLineElm Exception: {0}", ex.Message);
			}

            return res;
        }
    }
}
