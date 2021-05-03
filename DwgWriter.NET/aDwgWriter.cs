// Copyright 2004-2007 by Autodesk, Inc.
//
//Permission to use, copy, modify, and distribute this software in
//object code form for any purpose and without fee is hereby granted, 
//provided that the above copyright notice appears in all copies and 
//that both that copyright notice and the limited warranty and
//restricted rights notice below appear in all supporting 
//documentation.
//
//AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
//AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
//MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
//DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
//UNINTERRUPTED OR ERROR FREE.
//
//Use, duplication, or disclosure by the U.S. Government is subject to 
//restrictions set forth in FAR 52.227-19 (Commercial Computer
//Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
//(Rights in Technical Data and Computer Software), as applicable.

using System;
using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.ApplicationServices;
using System.Reflection;

using System.IO;
using System.Collections;
using System.Runtime.InteropServices;
using System.Diagnostics;
using Autodesk.AutoCAD.EditorInput;
using Autodesk.AutoCAD.GraphicsInterface;
using Autodesk.AutoCAD.Colors;

[assembly: CommandClass(typeof(aDwgWriter.DwgWriter))]


//This application implements a command called ellipsejig. It will help you 
//create an ellipse from scratch by doing a jig. The user is first asked to
//enter the ellipse major axis followed by ellipse minor axis. 

//To use ellipsejig.dll:
//1. Start AutoCAD and open a new drawing.
//2. Type netload and select aDwgWrite.dll.
//3. Execute the ellipsejig command, defined by aDwgWrite.dll.

//Please add the References acdbmgd.dll,acmgd.dll,
//Autodesk.AutoCAD.Interop.dll and Autodesk.AutoCAD.Interop.Common.dll
//before trying to build this project.

namespace aDwgWriter
{
    public class DwgWriter
    {
        [CommandMethod("MNL")]
        static public void MyNetLoad()
        {
          Document doc =
            Application.DocumentManager.MdiActiveDocument;
          Editor ed = doc.Editor;

          PromptStringOptions pso =
            new PromptStringOptions(
              "\nEnter the path of the module to load: "
            );
          pso.AllowSpaces = true;

          PromptResult pr = ed.GetString(pso);
          if (pr.Status != PromptStatus.OK)
            return;

          try
          {
            Assembly.LoadFrom(pr.StringResult);
          }
          catch(System.Exception ex)
          {
            ed.WriteMessage(
              "\nCannot load {0}: {1}",
              pr.StringResult,
              ex.Message
            );
          }
        }

		[CommandMethod("DwgWriter")]
		public static void CommandDwgWriter()
		{
			PromptStringOptions opts = new PromptStringOptions("\nEnter File Path:");
			PromptResult res = Application.DocumentManager.MdiActiveDocument.Editor.GetString(opts);
			if (PromptStatus.OK == res.Status)
			{
				int iFileCount = 0;
				{
					using (StreamReader sr = new StreamReader(res.StringResult))
					{
						while (!sr.EndOfStream)
						{
							string aLine = sr.ReadLine();
							if (0 == aLine.IndexOf("NEWFILE")) iFileCount++;
						}
					}
				}

				Application.ShowModelessDialog(Application.MainWindow.Handle, (System.Windows.Forms.Form)ProgressLogDlg.Instance, false);
				//ProgressLogDlg.Instance.listViewLog.Items.Clear();
				ProgressLogDlg.Instance.buttonClose.Enabled = false;

				ProgressLogDlg.Instance.progressBarStatus.Maximum = iFileCount;
				ProgressLogDlg.Instance.progressBarStatus.Minimum = 0;
				ProgressLogDlg.Instance.progressBarStatus.Step = 1;
				using (StreamReader sr = new StreamReader(res.StringResult))
				{
					string sFilePath = string.Empty;
					while (!sr.EndOfStream)
					{
						string aLine = sr.ReadLine();
						if (0 == aLine.IndexOf("NEWFILE"))
						{
							sFilePath = aLine.Substring("NEWFILE ".Length);
							if (File.Exists(sFilePath))
							{
								Database oldDb = HostApplicationServices.WorkingDatabase;

								string tmp = Path.Combine(Path.GetTempPath(), Path.GetFileName(sFilePath));
								File.Copy(sFilePath, tmp, true);
								using (Database database = new Database(false, true))
								{
									ProgressLogDlg.Instance.textBoxFileName.Text = Path.GetFileName(sFilePath);
									database.ReadDwgFile(sFilePath, FileOpenMode.OpenForReadAndWriteNoShare, false, string.Empty);
									database.CloseInput(true);
									HostApplicationServices.WorkingDatabase = database;

									while (!sr.EndOfStream)
									{
										aLine = sr.ReadLine();
										if (0 == aLine.IndexOf("SAVE DESIGN"))
										{
											HostApplicationServices.WorkingDatabase = oldDb;
											database.SaveAs(sFilePath, DwgVersion.Current);

											ProgressLogDlg.Instance.progressBarStatus.PerformStep();
											System.Windows.Forms.Application.DoEvents();
											break;
										}
										if (File.Exists(aLine)) Draw(database, aLine);
									}
								}
							}
						}
					}
				}
				ProgressLogDlg.Instance.buttonClose.Enabled = true;
			}
		}

		private static void Draw(Database database , string sFilePath)
		{
			using (StreamReader sr = new StreamReader(sFilePath))
			{
				while (!sr.EndOfStream)
				{
					WriteElm oWriteElm = null;

					string aLine = sr.ReadLine();
					oWriteElm = ElmFactory.CreateElm(aLine);

					if (oWriteElm == null) continue;
					if (false == oWriteElm.Parse(aLine, sr)) continue;

					if (null != oWriteElm)
					{
						/// write a element
						using (Transaction myT = database.TransactionManager.StartTransaction())
						{
							BlockTable bt = (BlockTable)myT.GetObject(database.BlockTableId, OpenMode.ForRead, false);
							BlockTableRecord btr = (BlockTableRecord)myT.GetObject(bt[BlockTableRecord.ModelSpace], OpenMode.ForWrite, false);
							if (!(oWriteElm is LayerElm))
							{
								LayerTable acLyrTbl = (LayerTable)myT.GetObject(database.LayerTableId, OpenMode.ForRead);
								if (!acLyrTbl.Has(oWriteElm.Layer) && !string.IsNullOrEmpty(oWriteElm.Layer))
								{
									LayerTableRecord acLyrTblRcd = new LayerTableRecord();

									/// ... and set its properties
									acLyrTblRcd.Name = oWriteElm.Layer;
									///ltr.Color = WriteElm.ParseColor();

									/// Add the new layer to the layer table
									acLyrTbl.UpgradeOpen();
									ObjectId ltId = acLyrTbl.Add(acLyrTblRcd);
									myT.AddNewlyCreatedDBObject(acLyrTblRcd, true);

									/// Set the layer to be current for this drawing
									database.Clayer = ltId;
								}
							}

							oWriteElm.CreateEntity(bt, btr, myT);
							myT.Commit();
						}
					}
				}
			}
		}

        [CommandMethod("sdwf")]
        static public void DoIt()
        {
            Editor ed = Application.DocumentManager.MdiActiveDocument.Editor;            

            PromptStringOptions opts = new PromptStringOptions("\nEnter File Path:");
            PromptResult res = ed.GetString(opts);
            if (PromptStatus.OK == res.Status)
            {
                Vector3d x = Application.DocumentManager.MdiActiveDocument.Database.Ucsxdir;
                Vector3d y = Application.DocumentManager.MdiActiveDocument.Database.Ucsydir;
                Vector3d NormalVec = x.CrossProduct(y);

                Database db = Application.DocumentManager.MdiActiveDocument.Database;
                
                Autodesk.AutoCAD.DatabaseServices.TransactionManager tm = db.TransactionManager;

                /// Append entity.
                using (StreamReader oSR = new StreamReader(res.StringResult))
                {
                    while (!oSR.EndOfStream)
                    {
                        WriteElm oWriteElm = null;
                       
                        string aLine = oSR.ReadLine();
                        oWriteElm = ElmFactory.CreateElm(aLine);

                        if (oWriteElm == null) continue;
                        if (false == oWriteElm.Parse(aLine, oSR)) continue;
                        
                        if(null != oWriteElm)
                        {
                            /// write a element
                            using (Transaction myT = tm.StartTransaction())
                            {
								BlockTable bt = (BlockTable)tm.GetObject(db.BlockTableId, OpenMode.ForRead, false);
								BlockTableRecord btr = (BlockTableRecord)tm.GetObject(bt[BlockTableRecord.ModelSpace], OpenMode.ForWrite, false);
								if (!(oWriteElm is LayerElm))
								{	
									LayerTable lt = (LayerTable)myT.GetObject(db.LayerTableId, OpenMode.ForRead);
									if (!lt.Has(oWriteElm.Layer) && !string.IsNullOrEmpty(oWriteElm.Layer))
									{
										LayerTableRecord ltr = new LayerTableRecord();

										/// ... and set its properties
										ltr.Name = oWriteElm.Layer;
										///ltr.Color = WriteElm.ParseColor();

										/// Add the new layer to the layer table
										lt.UpgradeOpen();
										ObjectId ltId = lt.Add(ltr);
										myT.AddNewlyCreatedDBObject(ltr, true);

										/// Set the layer to be current for this drawing
										db.Clayer = ltId;
									}
								}

								oWriteElm.CreateEntity(bt , btr, myT);
                                myT.Commit();
                            }
                        }
                    }
                }
            }
        }
    }
}
