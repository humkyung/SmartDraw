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
	class TextStyleElm : WriteElm
	{
		public static Dictionary<string, TextStyleElm> dicTextStyle = new Dictionary<string, TextStyleElm>();

		string mStyleName = string.Empty , mFontName = string.Empty;
		double mTextHeight = 2.5, mWidthFactor = 1, mObliquingAngle = 0;
		bool mVertical = false;

		public double TextHeight
		{
			get { return mTextHeight; }
		}
		public double WidthFactor
		{
			get { return mWidthFactor; }
		}

		/// <summary>
		/// parse text style property
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.06</date>
		/// <param name="aLine"></param>
		/// <param name="oSR"></param>
		/// <returns></returns>
		public override bool Parse(string aLine, StreamReader oSR)
		{
			string[] tokens = aLine.Split("|".ToCharArray());
			if (2 == tokens.Length)
			{
				string[] subtokens = tokens[1].Split(",".ToCharArray());
				if (6 == subtokens.Length)
				{
					mStyleName = subtokens[0];
					mFontName = subtokens[1];
					mTextHeight = (string.Empty != subtokens[2]) ? Convert.ToDouble(subtokens[2]) : 2.5;
					mWidthFactor = (string.Empty != subtokens[3]) ? Convert.ToDouble(subtokens[3]) : 1;
					mObliquingAngle = (string.Empty != subtokens[4]) ? Convert.ToDouble(subtokens[4]) : 0;
					mVertical = ("True" == subtokens[5]);

					if (false == TextStyleElm.dicTextStyle.ContainsKey(mStyleName))	/// 2012.12.12 added by humkyung
					{
						TextStyleElm.dicTextStyle.Add(mStyleName, this);
					}
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
		/// create a textstyle
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.06</date>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
		{
			List<ObjectId> retIds = new List<ObjectId>();

			/// create a text style record if not exists
			ObjectId acLineTypeId = ObjectId.Null;
			TextStyleTable acTextStyleTbl = myT.GetObject(bt.Database.TextStyleTableId, OpenMode.ForRead) as TextStyleTable;
			if (!string.IsNullOrEmpty(mStyleName) && !acTextStyleTbl.Has(mStyleName))
			{
				acTextStyleTbl.UpgradeOpen();
				TextStyleTableRecord acTextStyleTblRcd = new TextStyleTableRecord();
				{
					acTextStyleTblRcd.Name = mStyleName;
					acTextStyleTblRcd.FileName = mFontName;
					acTextStyleTblRcd.TextSize = mTextHeight;
					acTextStyleTblRcd.XScale = mWidthFactor;
					acTextStyleTblRcd.ObliquingAngle = mObliquingAngle;
					acTextStyleTblRcd.IsVertical = mVertical;
					acTextStyleTblRcd.IsShapeFile = true;
				}
				retIds.Add(acTextStyleTbl.Add(acTextStyleTblRcd));
				myT.AddNewlyCreatedDBObject(acTextStyleTblRcd, true);
			}
			else if (acTextStyleTbl.Has(mStyleName))
			{
				SymbolTableEnumerator oEnumerator = acTextStyleTbl.GetEnumerator();
				while (oEnumerator.MoveNext())
				{
					TextStyleTableRecord acTextStyleTblRcd = oEnumerator.Current.GetObject(OpenMode.ForWrite) as TextStyleTableRecord;
					if (mStyleName == acTextStyleTblRcd.Name)
					{
						acTextStyleTblRcd.FileName = mFontName;
						acTextStyleTblRcd.TextSize = mTextHeight;
						acTextStyleTblRcd.XScale = mWidthFactor;
						acTextStyleTblRcd.ObliquingAngle = mObliquingAngle;
						acTextStyleTblRcd.IsVertical = mVertical;
						retIds.Add(acTextStyleTblRcd.Id);
						break;
					}
				}
			}
			/// up to here

			return retIds;
		}
	}
}
