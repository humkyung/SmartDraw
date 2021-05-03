using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.ApplicationServices;

namespace aDwgWriter
{
	class DonutElm : WriteElm
	{
		public static string TypeString
		{
			get { return "<DONUT>"; }
		}
		private Point3d m_ptCenter;
		private double m_dInsideDia , m_dOutsideDia;
		private string mPattern = "SOLID";

		/// <summary>
		/// parse donut
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
				if (4 == subtokens.Length)
				{
					m_dInsideDia = Convert.ToDouble(subtokens[0]);
					m_dOutsideDia = Convert.ToDouble(subtokens[1]);
					m_ptCenter = new Point3d(Convert.ToDouble(subtokens[2]), Convert.ToDouble(subtokens[3]), 0);
				}
				else
				{
					return false;
				}

				subtokens = tokens[2].Split(",".ToCharArray());
				if (2 == subtokens.Length)
				{
					mLayer = subtokens[0];
					mColor = subtokens[1];
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

			Circle oCircle = new Circle(m_ptCenter, Vector3d.ZAxis, m_dOutsideDia*0.5);
			{
				oCircle.Layer = this.Layer;
				oCircle.Color = WriteElm.ParseColor(mColor);
				oCircle.Linetype = "ByLayer";
				oCircle.LinetypeScale = 0;
				oCircle.LineWeight = LineWeight.LineWeight000;
			}
			ids.Add(btr.AppendEntity(oCircle));
			retIds.Add(ids[0]);
			myT.AddNewlyCreatedDBObject(oCircle, true);

			try
			{
				Hatch oHatch = new Hatch();
				oHatch.Normal = Vector3d.ZAxis;
				oHatch.SetDatabaseDefaults();
				oHatch.PatternScale = 1;
				oHatch.SetHatchPattern(HatchPatternType.PreDefined, mPattern);
				oHatch.Color = WriteElm.ParseColor(mColor);
				oHatch.AppendLoop(HatchLoopTypes.Default, ids);
				oHatch.EvaluateHatch(true);
				retIds.Add(btr.AppendEntity(oHatch));
				myT.AddNewlyCreatedDBObject(oHatch, true);
			}
			catch { }

			return retIds;
		}
	}
}
