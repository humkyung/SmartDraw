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
    class TextElm : WriteElm
    {
        private Point3d mptLoc;
        private double mHeight, mWidth;
        private double mAngle , mOblique;
        private string mText;
        private string mJustify , mTextStyle;

		/// <summary>
		/// parse a line for text element
		/// </summary>
		/// <param name="aLine"></param>
		/// <param name="oSR"></param>
		/// <returns></returns>
        public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (3 == tokens.Length)
			{
				CsvParser.CsvParser parser = new CsvParser.CsvParser();
				using (StringReader reader = new StringReader(tokens[1]))
				{
					string[][] subtokens = parser.Parse(reader);
					if ((1 == subtokens.Length) && (9 == subtokens[0].Length))
					{
						mptLoc = new Point3d(Convert.ToDouble(subtokens[0][0]), Convert.ToDouble(subtokens[0][1]), Convert.ToDouble(subtokens[0][2]));
						mHeight = Convert.ToDouble(subtokens[0][3]);
						mWidth = Convert.ToDouble(subtokens[0][4]);
						mAngle = DEG2RAD(Convert.ToDouble(subtokens[0][5]));
						mText = subtokens[0][6];
						mJustify = subtokens[0][7];
						mOblique = Convert.ToDouble(subtokens[0][8]);
					}
					else
					{
						return false;
					}
				}

				{
					string[] subtokens = tokens[2].Split(",".ToCharArray());
					if (4 == subtokens.Length)
					{
						mLayer = subtokens[0];
						mColor = subtokens[1];
						mTextStyle = subtokens[2];
						mLweight = subtokens[3];
					}
					else
					{
						return false;
					}
				}

				return true;
			}

            return false;
        }


		/// <summary>
		/// create a text instance
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.12</date>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
        {
            Document doc = Application.DocumentManager.MdiActiveDocument;
            Editor ed = doc.Editor;
            List<ObjectId> retIds = new List<ObjectId>();

			try
			{
				DBText acTxt = new DBText();
				{
					acTxt.TextString = mText;
					acTxt.Position = mptLoc;
					acTxt.Height = mHeight;
					acTxt.Rotation = mAngle;
					acTxt.Oblique = mOblique;
					acTxt.Normal = ed.CurrentUserCoordinateSystem.CoordinateSystem3d.Zaxis;
					acTxt.Layer = this.Layer;
					acTxt.Color = WriteElm.ParseColor(mColor);
					acTxt.LineWeight = WriteElm.ParseLineWeight(mLweight);
					acTxt.Justify = this.ParseJustify(mJustify);
					if (acTxt.Justify == AttachmentPoint.TopCenter)
					{
						acTxt.VerticalMode = TextVerticalMode.TextTop;
						acTxt.HorizontalMode = TextHorizontalMode.TextCenter;
					}
					if (acTxt.Justify != AttachmentPoint.BaseLeft)
					{
						acTxt.AlignmentPoint = mptLoc;
					}

					using (TextStyleTable tblStyle = (TextStyleTable)bt.Database.TextStyleTableId.GetObject(OpenMode.ForWrite))
					{
						if (tblStyle.Has(mTextStyle))
						{
							ObjectId oIdStyle = tblStyle[mTextStyle];
							acTxt.TextStyleId = oIdStyle;
							acTxt.WidthFactor = TextStyleElm.dicTextStyle.ContainsKey(mTextStyle) ? TextStyleElm.dicTextStyle[mTextStyle].WidthFactor : 1;	/// 2012.12.12 added by humkyung
						}
						else
						{
							acTxt.TextStyleId = tblStyle["Standard"];
						}
					}

					retIds.Add(btr.AppendEntity(acTxt));
					myT.AddNewlyCreatedDBObject(acTxt, true);
				}
			}
			catch (Autodesk.AutoCAD.Runtime.Exception ex)
			{
				ed.WriteMessage("\nTextElm Exception: {0}", ex.StackTrace);
			}

            return retIds;
        }

        /// <summary>
        /// get justify from string
        /// </summary>
        /// <author>humkyung</author>
        /// <date>2012.10.23</date>
        /// <param name="sJustify"></param>
        /// <returns></returns>
        private AttachmentPoint ParseJustify(string sJustify)
        {
            AttachmentPoint res = AttachmentPoint.BottomLeft;

            switch (sJustify)
            {
                case "Left Top":
                    res = AttachmentPoint.TopLeft;
                    break;
                case "Left Center":
                    res = AttachmentPoint.MiddleLeft;
                    break;
                case "Left Bottom":
                    res = AttachmentPoint.BaseLeft;
                    break;
                case "Center Top":
                    res = AttachmentPoint.TopMid;
                    break;
                case "Center Center":
                    res = AttachmentPoint.MiddleMid;
                    break;
                case "Center Bottom":
                    res = AttachmentPoint.BottomMid;
                    break;
                case "Right Top":
                    res = AttachmentPoint.TopRight;
                    break;
                case "Right Center":
                    res = AttachmentPoint.MiddleRight;
                    break;
                case "Right Bottom":
                    res = AttachmentPoint.BaseRight;
                    break;
            }

            return res;
        }
    }
}
