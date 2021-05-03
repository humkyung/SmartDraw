using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.Common;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace DataExchange
{
	public partial class FormElevationTypeBySpec : Form
	{
		public string SqliteFilePath { get; set; }
		public string AccessFilePath { get; set; }

		public FormElevationTypeBySpec()
		{
			InitializeComponent();
		}

		private void buttonOK_Click(object sender, EventArgs e)
		{
			using (AppSQLiteDatabase sqlite = new AppSQLiteDatabase() { FilePath = SqliteFilePath })
			{
				using (DbTransaction txn = sqlite.BeginTransaction())
				{
					sqlite.ExecuteNonQuery("delete from ElevationTypeBySpec");
					foreach (DataGridViewRow oGridRow in dataGridViewElevationTypeBySpec.Rows)
					{
						sqlite.ExecuteNonQuery
						(
							string.Format(
							"insert into ElevationTypeBySpec(Spec,ElevationType) values('{0}','{1}')",
							oGridRow.Cells[0].Value, oGridRow.Cells[1].Value)
						);
					}
					txn.Commit();
				}
			}

			this.Close();
		}

		private void buttonClose_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		private void FormElevationTypeBySpec_Load(object sender, EventArgs e)
		{
			Dictionary<string, string> oSpecElevationTypeColl = new Dictionary<string, string>();
			using (AppSQLiteDatabase sqlite = new AppSQLiteDatabase() { FilePath = SqliteFilePath })
			{
				sqlite.ExecuteNonQuery(
				@"create table if not exists ElevationTypeBySpec(
				Spec varchar(32) primary key,
				ElevationType varchar(16) not null)");

				using (DataSet ds = sqlite.ExecuteDataSet(sqlite.GetSqlStringCommand("select Spec,ElevationType from ElevationTypeBySpec")))
				{
					if ((ds.Tables.Count > 0) && ds.Tables[0].Rows.Count > 0)
					{
						foreach (DataRow oRow in ds.Tables[0].Rows)
						{
							oSpecElevationTypeColl.Add(oRow["Spec"].ToString(), oRow["ElevationType"].ToString());
						}
					}
				}
			}

			#region load spec from database
			using (AppOleDatabase access = new AppOleDatabase() { AccessFilePath = AccessFilePath })
			{
				List<string> oPartNoColl = new List<string>();
				using (DataSet ds = access.ExecuteDataSet(access.GetSqlStringCommand("select partition_no from pdtable_113 where discipline_indx_no='1'")))
				{
					if ((ds.Tables.Count > 0) && ds.Tables[0].Rows.Count > 0)
					{
						foreach (DataRow oRow in ds.Tables[0].Rows)
						{
							oPartNoColl.Add(oRow["partition_no"].ToString());
						}
					}
				}

				foreach (string sPartNo in oPartNoColl)
				{
					using (DataSet ds = access.ExecuteDataSet(access.GetSqlStringCommand(string.Format("select distinct piping_mater_class from pdtable_12_{0}", sPartNo))))
					{
						if ((ds.Tables.Count > 0) && ds.Tables[0].Rows.Count > 0)
						{
							foreach (DataRow oRow in ds.Tables[0].Rows)
							{
								string sSpec = oRow["piping_mater_class"].ToString();
								if (!oSpecElevationTypeColl.ContainsKey(sSpec))
								{
									oSpecElevationTypeColl.Add(sSpec, "BOP");
								}
							}
						}
					}
				}
			}
			#endregion

			dataGridViewElevationTypeBySpec.Rows.Add(oSpecElevationTypeColl.Count);
			int iRow = 0;
			foreach (KeyValuePair<string, string> pair in oSpecElevationTypeColl)
			{
				dataGridViewElevationTypeBySpec.Rows[iRow].Cells[0].Value = pair.Key;
				DataGridViewComboBoxCell oComboBoxCell = dataGridViewElevationTypeBySpec.Rows[iRow++].Cells[1] as DataGridViewComboBoxCell;
				if (null != oComboBoxCell)
				{
					oComboBoxCell.Value = pair.Value;
				}
			}
		}
	}
}
