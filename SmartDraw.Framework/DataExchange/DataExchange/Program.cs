using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace DataExchange
{
	static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			if (2 == args.Length)
			{
				Application.Run(new DataExchange(args[0], args[1]));
			}
			else if(3 == args.Length)
			{
				if ("ElevationTypeBySpec" == args[0])
				{
					Application.Run(new FormElevationTypeBySpec() {SqliteFilePath=args[1],AccessFilePath=args[2] });
				}
				else
				{
					Application.Run(new DataExchange(args[0], args[1]) { Visible = false, TemplateXlsFilePath = args[2] });
				}
			}
		}
	}
}