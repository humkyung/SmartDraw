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
    public class DimensionStyleElm : WriteElm
    {
        public static Dictionary<string, DimensionStyleElm> dicDimStyle = new Dictionary<string, DimensionStyleElm>();

        private string m_sName;
        public string Name
        {
			get { return m_sName; }
        }

		private string mTextStyle;
        public string TextStyle
        {
			get { return mTextStyle; }
        }

        private double _m_dTextWidth;
        public double m_dTextWidth
        {
            get { return _m_dTextWidth; }
        }

        private double _m_dTextHeight;
        public double m_dTextHeight
        {
            get { return _m_dTextHeight; }
        }

        private double _m_dTextWeight;
        public double m_dTextWeight
        {
            get { return _m_dTextWeight; }
        }

        private string _m_sTextLevel;
        public string m_sTextLevel
        {
            get { return _m_sTextLevel; }
        }

        private string m_sTextColor;
        public string TextColor
        {
            get { return m_sTextColor; }
        }
        
        private double _m_dTextOffset;
        public double m_dTextOffset
        {
            get { return _m_dTextOffset; }
        }

        private int m_iTextPrecision;
        public int TextPrecision
        {
            get { return m_iTextPrecision; }
        }

        private string m_sDimLineColor;
        public string DimLineColor
        {
            get { return m_sDimLineColor; }
        }

        private double _m_dDimLineDistance;
        public double m_dDimLineDistance
        {
            get { return _m_dDimLineDistance; }
        }

        private string m_sExtLineColor;
        public string ExtLineColor
        {
            get { return m_sExtLineColor; }
        }

        private double _m_dExtLineDistance;
        public double m_dExtLineDistance
        {
            get { return _m_dExtLineDistance; }
        }

        private double _m_dExtLineExt;
        public double m_dExtLineExt
        {
            get { return _m_dExtLineExt; }
        }

        private string _m_sArrowType;
        public string m_sArrowType
        {
            get { return _m_sArrowType; }
        }

        private double m_dArrowSizeWidth;
        public double ArrowSizeWidth
        {
            get { return m_dArrowSizeWidth; }
        }

        private double _m_dArrowSizeHeight;
        public double m_dArrowSizeHeight
        {
            get { return _m_dArrowSizeHeight; }
        }

		/// <summary>
		/// parse dimstyle propertis
		/// </summary>
		/// <author>humkyung</author>
		/// <param name="aLine"></param>
		/// <param name="oSR"></param>
		/// <returns></returns>
		public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (2 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (18 == subtokens.Length)
                {
                    m_sName = subtokens[0];
                    mTextStyle = subtokens[1];
					_m_dTextHeight = (string.Empty != subtokens[2]) ? Convert.ToDouble(subtokens[2]) : 0;
                    _m_dTextWidth = (string.Empty != subtokens[3]) ? Convert.ToDouble(subtokens[3]) : 0;
                    _m_dTextWeight = (string.Empty != subtokens[4]) ? Convert.ToDouble(subtokens[4]) : 0;

                    _m_sTextLevel = subtokens[5];
                    m_sTextColor = subtokens[6];
                    _m_dTextOffset = (string.Empty != subtokens[7]) ? Convert.ToDouble(subtokens[7]) : 0;
                    m_iTextPrecision = (!string.IsNullOrEmpty(subtokens[8]) ? Convert.ToInt32(subtokens[8]) : 0);

                    m_sDimLineColor = subtokens[9];
                    _m_dDimLineDistance = (string.Empty != subtokens[10]) ? Convert.ToDouble(subtokens[10]) : 0;

                    m_sExtLineColor = subtokens[11];
                    _m_dExtLineDistance = (string.Empty != subtokens[12]) ? Convert.ToDouble(subtokens[12]) : 0;
                    _m_dExtLineExt = Convert.ToDouble(subtokens[13]);

                    _m_sArrowType = subtokens[14];
                    m_dArrowSizeWidth = (string.Empty != subtokens[15]) ? Convert.ToDouble(subtokens[15]) : 0;
                    _m_dArrowSizeHeight = (string.Empty != subtokens[16]) ? Convert.ToDouble(subtokens[16]) : 0;
					string sAutoSizeDimText = subtokens[17];

					if (false == DimensionStyleElm.dicDimStyle.ContainsKey(m_sName))	/// 2012.12.12 added by humkyung
					{
						DimensionStyleElm.dicDimStyle.Add(m_sName, this);
					}
                }

                return true;
            }

            return false;
        }

		/// <summary>
		/// create a dimstyle
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

			DimStyleTable acDimStyleTbl = (DimStyleTable)myT.GetObject(bt.Database.DimStyleTableId, OpenMode.ForWrite);
			if (false == acDimStyleTbl.Has(m_sName))
			{
				TextStyleTable acTextStyleTbl = myT.GetObject(bt.Database.TextStyleTableId, OpenMode.ForRead) as TextStyleTable;
				if (acTextStyleTbl.Has(mTextStyle))
				{
					DimStyleTableRecord acDimStyleRcd = new DimStyleTableRecord();
					{
						acDimStyleRcd.Name = m_sName;
						acDimStyleRcd.Dimtxsty = acTextStyleTbl[mTextStyle];
						acDimStyleRcd.Dimtxt = TextStyleElm.dicTextStyle[mTextStyle].TextHeight;
						acDimStyleRcd.Dimdec = TextPrecision;	/// precision 0
						acDimStyleRcd.Dimtad = 1;				/// place text above dim line
						acDimStyleRcd.Dimtih = false;			/// Aligns text with the dimension line
						acDimStyleRcd.Dimtix = true;			/// force to draw text between ext lines
						acDimStyleRcd.Dimgap = 1;				/// text offset
						acDimStyleRcd.Dimasz = ArrowSizeWidth;	/// arrow size
						acDimStyleRcd.Dimclrt = WriteElm.ParseColor(TextColor);
						acDimStyleRcd.Dimclrd = WriteElm.ParseColor(DimLineColor);
						acDimStyleRcd.Dimclre = WriteElm.ParseColor(ExtLineColor);
					}

					retIds.Add(acDimStyleTbl.Add(acDimStyleRcd));
					myT.AddNewlyCreatedDBObject(acDimStyleRcd, true);
				}
			}

			return retIds;
		}
    }
}
