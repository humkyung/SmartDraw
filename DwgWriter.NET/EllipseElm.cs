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
	class EllipseElm : WriteElm
	{
		private Point3d m_ptCenter;
		private double mPrimaryAxis = 0.0 , mSecondaryAxis = 0.0 , mRotation = 0.0;
		
		/// <summary>
		/// parse a ellipse attribute
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.11</date>
		/// <param name="aLine"></param>
		/// <param name="oSR"></param>
		/// <returns></returns>
		public override bool Parse(string aLine, StreamReader oSR)
		{
			string[] tokens = aLine.Split("|".ToCharArray());
			if (3 == tokens.Length)
			{
				string[] subtokens = tokens[1].Split(",".ToCharArray());
				if (5 == subtokens.Length)
				{
					m_ptCenter = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), 0);
					mPrimaryAxis = Convert.ToDouble(subtokens[2]);
					mSecondaryAxis = Convert.ToDouble(subtokens[3]);
					mRotation = Convert.ToDouble(subtokens[4]);
				}
				else if(6 == subtokens.Length)
				{
					m_ptCenter = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), Convert.ToDouble(subtokens[2]));
					mPrimaryAxis = Convert.ToDouble(subtokens[3]);
					mSecondaryAxis = Convert.ToDouble(subtokens[4]);
					mRotation = Convert.ToDouble(subtokens[5]);
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

		/// <summary>
		/// create a instance of ellipse
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.11</date>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
		{
			List<Entity> res = new List<Entity>();
			List<ObjectId> retIds = new List<ObjectId>();

			try
			{
				Ellipse oEllipse = new Ellipse(m_ptCenter, new Vector3d(0.0, 0.0, 1.0), new Vector3d(mPrimaryAxis, 0.0, 0.0), mSecondaryAxis / mPrimaryAxis, 0.0, WriteElm.DEG2RAD(360.0));
				{
					oEllipse.Layer = this.Layer;
					oEllipse.Color = WriteElm.ParseColor(mColor);
					try
					{
						oEllipse.Linetype = WriteElm.ParseLineType(mLtype);
					}
					catch
					{
						oEllipse.Linetype = "ByLayer";
					}
					oEllipse.LinetypeScale = ("Default" != mLtscale) ? Convert.ToDouble(mLtscale) : 1.0;
					oEllipse.LineWeight = WriteElm.ParseLineWeight(mLweight);
				}
				retIds.Add(btr.AppendEntity(oEllipse));
				myT.AddNewlyCreatedDBObject(oEllipse, true);
			}
			catch (Autodesk.AutoCAD.Runtime.Exception ex)
			{
				Document doc = Application.DocumentManager.MdiActiveDocument;
				Autodesk.AutoCAD.EditorInput.Editor ed = doc.Editor;

				ed.WriteMessage("\nEllipseElm Exception: {0}", ex.Message);
			}

			return retIds;
		}
	}
}
