using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using SmartDrawFramework;

namespace UnitTest
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e)
		{
			//SmartDrawFramework.AppFramework AppFramework = new SmartDrawFramework.AppFramework();
            //int res = AppFramework.DumpProjectDatabase("172.16.9.179", "orac", "pd_tr2007", "pd_tr2007" , @"d:\Project\SolutionWare\Projects\SmartDraw\DatabaseDump\PDS.mdb");
            //res = AppFramework.DumpDesignDatabase("172.16.9.179", "orac", "dd_tr2007", "dd_tr2007", @"d:\Project\SolutionWare\Projects\SmartDraw\DatabaseDump\PDS.mdb");
            //int res = AppFramework.WriteModelFolder("172.16.9.179", "orac", "pd_tr2007", "pd_tr2007", @"D:\Project\Server\Sample\Setting\Sample.prj");
            string str = @"D:\Project\Server\Sample\Setting\Sample.prj";
            str = str.Substring(str.IndexOf('\\'));
            string test = str;
		}
	}
}
