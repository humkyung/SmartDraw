using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Data;

using GemBox.Spreadsheet;

namespace DataExchange
{
	class ReportCommand
	{
		public string SqliteFilePath { get; set; }
		public string TemplateXlsFilePath { get; set; }
		const string GemBoxLicense = "EXK0-347T-N5GE-K8Y1";

		/// <summary>
		/// report to excel file
		/// </summary>
		public void Execute()
		{
			using (AppSQLiteDatabase database = new AppSQLiteDatabase() { FilePath = SqliteFilePath })
			{
				FileDialog dlg = new SaveFileDialog() { DefaultExt = ".xlsx", Filter = "Excel File(*.xlsx)|*.xlsx" };
				if (DialogResult.OK == dlg.ShowDialog())
				{
					WaitDlgModeless oWaitDlgModeless = (WaitDlgModeless)null;
					try
					{
						oWaitDlgModeless = new WaitDlgModeless();
						oWaitDlgModeless.SetMessage("Reporting ...");
						oWaitDlgModeless.Show(DataExchange.Instance);

						File.Copy(TemplateXlsFilePath, dlg.FileName, true);

						SpreadsheetInfo.SetLicense(GemBoxLicense);
						ExcelFile oExlFile = new ExcelFile();
						oExlFile.LoadXlsx(dlg.FileName, XlsxOptions.PreserveMakeCopy);

						List<string> oReportList = new List<string>();
						using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("select * from reports")))
						{
							if ((null != ds.Tables) && (ds.Tables[0].Rows.Count > 0))
							{
								foreach (DataRow oRow in ds.Tables[0].Rows)
								{
									oReportList.Add(oRow[0].ToString());
								}
							}
						}

						foreach (string sReportName in oReportList)
						{
							try
							{
								ExcelWorksheet oSheet = oExlFile.Worksheets.FirstOrDefault(param => param.Name == sReportName);
								if (null == oSheet) continue;

								///get row,column index and cell style information
								int iStartRow = 0;
								Dictionary<string, KeyValuePair<int, CellStyle>> oColList = new Dictionary<string, KeyValuePair<int, CellStyle>>();
								for (int iRow = 0; iRow < int.MaxValue; ++iRow)
								{
									if (null == oSheet.Cells[iRow, 0].Value) break;
									string sValue = oSheet.Cells[iRow, 0].Value.ToString();
									if (('[' == sValue[0]) && (']' == sValue[sValue.Length - 1]))
									{
										iStartRow = iRow;
										for (int iCol = 0; iCol < int.MaxValue; ++iCol)
										{
											if (null == oSheet.Cells[iRow, iCol].Value) break;
											sValue = oSheet.Cells[iRow, iCol].Value.ToString();
											if (('[' == sValue[0]) && (']' == sValue[sValue.Length - 1]))
											{
												string sColName = sValue.Substring(1, sValue.Length - 2).ToUpper();
												oColList.Add(sColName, new KeyValuePair<int, CellStyle>(iCol, oSheet.Cells[iRow, iCol].Style));
											}
											else
											{
												break;
											}
										}
										break;
									}
								}
								///up to here

								using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(string.Format("select C.Name as DrawingName,A.* from {0} A join View B on A.ViewGuid=B.Guid join Drawing C on B.DocGuid=C.Guid", sReportName))))
								{
									int iRow = iStartRow;
									foreach (DataRow oRow in ds.Tables[0].Rows)
									{
										foreach (DataColumn oCol in ds.Tables[0].Columns)
										{
											if (oColList.ContainsKey(oCol.ColumnName.ToUpper()))
											{
												int iCol = oColList[oCol.ColumnName.ToUpper()].Key;
												oSheet.Cells[iRow, iCol].Value = oRow[oCol];
												oSheet.Cells[iRow, iCol].Style = oColList[oCol.ColumnName.ToUpper()].Value;
											}
										}
										++iRow;
									}
								}
							}
							catch (Exception ex)
							{
								System.Windows.Forms.MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
							}
						}
						oExlFile.SaveXlsx(dlg.FileName);
						System.Diagnostics.Process.Start(dlg.FileName);
					}
					catch (Exception ex)
					{
						System.Windows.Forms.MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					}
					finally
					{
						if (null != oWaitDlgModeless)
						{
							oWaitDlgModeless.Close();
						}
					}
				}
			}
		}
	}
}
