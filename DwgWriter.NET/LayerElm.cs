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
	class LayerElm : WriteElm
	{
		/// <summary>
		/// parse layer property
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.05</date>
		/// <param name="aLine"></param>
		/// <param name="oSR"></param>
		/// <returns></returns>
		public override bool Parse(string aLine, StreamReader oSR)
		{
			string[] tokens = aLine.Split("|".ToCharArray());
			if (2 == tokens.Length)
			{
				string[] subtokens = tokens[1].Split(",".ToCharArray());
				if (4 == subtokens.Length)
				{
					mLayer = subtokens[0];
					mColor = subtokens[1];
					mLtype = subtokens[2];
					mLweight = subtokens[3];
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
		/// create a layer
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.05</date>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
		{
			/// create a line type record if not exists
			ObjectId acLineTypeId = ObjectId.Null;
			LinetypeTable acLineTypeTbl = myT.GetObject(bt.Database.LinetypeTableId, OpenMode.ForRead) as LinetypeTable;
			if ((string.Empty != mLtype) && !acLineTypeTbl.Has(mLtype))
			{
				LinetypeTableRecord ltr = new LinetypeTableRecord();
				ltr.Name = mLtype;

				acLineTypeTbl.UpgradeOpen();
				acLineTypeId = acLineTypeTbl.Add(ltr);
				myT.AddNewlyCreatedDBObject(ltr, true);
			}
			else if (acLineTypeTbl.Has(mLtype))
			{
				SymbolTableEnumerator oEnumerator = acLineTypeTbl.GetEnumerator();
				while (oEnumerator.MoveNext())
				{
					LinetypeTableRecord acLineTypeTblRcd = oEnumerator.Current.GetObject(OpenMode.ForRead) as LinetypeTableRecord;
					if (mLtype == acLineTypeTblRcd.Name)
					{
						acLineTypeId = acLineTypeTblRcd.Id;
						break;
					}
				}
			}
			/// up to here

			LayerTable lt = (LayerTable)myT.GetObject(bt.Database.LayerTableId, OpenMode.ForRead);
			if ((string.Empty != mLayer) && !lt.Has(mLayer))
			{
				LayerTableRecord ltr = new LayerTableRecord();

				/// ... and set its properties
				ltr.Name = mLayer;
				ltr.Color = WriteElm.ParseColor(mColor);
				if (ObjectId.Null != acLineTypeId)
				{
					ltr.LinetypeObjectId = acLineTypeId;
				}
				ltr.LineWeight = WriteElm.ParseLineWeight(mLweight);

				/// Add the new layer to the layer table
				lt.UpgradeOpen();
				ObjectId ltId = lt.Add(ltr);
				myT.AddNewlyCreatedDBObject(ltr, true);
			}

			return null;
		}
	}
}
