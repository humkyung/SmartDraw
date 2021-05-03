using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace UnitTest
{
	static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			if (1 == args.Length)
			{
				Application.EnableVisualStyles();
				Application.SetCompatibleTextRenderingDefault(false);
				Application.Run(new FormExtractOracle(args[0]));
			}
		}
	}
}