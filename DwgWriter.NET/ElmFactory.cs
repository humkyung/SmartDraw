using System;
using System.Collections.Generic;
using System.Text;

namespace aDwgWriter
{
    public class ElmFactory 
    {
		/// <summary>
		/// create a instance for element corresponding to type
		/// </summary>
		/// <author>humkyung</author>
		/// <date>????-??-??</date>
		/// <param name="aLine"></param>
		/// <returns></returns>
        public static WriteElm CreateElm(string aLine)
        {
			if (0 == aLine.IndexOf("<LINE>"))
				return (new LineElm());

			else if (0 == aLine.IndexOf("<TEXT>"))
				return (new TextElm());

			else if (0 == aLine.IndexOf("<CIRCLE>"))
				return (new CircleElm());

			else if (0 == aLine.IndexOf("<PLINE>"))
				return new PolylineElm();

			else if (0 == aLine.IndexOf("<LEADER>"))
				return new LeaderElm();

			else if (0 == aLine.IndexOf("<DIMENSION>"))
				return (new DimensionElm());

			else if (0 == aLine.IndexOf("<HATCHPLINE>"))
				return (new HatchPolylineElm());

			else if (0 == aLine.IndexOf("<HATCHCIRCLE>"))
				return new HatchCircleElm();

			else if (0 == aLine.IndexOf("<HATCHDONUT>"))
				return new HatchDonutElm();

			else if (0 == aLine.IndexOf("<HATCHRECT>"))
				return new HatchHRecElm();

			else if (0 == aLine.IndexOf("<GROUP>"))
				return new GroupElm();
			else if (0 == aLine.IndexOf("<INSERT>"))
				return new InsertElm();
			/// 2012.12.05 added by humkyung
			else if (0 == aLine.IndexOf("<LAYER>"))
				return new LayerElm();
			/// up to here
			/// 2012.12.06 added by humkyung
			else if (0 == aLine.IndexOf("<DIMSTYLE>"))
				return new DimensionStyleElm();
			else if (0 == aLine.IndexOf("<TEXTSTYLE>"))
				return new TextStyleElm();
			/// up to here
			/// 2012.12.11 added by humkyung
			else if (0 == aLine.IndexOf("<ELLIPSE>"))
				return new EllipseElm();
			/// up to here
			/// 2012.12.14 added by humkyung
			else if (0 == aLine.IndexOf("<LINETYPE>"))
				return new LineTypeElm();
			/// up to here
			else if (0 == aLine.IndexOf(ArcElm.TypeString))
			{
				return new ArcElm();
			}
			else if (0 == aLine.IndexOf(DonutElm.TypeString))
			{
				return new DonutElm();
			}

            return null;
        }
    }
}
