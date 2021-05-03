using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Data.OleDb;  /// for ADO.NET OLEDb provider
using System.Data.OracleClient;

namespace SmartDrawFramework
{
    public partial class SmartDrawUserControl : UserControl
    {
		private static string GemBoxLicense = "EXK0-347T-N5GE-K8Y1";

		public string Host { get; set; }
		public string Service { get; set; }
		public string ProjectId { get; set; }
		public string ProjectPassword { get; set; }
		public string DesignId { get; set; }
		public string DesignPassword { get; set; }
		public string AccessFilePath { get; set; }
		public string SettingFilePath { get; set; }
		
        public SmartDrawUserControl()
        {
            InitializeComponent();
        }

		/// <summary>
		/// do oracle connection test
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2014.05.03</date>
		/// <returns></returns>
		public bool DoOracleConnectionTest(string sHost , string sService , string sId , string sPassword)
		{
			bool res = false;

			using (MyOracleConnection conn = new MyOracleConnection())
			{
				res = conn.Connect(sHost , sService, sId , sPassword);
			}

			return res;
		}
    }
}
