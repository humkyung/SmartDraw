using System;
using System.Data;
using System.IO;
using System.Drawing;
using System.Configuration;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.Reflection;

using acApp = Autodesk.AutoCAD.ApplicationServices.Application;
using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.EditorInput;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.Windows;

[assembly: ExtensionApplication(typeof(aDwgWriter.PlugInInitializer))]

namespace aDwgWriter
{
	public class PlugInInitializer : IExtensionApplication
	{
		static PlugInInitializer()
		{
		} 

		public void Initialize()
		{
			try
			{
				//System.Reflection.Assembly asm = System.Reflection.Assembly.GetExecutingAssembly();
				//string sAppRoot = Path.GetDirectoryName(asm.Location);
				//AppDocData.AppIniFilePath = Path.Combine(sAppRoot, "CompareIsoDwg.ini");

				acApp.ShowModelessDialog(acApp.MainWindow.Handle, (Form)CustomToolbar.Instance, false);
			}
			catch (System.Exception ex)
			{
				throw ex;
			}
			finally
			{
			}
		}

		public void Terminate()
		{
			Console.WriteLine("Cleaning up...");
		}
    }
}