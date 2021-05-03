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
	class InsertElm : WriteElm
	{
		private string mFilePath = string.Empty;
		private string mCellName = string.Empty;
		private Point3d mInsert;
		private string mWhere = string.Empty;
		private Point2d mScale;
		private double mAngle = 0.0;
		private string mExplode = "No";

		/// <summary>
		/// parse Insert element
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.11.05</date>
		/// <param name="aLine"></param>
		/// <param name="oSR"></param>
		/// <returns></returns>
		public override bool Parse(string aLine, StreamReader oSR)
		{
			string[] tokens = aLine.Split("|".ToCharArray());
			if (3 == tokens.Length)
			{
				string[] subtokens = tokens[1].Split(",".ToCharArray());
				if (8 == subtokens.Length)
				{
					mFilePath = subtokens[0];
					mCellName = subtokens[1];
					mInsert = new Point3d(Convert.ToDouble(subtokens[2]) , Convert.ToDouble(subtokens[3]) , 0);
					mWhere = subtokens[4];
					mScale = new Point2d(Convert.ToDouble(subtokens[5]), Convert.ToDouble(subtokens[6]));
					mAngle = DEG2RAD(Convert.ToDouble(subtokens[7]));
				}
				else
				{
					return false;
				}

				subtokens = tokens[2].Split(",".ToCharArray());
				if (2 == subtokens.Length)
				{
					mLayer = subtokens[0];
					mExplode = subtokens[1];
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
		/// create a block by using file
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.11.05</date>
		/// <param name="bt"></param>
		/// <returns></returns>
		private List<ObjectId> CreateBlockByUsingFile(BlockTable bt)
		{
			List<ObjectId> retids = new List<ObjectId>();

			Document doc = Application.DocumentManager.MdiActiveDocument;

			string sBlockName = Path.GetFileNameWithoutExtension(mFilePath);
			if (!bt.Has(sBlockName))
			{
				using (Database newdb = new Database(false, true))
				{
					/// Read the DWG into our side database
					newdb.ReadDwgFile(mFilePath, FileShare.Read, true, "");

					ObjectIdCollection ids = new ObjectIdCollection();
					Transaction tr = newdb.TransactionManager.StartTransaction();
					using (tr)
					{
						///Open the block table 
						BlockTable newbt = (BlockTable)tr.GetObject(newdb.BlockTableId, OpenMode.ForRead);

						/// Add the new block to the block table
						ids.Add(newbt[BlockTableRecord.ModelSpace]);

						tr.Commit();
					}

					/// Copy our modelspace block from the source to
					/// destination database
					/// (will also copy required, referenced objects)

					IdMapping im = new IdMapping();
					newdb.WblockCloneObjects(
						ids,
						bt.Database.BlockTableId,
						im,
						DuplicateRecordCloning.MangleName,
						false
					);

					using (Transaction tr2 = bt.Database.TransactionManager.StartTransaction())
					{
						/// Work through the results of the WblockClone
						foreach (IdPair ip in im)
						{
							/// Open each new destination object, checking for
							/// BlockTableRecords
							BlockTableRecord btr = tr2.GetObject(ip.Value, OpenMode.ForRead) as BlockTableRecord;
							if (btr != null)
							{
								/// If the name starts with the modelspace string
								if (btr.Name.StartsWith(BlockTableRecord.ModelSpace, StringComparison.InvariantCultureIgnoreCase))
								{
									/// Get write access to it and change the name
									/// to that of the source drawing
									btr.UpgradeOpen();
									btr.Name = sBlockName;
								}

								retids.Add(btr.ObjectId);
							}
						}

						/// We need to commit, as we've made changes
						tr2.Commit();
					}
				}
			}
			/// 2012.11.29 added by humkyung
			else
			{
				SymbolTableEnumerator oEnumerator = bt.GetEnumerator();
				while (oEnumerator.MoveNext())
				{
					BlockTableRecord oBlkTblRcd = oEnumerator.Current.GetObject(OpenMode.ForRead) as BlockTableRecord;
					if (sBlockName == oBlkTblRcd.Name)
					{
						retids.Add(oBlkTblRcd.Id);
					}
				}
			}
			/// up to here

			return retids;
		}

		/// <summary>
		/// create insert entity
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.11.05</date>
		/// <param name="bt"></param>
		/// <param name="btr"></param>
		/// <param name="tm"></param>
		/// <returns></returns>
		public override List<ObjectId> CreateEntity(BlockTable bt, BlockTableRecord btr, Autodesk.AutoCAD.DatabaseServices.Transaction myT)
		{
			List<ObjectId> retids = new List<ObjectId>();

			List<ObjectId> ids = CreateBlockByUsingFile(bt);
			if(ids.Count > 0)
			{
				BlockReference br = new BlockReference(mInsert, ids[0]);
				if (null != br)
				{
					br.Rotation = mAngle;
					br.ScaleFactors = new Scale3d(mScale.X);	/// 2012.11.29 added by humkyung
					retids.Add(btr.AppendEntity(br));
					myT.AddNewlyCreatedDBObject(br, true);
				}
			}

			return retids;
		}

	}
}
