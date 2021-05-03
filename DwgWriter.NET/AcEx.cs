using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Autodesk.AutoCAD.ApplicationServices;
using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.EditorInput;

namespace aDwgWriter
{
	public class AcEx
	{
		public static Database Database
		{
			get
			{
				return HostApplicationServices.WorkingDatabase;
			}
		}

		public static Document Document
		{
			get
			{
				return Application.DocumentManager.MdiActiveDocument;
			}
		}

		public static Editor Editor
		{
			get
			{
				return Application.DocumentManager.MdiActiveDocument.Editor;
			}
		}

		public static bool IsActiveDocumentQuiescent
		{
			get
			{
				if (null == AcEx.Document) return false;
				if (string.IsNullOrEmpty(AcEx.Document.CommandInProgress))
				{
					return AcEx.Editor.IsQuiescent;
				}

				return false;
			}
		}

		/// <summary>
		/// create application name
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2014.12.15</date>
		/// <param name="regAppName"></param>
		public static void AddRegAppTableRecord(Database db, string regAppName)
		{
			using (Transaction txn = db.TransactionManager.StartTransaction())
			{
				RegAppTable acRegAppTbl = (RegAppTable)txn.GetObject(db.RegAppTableId, OpenMode.ForRead, false);
				if (!acRegAppTbl.Has(regAppName))
				{
					acRegAppTbl.UpgradeOpen();
					RegAppTableRecord acRegAppTblRcd = new RegAppTableRecord();
					acRegAppTblRcd.Name = regAppName;
					acRegAppTbl.Add(acRegAppTblRcd);
					txn.AddNewlyCreatedDBObject(acRegAppTblRcd, true);
				}
				txn.Commit();
			}
		}

		private AcEx()
		{
		}
	}
}
