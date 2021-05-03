using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

using GemBox.Spreadsheet;

namespace DataExchange
{
	public partial class DataExchange : Form
	{
		private static DataExchange s_oInstance;
		public static DataExchange Instance
		{
			get
			{
				return DataExchange.s_oInstance;
			}
		}

		public enum eConvertType
		{
			None = 0,
			Export = 1,
			Import = 2,
			Report = 3
		};
		private eConvertType ConvertType { get; set; }
		private string SqliteFilePath { get; set; }
		public string TemplateXlsFilePath { get; set; }

		private List<string> PartionNoList = new List<string>();

		public DataExchange(string sConvertType, string sSqliteFilePath)
		{
			InitializeComponent();
			DataExchange.s_oInstance = this;

			ConvertType = eConvertType.None;
			if (DataExchange.eConvertType.Export.ToString() == sConvertType)
				ConvertType = DataExchange.eConvertType.Export;
			else if (DataExchange.eConvertType.Import.ToString() == sConvertType)
				ConvertType = DataExchange.eConvertType.Import;
			else if (DataExchange.eConvertType.Report.ToString() == sConvertType)
				ConvertType = DataExchange.eConvertType.Report;

			SqliteFilePath = sSqliteFilePath;
		}

		private void buttonClose_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void FormPipeDiaEditor_Load(object sender, EventArgs e)
		{
		}

		private void FormPipeDiaEditor_Shown(object sender, EventArgs e)
		{
			const string GemBoxLicense = "EXK0-347T-N5GE-K8Y1";

			try
			{
				this.Visible = false;

				if (eConvertType.Export == ConvertType)
				{
					using (AppSQLiteDatabase database = new AppSQLiteDatabase() { FilePath = SqliteFilePath })
					{
						FileDialog dlg = new SaveFileDialog() { DefaultExt = ".xlsx", Filter = "Excel File(*.xlsx)|*.xlsx" };
						if (DialogResult.OK == dlg.ShowDialog())
						{
							SpreadsheetInfo.SetLicense(GemBoxLicense);
							ExcelFile oExlFile = new ExcelFile();

							List<string> oCodeList = new List<string>();
							using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("select * from codes")))
							{
								if ((null != ds.Tables) && (ds.Tables[0].Rows.Count > 0))
								{
									foreach (DataRow oRow in ds.Tables[0].Rows)
									{
										oCodeList.Add(oRow[0].ToString().ToUpper());
									}
								}
							}

							foreach (string sCodeName in oCodeList)
							{
								ExcelWorksheet oSheet = oExlFile.Worksheets.Add(sCodeName);
								using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(string.Format("select FromSize,ToSize,OD from {0}", sCodeName))))
								{
									int iRow = 0;
									foreach (DataRow oRow in ds.Tables[0].Rows)
									{
										oSheet.Cells[iRow, 0].Value = oRow[0].ToString();
										oSheet.Cells[iRow, 1].Value = oRow[1].ToString();
										oSheet.Cells[iRow++, 2].Value = oRow[2].ToString();
									}
								}
							}
							oExlFile.SaveXlsx(dlg.FileName);
						}
					}
				}
				else if (eConvertType.Report == ConvertType)
				{
					ReportCommand cmd = new ReportCommand() { SqliteFilePath = SqliteFilePath, TemplateXlsFilePath = TemplateXlsFilePath };
					cmd.Execute();
				}
				else if (eConvertType.Import == ConvertType)
				{
					using (AppSQLiteDatabase database = new AppSQLiteDatabase() { FilePath = SqliteFilePath })
					{
						FileDialog dlg = new OpenFileDialog() { CheckFileExists = true, DefaultExt = ".xlsx", Filter = "Excel File(*.xlsx)|*.xlsx" };
						if (DialogResult.OK == dlg.ShowDialog(this))
						{
							SpreadsheetInfo.SetLicense(GemBoxLicense);
							ExcelFile oExlFile = new ExcelFile();
							oExlFile.LoadXlsx(dlg.FileName, XlsxOptions.PreserveMakeCopy);
							try
							{
								using (System.Data.Common.DbTransaction txn = database.BeginTransaction())
								{
									/// truncate codes table
									string sSql = "delete from codes";
									database.ExecuteNonQuery(sSql);
									/// up to here

									foreach (ExcelWorksheet oSheet in oExlFile.Worksheets)
									{
										sSql = string.Format(
											@"create table if not exists {0}(
											FromSize real not null,
											ToSize real not null,
											Inch real not null,
											OD real not null)", oSheet.Name);
										database.ExecuteNonQuery(sSql);

										sSql = string.Format("delete from {0}", oSheet.Name);
										database.ExecuteNonQuery(sSql);

										CellRange oRange = oSheet.GetUsedCellRange();
										for (int iRow = oRange.FirstRowIndex; iRow <= oRange.LastRowIndex; ++iRow)
										{
											object oValue = oRange[iRow, 0].Value;
											string sFromSize = (null != oValue) ? oValue.ToString() : string.Empty;
											oValue = oRange[iRow, 1].Value;
											string sToSize = (null != oValue) ? oValue.ToString() : string.Empty;
											oValue = oRange[iRow, 2].Value;
											string sOD = (null != oValue) ? oValue.ToString() : string.Empty;

											sSql = string.Format("insert into {0}(FromSize,ToSize,Inch,OD) values({1},{2},0,{3})",
												oSheet.Name,sFromSize, sToSize, sOD);
											database.ExecuteNonQuery(sSql);
										}

										sSql = string.Format("insert into codes values('{0}')", oSheet.Name);
										database.ExecuteNonQuery(sSql);
									}
									txn.Commit();
								}
							}
							catch { }
						}
					}
				}
				toolStripStatusLabelProgress.ForeColor = Color.Blue;
				toolStripStatusLabelProgress.Text = "Dump is done.";
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}

			this.Close();
		}
	}
}
