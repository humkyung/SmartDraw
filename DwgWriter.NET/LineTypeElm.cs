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
	class LineTypeElm : WriteElm
	{
		private string mFileName = string.Empty;
		private string mLineType = string.Empty;
		
		/// <summary>
		/// parse linetype element
		/// </summary>
		/// <param name="aLine"></param>
		/// <param name="oSR"></param>
		/// <returns></returns>
		public override bool Parse(string aLine, StreamReader oSR)
		{
			string[] tokens = aLine.Split("|".ToCharArray());
			if (2 == tokens.Length)
			{
				string[] subtokens = tokens[1].Split(",".ToCharArray());
				if (2 == subtokens.Length)
				{
					mLineType = subtokens[0];
					mFileName = subtokens[1];
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
		/// load line type
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.12.14</date>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
		{
			List<Entity> res = new List<Entity>();
			List<ObjectId> retIds = new List<ObjectId>();

			Document doc = Application.DocumentManager.MdiActiveDocument;
			Editor ed = doc.Editor;

			try
			{
				string path = HostApplicationServices.Current.FindFile(mFileName , bt.Database , FindFileHint.Default);
				bt.Database.LoadLineTypeFile(mLineType, path);
			}
			catch (Autodesk.AutoCAD.Runtime.Exception ex)
			{
				if (ex.ErrorStatus == ErrorStatus.FilerError)
					ed.WriteMessage(
					  "\nCould not find file \"{0}\".",
					  mFileName
					);
				else if (ex.ErrorStatus == ErrorStatus.DuplicateRecordName)
					ed.WriteMessage(
					  "\nCannot load already defined linetypes."
					);
				else
					ed.WriteMessage(
					  "\nException: {0}", ex.Message
					);
			}

			return retIds;
		}
	}
}
