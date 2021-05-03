using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Data.OleDb;  /// for ADO.NET OLEDb provider
using System.Data.OracleClient;

namespace UnitTest
{
	public partial class FormExtractOracle : Form
	{
		private string IniFilePath { get; set; }
		private string Host { get; set; }
		private string Service { get; set; }
		private string PrjDatabaseUser { get; set; }
		private string PrjDatabasePassword { get; set; }
		private string DesignDatabaseUser { get; set; }
		private string DesignDatabasePassword { get; set; }
		private string AccessFilePath { get; set; }

		private List<string> PartionNoList = new List<string>();

		public FormExtractOracle(string sIniFilePath)
		{
			InitializeComponent();
			IniFilePath = sIniFilePath;
		}

		private void buttonClose_Click(object sender, EventArgs e)
		{
			Close();
		}

		/// <summary>
		/// dump project database of pds
		/// </summary>
		/// <param name="host"></param>
		/// <param name="service"></param>
		/// <param name="id"></param>
		/// <param name="password"></param>
		/// <param name="AccessFilePath"></param>
		/// <returns></returns>
		public int DumpProjectDatabase()
		{
			int res = 0;
			OleDbConnection cnn;
			string connetionString = string.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source={0};", AccessFilePath);
			cnn = new OleDbConnection(connetionString);
			try
			{
				cnn.Open();
				using (framework.core.AppOracleDatabase database = new framework.core.AppOracleDatabase()
				{
					Host = this.Host,
					Service = this.Service,
					Id = this.PrjDatabaseUser,
					Password = this.PrjDatabasePassword
				}
				)
				{
					toolStripStatusLabelProgress.ForeColor = Color.Black;
					toolStripStatusLabelProgress.Text = "Dump Project Database";
					res = this.DumpProjectTable(cnn, database);
				}
				cnn.Close();

				return res;
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
			}
			return res;
		}

		/// <summary>
		/// dump project table
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.09.17</date>
		/// <param name="oleConn"></param>
		/// <param name="oraConn"></param>
		/// <returns></returns>
		private int DumpProjectTable(OleDbConnection oleConn, framework.core.AppOracleDatabase database)
		{
			int res = 0;
			try
			{
				progressBarProjectDatabase.Minimum = 0;
				progressBarProjectDatabase.Maximum = 10;
				progressBarProjectDatabase.Step = 1;
				progressBarProjectDatabase.Value = 0;

				string[] sValues = new string[13];
				///--------------------------------------------------------------------------------
				string sSql = "SELECT * FROM PDTABLE_101";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))///OracleCommand comm = new OracleCommand(sSql, oraConn.Connection))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
							sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
							sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
							sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
							sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
							sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_101 (system_unique_no,project_no,project_name,job_no,company_name,plant_name,site_id) VALUES(";
							sSql += "'" + sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "')";
							toolStripStatusLabelProgress.Text = sSql;
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				toolStripStatusLabelProgress.Text = "SELECT * FROM PDTABLE_105";
				sSql = "SELECT * FROM PDTABLE_105";
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
							sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
							sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
							sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
							sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
							sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;
							sValues[7] = (null != oDataRow[7]) ? oDataRow[7].ToString() : string.Empty;
							sValues[8] = (null != oDataRow[8]) ? oDataRow[8].ToString() : string.Empty;
							sValues[9] = (null != oDataRow[9]) ? oDataRow[9].ToString() : string.Empty;
							sValues[10] = (null != oDataRow[10]) ? oDataRow[10].ToString() : string.Empty;
							sValues[11] = (null != oDataRow[11]) ? oDataRow[11].ToString() : string.Empty;
							sValues[12] = (null != oDataRow[12]) ? oDataRow[12].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_105 (site_id,site_description,min_model_range,max_model_range,min_dwg_range,max_dwg_range,min_dwg_view_range,max_dwg_view_range,min_pip_partition,max_pip_partition,min_eqp_partition,max_eqp_partition,master_flag) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "','";
							sSql += sValues[7] + "','" + sValues[8] + "','" + sValues[9] + "','" + sValues[10] + "','" + sValues[11] + "','" + sValues[12] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT model_index_no,discipline_indx_no,partition_no,model_no,model_file_spec,path_name,network_address FROM PDTABLE_113";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
							sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
							sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
							sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
							sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
							sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_113 (model_index_no,discipline_indx_no,partition_no,model_no,model_file_spec,path_name,network_address) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT discipline_indx_no,default_path_name,default_node FROM PDTABLE_115";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
							sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_115 (discipline_indx_no,default_path_name,default_node) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT sub_project_node,sub_project_path FROM PDTABLE_116";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_116 (sub_project_node,sub_project_path) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT * FROM PDTABLE_119";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_119 (model_index_no,site_id) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT dwg_index_no,drawing_title,default_scale,drawing_size,drawing_type,drawing_file_spec,path_name,network_address FROM PDTABLE_121";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
							sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
							sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
							sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
							sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
							sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;
							sValues[7] = (null != oDataRow[7]) ? oDataRow[7].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_121 (dwg_index_no,drawing_title,default_scale,drawing_size,drawing_type,drawing_file_spec,path_name,network_address) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "','" + sValues[7] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT dwg_view_index_no,dwg_view_no,dwg_view_name,dwg_view_scale,dwg_index_no,viewing_direction,composition_status,dwg_view_x_low,dwg_view_y_low,dwg_view_z_low,dwg_view_x_high,dwg_view_y_high,dwg_view_z_high FROM PDTABLE_122";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
							sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
							sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
							sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
							sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
							sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;
							sValues[7] = (null != oDataRow[7]) ? oDataRow[7].ToString() : string.Empty;
							sValues[8] = (null != oDataRow[8]) ? oDataRow[8].ToString() : string.Empty;
							sValues[9] = (null != oDataRow[9]) ? oDataRow[9].ToString() : string.Empty;
							sValues[10] = (null != oDataRow[10]) ? oDataRow[10].ToString() : string.Empty;
							sValues[11] = (null != oDataRow[11]) ? oDataRow[11].ToString() : string.Empty;
							sValues[12] = (null != oDataRow[12]) ? oDataRow[12].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_122 (dwg_view_index_no,dwg_view_no,dwg_view_name,dwg_view_scale,dwg_index_no,viewing_direction,composition_status,dwg_view_x_low,dwg_view_y_low,dwg_view_z_low,dwg_view_x_high,dwg_view_y_high,dwg_view_z_high) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "','";
							sSql += sValues[7] + "','" + sValues[8] + "','" + sValues[9] + "','" + sValues[10] + "','" + sValues[11] + "','" + sValues[12] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT dwg_view_index_no,model_index_no FROM PDTABLE_123";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_123 (dwg_view_index_no,model_index_no) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();

				///--------------------------------------------------------------------------------
				sSql = "SELECT drawing_type,drawing_size,drawing_scale,default_path_name,default_node FROM PDTABLE_126";
				toolStripStatusLabelProgress.Text = sSql;
				using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
				{
					using (OleDbCommand olecomm = new OleDbCommand(sSql, oleConn))
					{
						foreach (DataRow oDataRow in ds.Tables[0].Rows)
						{
							sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
							sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
							sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
							sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
							sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;

							sSql = "INSERT INTO PDTABLE_126 (drawing_type,drawing_size,drawing_scale,default_path_name,default_node) VALUES('";
							sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "')";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
					}
				}
				progressBarProjectDatabase.PerformStep();
				Application.DoEvents();
			}
			catch (OracleException ex)
			{
				toolStripStatusLabelProgress.ForeColor = Color.Red;
				toolStripStatusLabelProgress.Text = ex.Message;
				res = ex.ErrorCode;
			}
			catch (Exception ex1)
			{
				MessageBox.Show(ex1.Message);
				res = -1;
			}

			return res;
		}

		/// <summary>
		/// write model folder to ini file
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2012.09.20</date>
		/// <param name="host"></param>
		/// <param name="service"></param>
		/// <param name="id"></param>
		/// <param name="password"></param>
		/// <param name="sIniFilePath"></param>
		/// <returns></returns>
		public int WriteModelFolder()
		{
			int res = 0;

			CIni Ini = new CIni();
			using (framework.core.AppOracleDatabase database = new framework.core.AppOracleDatabase()
				{
					Host = this.Host,
					Service = this.Service,
					Id = this.PrjDatabaseUser,
					Password = this.PrjDatabasePassword
				}
			)
			{
				toolStripStatusLabelProgress.Text = "Write Model Folder Information";

				string sApp = "Folder", sValue = string.Empty;
				using(DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("SELECT * FROM PDTABLE_115")))
				{
					foreach (DataRow oDataRow in ds.Tables[0].Rows)
					{
						string sPathName = oDataRow["default_path_name"].ToString();
						int at = sPathName.IndexOf('\\');
						if (at >= 0) sPathName = sPathName.Substring(at);
						string sDefaultNode = oDataRow["default_node"].ToString();
						string sFolderPath = @"\\" + sDefaultNode + sPathName;
						string sDisciplineIndexNo = oDataRow["discipline_indx_no"].ToString();

						if ("1" == sDisciplineIndexNo)
						{
							sValue = Ini.GetIniValue(sApp, "Pipe", 256, IniFilePath);
							if (string.IsNullOrEmpty(sValue))
							{
								Ini.SetIniValue(sApp, "Pipe", sFolderPath, IniFilePath);
							}
						}
						else if ("2" == sDisciplineIndexNo)
						{
							sValue = Ini.GetIniValue(sApp, "Equipment", 256, IniFilePath);
							if (string.IsNullOrEmpty(sValue))
							{
								Ini.SetIniValue(sApp, "Equipment", sFolderPath, IniFilePath);
							}
						}
						else if ("5" == sDisciplineIndexNo)
						{
							sValue = Ini.GetIniValue(sApp, "CableT", 256, IniFilePath);
							if (string.IsNullOrEmpty(sValue))
							{
								Ini.SetIniValue(sApp, "CableT", sFolderPath, IniFilePath);
							}
						}
						else if ("4" == sDisciplineIndexNo)
						{
							sValue = Ini.GetIniValue(sApp, "Misc", 256, IniFilePath);
							if (string.IsNullOrEmpty(sValue))
							{
								Ini.SetIniValue(sApp, "Misc", sFolderPath, IniFilePath);
							}
						}
					}
				}

				/// structure
				using(DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("SELECT sub_project_node,sub_project_path FROM PDTABLE_116")))
				{
					foreach (DataRow oDataRow in ds.Tables[0].Rows)
					{
						string sDefaultNode = oDataRow["sub_project_node"].ToString();
						string sProjectPath = oDataRow["sub_project_path"].ToString();
						int at = sProjectPath.IndexOf('\\');
						if (at >= 0) sProjectPath = sProjectPath.Substring(at);
						string sFolderPath = @"\\" + sDefaultNode + Path.Combine(sProjectPath, "frz");
						sValue = Ini.GetIniValue(sApp, "Structure", 256, IniFilePath);
						if (string.IsNullOrEmpty(sValue))
						{
							Ini.SetIniValue(sApp, "Structure", sFolderPath, IniFilePath);
						}
					}
				}

				/// drawing
				using(DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("SELECT default_node,default_path_name FROM PDTABLE_126")))
				{
					foreach (DataRow oDataRow in ds.Tables[0].Rows)
					{
						string sDefaultNode = oDataRow["default_node"].ToString();
						string sPathName = oDataRow["default_path_name"].ToString();
						int at = sPathName.IndexOf('\\');
						if (at >= 0) sPathName = sPathName.Substring(at);
						string sFolderPath = @"\\" + sDefaultNode + sPathName;
						sValue = Ini.GetIniValue(sApp, "Drawing", 256, IniFilePath);
						if (string.IsNullOrEmpty(sValue))
						{
							Ini.SetIniValue(sApp, "Drawing", sFolderPath, IniFilePath);
						}
					}
				}
			}

			return res;
		}

		/// <summary>
		/// copy tables before dump design database
		/// </summary>
		/// <author>humkyung</author>
		/// <param name="oleConn"></param>
		/// <returns></returns>
		private bool CreateCopyTable(framework.core.AppOleDatabase access)
		{
			bool res = true;

			PartionNoList.Clear();
			string sSql = "SELECT DISTINCT partition_no FROM PDTABLE_113 WHERE cdbl(partition_no) > 0";
			using (DataSet ds = access.ExecuteDataSet(access.GetSqlStringCommand(sSql)))
			{
				foreach (DataRow oRow in ds.Tables[0].Rows)
				{
					if (null == oRow[0]) continue;
					string sTableNo = oRow[0].ToString();
					PartionNoList.Add(sTableNo);

					sSql = "SELECT system_unique_no,line_number_label,line_id,unit_number,unit_code,train_number,fluid_code,line_sequence_no,nominal_piping_dia,piping_mater_class,insulation_purpose,insulation_thick INTO PDTABLE_12_" + sTableNo + " From PDTABLE_12_";
					using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
					{
						try
						{
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
						catch (Exception ex)
						{
							toolStripStatusLabelProgress.ForeColor = Color.Red;
							toolStripStatusLabelProgress.Text = ex.Message;
						}
						Application.DoEvents();

						try
						{
							sSql = "SELECT equip_indx_no,equip_no,equip_descr_1,equip_descr_2,tutorial_no,equip_class,dry_weight,oper_weight_1,oper_weight_2,insulation_thk,construction_stat,equipment_division,approval_status INTO PDTABLE_21_" + sTableNo + " From PDTABLE_21_";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
						catch (Exception ex)
						{
							toolStripStatusLabelProgress.ForeColor = Color.Red;
							toolStripStatusLabelProgress.Text = ex.Message;
						}
						Application.DoEvents();

						try
						{
							sSql = "SELECT nozzle_indx_no,nozzle_no,equip_indx_no,nominal_piping_dia,rating,preparation,piping_mater_class,unit_no,fluid_code,table_suffix INTO PDTABLE_22_" + sTableNo + " From PDTABLE_22_";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
						catch (Exception ex)
						{
							toolStripStatusLabelProgress.ForeColor = Color.Red;
							toolStripStatusLabelProgress.Text = ex.Message;
						}

						try
						{
							sSql = "SELECT nozzle_indx_no,equip_indx_no,parm_indx_no,parm_noz_no,nozzle_type,orientation_1,orientation_2,orientation_3,orientation_4,dimension_1,dimension_2,dimension_3,dimension_4,dimension_5,dimension_6 INTO PDTABLE_23_" + sTableNo + " From PDTABLE_23_";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
						catch (Exception ex)
						{
							toolStripStatusLabelProgress.ForeColor = Color.Red;
							toolStripStatusLabelProgress.Text = ex.Message;
						}
						Application.DoEvents();

						try
						{
							sSql = "SELECT system_unique_no,piping_comp_no,model_code,commodity_name,remarks INTO PDTABLE_34_" + sTableNo + " From PDTABLE_34_";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
						catch (Exception ex)
						{
							toolStripStatusLabelProgress.ForeColor = Color.Red;
							toolStripStatusLabelProgress.Text = ex.Message;
						}
						Application.DoEvents();

						try
						{
							sSql = "SELECT system_unique_no,instrument_comp_no,model_code INTO PDTABLE_67_" + sTableNo + " From PDTABLE_67_";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
						catch (Exception ex)
						{
							toolStripStatusLabelProgress.ForeColor = Color.Red;
							toolStripStatusLabelProgress.Text = ex.Message;
						}
						Application.DoEvents();

						/// copy table for support
						try
						{
							sSql = "SELECT system_unique_no,pipe_support_no,details_for_shop,details_for_field,commodity_code INTO PDTABLE_80_" + sTableNo + " From PDTABLE_80_";
							olecomm.CommandText = sSql;
							olecomm.ExecuteNonQuery();
						}
						catch (Exception ex)
						{
							toolStripStatusLabelProgress.ForeColor = Color.Red;
							toolStripStatusLabelProgress.Text = ex.Message;
						}
						/// up to here
						Application.DoEvents();
					}
				}
			}

			return res;
		}

		/// <summary>
		/// dump design database to access file
		/// </summary>
		/// <author>humkyung</author>
		/// <param name="host"></param>
		/// <param name="service"></param>
		/// <param name="id"></param>
		/// <param name="password"></param>
		/// <param name="AccessFilePath"></param>
		/// <returns></returns>
		private bool DumpDesignDatabase()
		{
			bool res = false;
			try
			{
				string[] sValues = new string[15];

				using(framework.core.AppOleDatabase access = new framework.core.AppOleDatabase()
					{
						AccessFilePath = this.AccessFilePath
					})
				{
					using (framework.core.AppOracleDatabase oracle = new framework.core.AppOracleDatabase()
						{
							Host = this.Host,
							Service = this.Service,
							Id = this.DesignDatabaseUser,
							Password = this.DesignDatabasePassword
						}
					)
					{
						if (false == this.CreateCopyTable(access)) return res;
						toolStripStatusLabelProgress.ForeColor = Color.Black;
						toolStripStatusLabelProgress.Text = "Dump Design Database";

						progressBarDesignDatabase.Minimum = 0;
						progressBarDesignDatabase.Maximum = PartionNoList.Count;
						progressBarDesignDatabase.Step = 1;
						progressBarDesignDatabase.Value = 0;
						foreach (string sPartNo in this.PartionNoList)
						{
							try
							{
								string sSql = "SELECT system_unique_no,line_number_label,line_id,unit_number,unit_code,train_number,fluid_code,line_sequence_no,nominal_piping_dia,piping_mater_class,insulation_purpose,insulation_thick FROM PDTABLE_12_" + sPartNo;
								using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
								{
									using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
									{
										foreach (DataRow oDataRow in ds.Tables[0].Rows)
										{
											sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
											sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
											sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
											sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
											sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
											sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
											sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;
											sValues[7] = (null != oDataRow[7]) ? oDataRow[7].ToString() : string.Empty;
											sValues[8] = (null != oDataRow[8]) ? oDataRow[8].ToString() : string.Empty;
											sValues[9] = (null != oDataRow[9]) ? oDataRow[9].ToString() : string.Empty;
											sValues[10] = (null != oDataRow[10]) ? oDataRow[10].ToString() : string.Empty;
											sValues[11] = (null != oDataRow[11]) ? oDataRow[11].ToString() : string.Empty;
											for (int i = 0; i < 12; ++i)
											{
												sValues[i] = sValues[i].Replace("'", "''");
											}

											sSql = "INSERT INTO PDTABLE_12_" + sPartNo + " (system_unique_no,line_number_label,line_id,unit_number,unit_code,train_number,fluid_code,line_sequence_no,nominal_piping_dia,piping_mater_class,insulation_purpose,insulation_thick) VALUES('";
											sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "','";
											sSql += sValues[7] + "','" + sValues[8] + "','" + sValues[9] + "','" + sValues[10] + "','" + sValues[11] + "')";
											olecomm.CommandText = sSql;
											olecomm.ExecuteNonQuery();
										}
									}
								}
							}
							catch (Exception e)
							{
								toolStripStatusLabelProgress.ForeColor = Color.Red;
								toolStripStatusLabelProgress.Text =  ("PDTABLE_12_" + sPartNo + " - ") + e.Message.Trim();
							}

							try
							{
								string sSql = "SELECT system_unique_no,piping_comp_no,model_code,commodity_name,remarks FROM PDTABLE_34_" + sPartNo;
								using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
								{
									using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
									{
										foreach (DataRow oDataRow in ds.Tables[0].Rows)
										{
											sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
											sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
											sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
											sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
											sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
											for (int i = 0; i < 5; ++i)
											{
												sValues[i] = sValues[i].Replace("'", "''");
											}

											sSql = "INSERT INTO PDTABLE_34_" + sPartNo + " (system_unique_no,piping_comp_no,model_code,commodity_name,remarks) VALUES('";
											sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "')";
											olecomm.CommandText = sSql;
											olecomm.ExecuteNonQuery();
										}
									}
								}
							}
							catch (Exception e)
							{
								toolStripStatusLabelProgress.ForeColor = Color.Red;
								toolStripStatusLabelProgress.Text = ("PDTABLE_34_" + sPartNo + " - ") + e.Message.Trim();
							}

							try
							{
								string sSql = "SELECT system_unique_no,instrument_comp_no,model_code FROM PDTABLE_67_" + sPartNo;
								using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
								{
									using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
									{
										foreach (DataRow oDataRow in ds.Tables[0].Rows)
										{
											sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
											sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
											sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
											for (int i = 0; i < 3; ++i)
											{
												sValues[i] = sValues[i].Replace("'", "''");
											}

											sSql = "INSERT INTO PDTABLE_67_" + sPartNo + " (system_unique_no,instrument_comp_no,model_code) VALUES('";
											sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "')";
											olecomm.CommandText = sSql;
											olecomm.ExecuteNonQuery();
										}
									}
								}
							}
							catch (Exception e)
							{
								toolStripStatusLabelProgress.ForeColor = Color.Red;
								toolStripStatusLabelProgress.Text = ("PDTABLE_67_" + sPartNo + " - ") + e.Message.Trim();
							}

							/// dump table for support
							try
							{
								string sSql = "SELECT system_unique_no,pipe_support_no,details_for_shop,details_for_field,commodity_code FROM PDTABLE_80_" + sPartNo;
								using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
								{
									using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
									{
										foreach (DataRow oDataRow in ds.Tables[0].Rows)
										{
											sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
											sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
											sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
											sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
											sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
											for (int i = 0; i < 5; ++i)
											{
												sValues[i] = sValues[i].Replace("'", "''");
											}

											sSql = "INSERT INTO PDTABLE_80_" + sPartNo + " (system_unique_no,pipe_support_no,details_for_shop,details_for_field,commodity_code) VALUES('";
											sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "')";
											olecomm.CommandText = sSql;
											olecomm.ExecuteNonQuery();
										}
									}
								}
							}
							catch (Exception e)
							{
								toolStripStatusLabelProgress.ForeColor = Color.Red;
								toolStripStatusLabelProgress.Text = ("PDTABLE_80_" + sPartNo + " - ") + e.Message.Trim();
							}
							/// up to here

							try
							{
								string sSql = "SELECT equip_indx_no,equip_no,equip_descr_1,equip_descr_2,tutorial_no,equip_class,dry_weight,oper_weight_1,oper_weight_2,insulation_thk,construction_stat,equipment_division,approval_status FROM PDTABLE_21_" + sPartNo;
								using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
								{
									using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
									{
										foreach (DataRow oDataRow in ds.Tables[0].Rows)
										{
											sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
											sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
											sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
											sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
											sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
											sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
											sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;
											sValues[7] = (null != oDataRow[7]) ? oDataRow[7].ToString() : string.Empty;
											sValues[8] = (null != oDataRow[8]) ? oDataRow[8].ToString() : string.Empty;
											sValues[9] = (null != oDataRow[9]) ? oDataRow[9].ToString() : string.Empty;
											sValues[10] = (null != oDataRow[10]) ? oDataRow[10].ToString() : string.Empty;
											sValues[11] = (null != oDataRow[11]) ? oDataRow[11].ToString() : string.Empty;
											sValues[12] = (null != oDataRow[12]) ? oDataRow[12].ToString() : string.Empty;
											for (int i = 0; i < 13; ++i)
											{
												sValues[i] = sValues[i].Replace("'", "''");
											}

											sSql = "INSERT INTO PDTABLE_21_" + sPartNo + " (equip_indx_no,equip_no,equip_descr_1,equip_descr_2,tutorial_no,equip_class,dry_weight,oper_weight_1,oper_weight_2,insulation_thk,construction_stat,equipment_division,approval_status) VALUES('";
											sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "','";
											sSql += sValues[7] + "','" + sValues[8] + "','" + sValues[9] + "','" + sValues[10] + "','" + sValues[11] + "','" + sValues[12] + "')";
											olecomm.CommandText = sSql;
											olecomm.ExecuteNonQuery();
										}
									}
								}
							}
							catch (Exception e)
							{
								toolStripStatusLabelProgress.ForeColor = Color.Red;
								toolStripStatusLabelProgress.Text = ("PDTABLE_21_" + sPartNo + " - ") + e.Message.Trim();
							}

							try
							{
								string sSql = "SELECT nozzle_indx_no,nozzle_no,equip_indx_no,nominal_piping_dia,rating,preparation,piping_mater_class,unit_no,fluid_code,table_suffix FROM PDTABLE_22_" + sPartNo;
								using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
								{
									using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
									{
										foreach (DataRow oDataRow in ds.Tables[0].Rows)
										{
											sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
											sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
											sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
											sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
											sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
											sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
											sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;
											sValues[7] = (null != oDataRow[7]) ? oDataRow[7].ToString() : string.Empty;
											sValues[8] = (null != oDataRow[8]) ? oDataRow[8].ToString() : string.Empty;
											sValues[9] = (null != oDataRow[9]) ? oDataRow[9].ToString() : string.Empty;
											for (int i = 0; i < 10; ++i)
											{
												sValues[i] = sValues[i].Replace("'", "''");
											}

											sSql = "INSERT INTO PDTABLE_22_" + sPartNo + " (nozzle_indx_no,nozzle_no,equip_indx_no,nominal_piping_dia,rating,preparation,piping_mater_class,unit_no,fluid_code,table_suffix) VALUES('";
											sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "','";
											sSql += sValues[7] + "','" + sValues[8] + "','" + sValues[9] + "')";
											olecomm.CommandText = sSql;
											olecomm.ExecuteNonQuery();
										}
									}
								}
							}
							catch (Exception e)
							{
								toolStripStatusLabelProgress.ForeColor = Color.Red;
								toolStripStatusLabelProgress.Text = ("PDTABLE_22_" + sPartNo + " - ") + e.Message.Trim();
							}

							try
							{
								string sSql = "SELECT nozzle_indx_no,equip_indx_no,parm_indx_no,parm_noz_no,nozzle_type,orientation_1,orientation_2,orientation_3,orientation_4,dimension_1,dimension_2,dimension_3,dimension_4,dimension_5,dimension_6 FROM PDTABLE_23_" + sPartNo;
								using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
								{
									using (OleDbCommand olecomm = new OleDbCommand(sSql, (OleDbConnection)access.Connection))
									{
										foreach (DataRow oDataRow in ds.Tables[0].Rows)
										{
											sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
											sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
											sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;
											sValues[3] = (null != oDataRow[3]) ? oDataRow[3].ToString() : string.Empty;
											sValues[4] = (null != oDataRow[4]) ? oDataRow[4].ToString() : string.Empty;
											sValues[5] = (null != oDataRow[5]) ? oDataRow[5].ToString() : string.Empty;
											sValues[6] = (null != oDataRow[6]) ? oDataRow[6].ToString() : string.Empty;
											sValues[7] = (null != oDataRow[7]) ? oDataRow[7].ToString() : string.Empty;
											sValues[8] = (null != oDataRow[8]) ? oDataRow[8].ToString() : string.Empty;
											sValues[9] = (null != oDataRow[9]) ? oDataRow[9].ToString() : string.Empty;
											sValues[10] = (null != oDataRow[10]) ? oDataRow[10].ToString() : string.Empty;
											sValues[11] = (null != oDataRow[11]) ? oDataRow[11].ToString() : string.Empty;
											sValues[12] = (null != oDataRow[8]) ? oDataRow[12].ToString() : string.Empty;
											sValues[13] = (null != oDataRow[9]) ? oDataRow[13].ToString() : string.Empty;
											sValues[14] = (null != oDataRow[10]) ? oDataRow[14].ToString() : string.Empty;
											for (int i = 0; i < 11; ++i)
											{
												sValues[i] = sValues[i].Replace("'", "''");
											}

											sSql = "INSERT INTO PDTABLE_23_" + sPartNo + " (nozzle_indx_no,equip_indx_no,parm_indx_no,parm_noz_no,nozzle_type,orientation_1,orientation_2,orientation_3,orientation_4,dimension_1,dimension_2,dimension_3,dimension_4,dimension_5,dimension_6) VALUES('";
											sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "','" + sValues[3] + "','" + sValues[4] + "','" + sValues[5] + "','" + sValues[6] + "','";
											sSql += sValues[7] + "','" + sValues[8] + "','" + sValues[9] + "','" + sValues[10] + "','" + sValues[11] + "','" + sValues[12] + "','" + sValues[13] + "','" + sValues[14] + "')";
											olecomm.CommandText = sSql;
											olecomm.ExecuteNonQuery();
										}
									}
								}
							}
							catch (Exception e)
							{
								toolStripStatusLabelProgress.ForeColor = Color.Red;
								toolStripStatusLabelProgress.Text = ("PDTABLE_23_" + sPartNo + " - ") + e.Message.Trim();
							}

							progressBarDesignDatabase.PerformStep();
							Application.DoEvents();
						}
					}
				}

				return true;
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
			return res;
		}

		private void FormExtractOracle_Load(object sender, EventArgs e)
		{
		}

		private void FormExtractOracle_Shown(object sender, EventArgs e)
		{
			CIni ini = new CIni();

			Host = ini.GetIniValue("Database", "Host", 256, IniFilePath);
			Service = ini.GetIniValue("Database", "Service", 256, IniFilePath);
			PrjDatabaseUser = ini.GetIniValue("Database", "Project_Database_UserID", 256, IniFilePath);
			PrjDatabasePassword = ini.GetIniValue("Database", "Project_Database_Password", 256, IniFilePath);
			DesignDatabaseUser = ini.GetIniValue("Database", "Design_Database_UserID", 256, IniFilePath);
			DesignDatabasePassword = ini.GetIniValue("Database", "Design_Database_Password", 256, IniFilePath);
			AccessFilePath = ini.GetIniValue("Database", "Access File", 256, IniFilePath);

			buttonClose.Enabled = false;
			try
			{
				DumpProjectDatabase();
				WriteModelFolder();
				DumpDesignDatabase();
				toolStripStatusLabelProgress.ForeColor = Color.Blue;
				toolStripStatusLabelProgress.Text = "Dump is done.";
			}
			catch (Exception ex)
			{
				toolStripStatusLabelProgress.ForeColor = Color.Red;
				toolStripStatusLabelProgress.Text = ex.Message;
			}
			buttonClose.Enabled = true;
		}
	}
}
