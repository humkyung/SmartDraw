using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

using System.IO;
using System.Collections;
using System.Runtime.InteropServices;
using System.Diagnostics;

using Autodesk.AutoCAD.DatabaseServices;

namespace aDwgWriter
{
    interface WriteElmIO
    {
        bool Parse(string aLine , StreamReader oSR);
		List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT);
    }

    public class WriteElm : WriteElmIO
    {
		public static readonly double PI = 3.141592653589793238462643383279502884197169399375105820974944592308;
        public static double DEG2RAD(double deg)
        {
			return (deg * PI / 180);
        }

        public virtual bool Parse(string aLine, StreamReader oSR) { return true; }
		public virtual List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT) { return null; }

        /// <summary>
        /// get color from string
        /// </summary>
        /// <author>humkyung</author>
        /// <date>2012.10.23</date>
        /// <param name="sColor"></param>
        /// <returns></returns>
        public static Autodesk.AutoCAD.Colors.Color ParseColor(string sColor)
        {
            Autodesk.AutoCAD.Colors.Color color;
            switch(sColor)
            {
                case "White":
                    color = Autodesk.AutoCAD.Colors.Color.FromRgb(255,255,255);
                    break;
                case "Red":
                    color = Autodesk.AutoCAD.Colors.Color.FromRgb(255,0,0);
                    break;
                case "Blue":
                    color = Autodesk.AutoCAD.Colors.Color.FromRgb(0,0,255);
                    break;
                case "Green":
                    color = Autodesk.AutoCAD.Colors.Color.FromRgb(0,255,0);
                    break;
                case "Yellow":
                    color = Autodesk.AutoCAD.Colors.Color.FromRgb(255,255,0);
                    break;
                case "Magenta":
                    color = Autodesk.AutoCAD.Colors.Color.FromRgb(255,0,255);
                    break;
	            case "Cyan":
                    color = Autodesk.AutoCAD.Colors.Color.FromRgb(0,255,255);
                    break;
	            case "ByBlock":
                    color = Autodesk.AutoCAD.Colors.Color.FromColorIndex(Autodesk.AutoCAD.Colors.ColorMethod.ByBlock,0);
                    break;
                case "ByLayer":
                    color = Autodesk.AutoCAD.Colors.Color.FromColorIndex(Autodesk.AutoCAD.Colors.ColorMethod.ByLayer,256);
                    break;
                default:
					short colorIndex = 256;
					try
					{
						colorIndex = ((!string.IsNullOrEmpty(sColor)) ? Convert.ToInt16(sColor) : (short)256);
					}
					catch
					{
						colorIndex = 256;
					}
                    color = Autodesk.AutoCAD.Colors.Color.FromColorIndex(Autodesk.AutoCAD.Colors.ColorMethod.ByColor , colorIndex);
                break;
            }

            return color;
        }

		/// <summary>
		/// return ByLayer if parameter is "0"
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.14</date>
		/// <param name="sLineType"></param>
		/// <returns></returns>
		public static string ParseLineType(string sLineType)
		{
			string res = (string.IsNullOrEmpty(sLineType) || ("0" == sLineType)) ? "ByLayer" : sLineType;

			return res;
		}

        /// <summary>
        /// get line weight from string
        /// </summary>
        /// <author>humkyung</author>
        /// <date>2012.10.23</date>
        /// <param name="sColor"></param>
        /// <returns></returns>
        public static LineWeight ParseLineWeight(string sLineWeight)
        {
            LineWeight res = LineWeight.ByLayer;
            if ("Default" != sLineWeight)
            {
                try
                {
                    int iLineWeight = Convert.ToInt16(sLineWeight)*100;
                    switch (iLineWeight)
                    {
                        case 0:
                            res = LineWeight.LineWeight000;
                            break;
                        case 5:
                            res = LineWeight.LineWeight005;
                            break;
                        case 9:
                            res = LineWeight.LineWeight009;
                            break;
                        case 13:
                            res = LineWeight.LineWeight013;
                            break;
                        case 15:
                            res = LineWeight.LineWeight015;
                            break;
                        case 18:
                            res = LineWeight.LineWeight018;
                            break;
                        case 20:
                            res = LineWeight.LineWeight020;
                            break;
                        case 25:
                            res = LineWeight.LineWeight025;
                            break;
                        case 30:
                            res = LineWeight.LineWeight030;
                            break;
                        case 35:
                            res = LineWeight.LineWeight035;
                            break;
                        case 40:
                            res = LineWeight.LineWeight040;
                            break;
                        case 50:
                            res = LineWeight.LineWeight050;
                            break;
                        case 53:
                            res = LineWeight.LineWeight053;
                            break;
                        case 60:
                            res = LineWeight.LineWeight060;
                            break;
                        case 70:
                            res = LineWeight.LineWeight070;
                            break;
                        case 80:
                            res = LineWeight.LineWeight080;
                            break;
                        case 90:
                            res = LineWeight.LineWeight090;
                            break;
                        case 100:
                            res = LineWeight.LineWeight100;
                            break;
                        case 106:
                            res = LineWeight.LineWeight106;
                            break;
                        case 120:
                            res = LineWeight.LineWeight120;
                            break;
                        case 140:
                            res = LineWeight.LineWeight140;
                            break;
                        case 158:
                            res = LineWeight.LineWeight158;
                            break;
                        case 200:
                            res = LineWeight.LineWeight200;
                            break;
                        case 211:
                            res = LineWeight.LineWeight211;
                            break;
                    }
                }
                catch (FormatException)
                {
                }
            }
	
            return res;
        }

        /// <summary>
        /// parse line scale
        /// </summary>
        /// <author>humkyung</author>
        /// <date>2012.10.29</date>
        /// <param name="sLtscale"></param>
        /// <returns></returns>
        protected double ParseLineScale(string sLtscale)
        {
            double res = 1.0;
            try
            {
                res = Convert.ToDouble(sLtscale);
            }
            catch (FormatException)
            {
            }

            return res;
        }

        protected string mLayer, mColor, mLtype, mLtscale, mLweight;
     
        public string Layer
        {
            get { return mLayer; }
        }
        public string Color
        {
            get { return mColor; }
        }

		protected WriteElm()
		{
			mLtype = "ByLayer";
		}
    }
}
