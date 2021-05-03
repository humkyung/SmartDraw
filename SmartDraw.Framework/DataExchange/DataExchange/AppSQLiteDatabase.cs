using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SQLite;

namespace DataExchange
{
	public class AppSQLiteDatabase : AbstractDatabase<SQLiteConnection, SQLiteCommand, SQLiteDataAdapter>
	{
		public string FilePath { get; set; }

		/// <summary>
		/// return connection string for sqlite
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2015.02.05</date>
		/// <returns></returns>
		protected override string GetConnectionString()
		{
			string connString = string.Format("Data Source={0}" , FilePath);
			return connString;
		}
	}
}
