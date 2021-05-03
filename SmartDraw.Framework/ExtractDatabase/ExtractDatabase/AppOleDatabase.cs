using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.OleDb;  /// for ADO.NET OLEDb provider

namespace framework.core
{
	class AppOleDatabase : AbstractDatabase<OleDbConnection, OleDbCommand, OleDbDataAdapter>
	{
		public string AccessFilePath { get; set; }

		/// <summary>
		/// connect oralce with host,service,id,password
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.07.17</date>
		/// <param name="sHost"></param>
		/// <param name="sService"></param>
		/// <param name="sId"></param>
		/// <param name="sPassword"></param>
		/// <returns></returns>
		protected override string GetConnectionString()
		{
			return string.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source={0};", AccessFilePath);
		}
	}
}
