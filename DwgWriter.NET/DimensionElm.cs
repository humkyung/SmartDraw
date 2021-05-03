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
    class DimensionElm : WriteElm
    {
        private Point3d m_ptFirst, m_ptSecond, m_ptLoc;
        private double m_dDimScale;
        private string m_sDimDirec, m_sTextDirec;
        private string m_sDimStyle;

		/// <summary>
		/// parse dimension element
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
                if (7 == subtokens.Length)
                {
                    m_ptFirst = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), 0.0);
                    m_ptSecond = new Point3d(Convert.ToDouble(subtokens[2]), Convert.ToDouble(subtokens[3]), 0.0);
                    m_ptLoc = new Point3d(Convert.ToDouble(subtokens[4]), Convert.ToDouble(subtokens[5]), 0.0);
                    m_dDimScale = Convert.ToDouble(subtokens[6]);
                }
                else
                {
                    return false;
                }

                subtokens = tokens[2].Split(",".ToCharArray());
                if (4 == subtokens.Length)
                {
                    mLayer = subtokens[0];
					if (string.IsNullOrEmpty(mLayer)) mLayer = "0";
                    m_sDimDirec = subtokens[1];
                    m_sTextDirec = subtokens[2];
                    m_sDimStyle = subtokens[3];
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
		/// create a dimension
		/// </summary>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
        {
            List<ObjectId> retIds = new List<ObjectId>();
            ObjectId objid = ObjectId.Null;

			/// get dimension style
			ObjectId oDimStyleObjId = ObjectId.Null;
			DimStyleTable acDimStyleTbl = (DimStyleTable)myT.GetObject(bt.Database.DimStyleTableId, OpenMode.ForRead);
			if (true == acDimStyleTbl.Has(m_sDimStyle))
			{
				oDimStyleObjId = acDimStyleTbl[m_sDimStyle];
			}
			/// up to here

			try
			{
				AlignedDimension acDim = new AlignedDimension(m_ptFirst, m_ptSecond, m_ptLoc, string.Empty, oDimStyleObjId);
				{
					if ((ObjectId.Null == oDimStyleObjId) && (true == DimensionStyleElm.dicDimStyle.ContainsKey(m_sDimStyle)))
					{
						DimensionStyleElm oDimStyle = DimensionStyleElm.dicDimStyle[m_sDimStyle];
						//oDimension.Text = null;
						acDim.Dimtxt = oDimStyle.m_dTextHeight;
					}

					acDim.Dimsoxd = true;	/// suppress arrowhead when space is not enough
					acDim.Dimlfac = m_dDimScale;
					acDim.Dimtix = true;
					Application.DocumentManager.MdiActiveDocument.Editor.WriteMessage("\nDimensionElm Direction: {0}", acDim.Dimatfit);
					if (m_sTextDirec == "D")
						acDim.Dimtad = 4;	/// place text below dimension line
					else if ("DR" == m_sTextDirec)
					{
						acDim.TextRotation = Math.PI*(double)90/(double)180;
						acDim.Dimtad = 4;
					}
					//oDimension.Dimdec = 0;
					acDim.Layer = mLayer;
				}
				retIds.Add(btr.AppendEntity(acDim));
				myT.AddNewlyCreatedDBObject(acDim, true);
			}
			catch (Autodesk.AutoCAD.Runtime.Exception ex)
			{
				Document doc = Application.DocumentManager.MdiActiveDocument;
				Autodesk.AutoCAD.EditorInput.Editor ed = doc.Editor;

				ed.WriteMessage("\nDimensionElm Exception: {0}", ex.Message);
			}

            return retIds;
        }
    }
}
