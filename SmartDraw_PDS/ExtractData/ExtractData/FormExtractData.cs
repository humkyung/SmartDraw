using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using framework.core;
using System.Data.OracleClient;
using System.IO;
using System.Runtime.InteropServices;

namespace ExtractData
{
    public partial class FormExtractData : Form
    {
        [DllImport("kernel32", CharSet = CharSet.Unicode)]
        static extern long WritePrivateProfileString(string Section, string Key, string Value, string FilePath);

        [DllImport("kernel32", CharSet = CharSet.Unicode)]
        static extern int GetPrivateProfileString(string Section, string Key, string Default, StringBuilder RetVal, int Size, string FilePath);

        [DllImport("kernel32", CharSet = CharSet.Unicode)]
        static extern uint GetPrivateProfileInt(string Section, string Key, int Default, string FilePath);

        [DllImport("", CharSet=CharSet.Ansi, CallingConvention=CallingConvention.StdCall)]
        static void Dgn2Sxf(string pExportFilePath , int iModelType , string pImportFilePath , string pIniFilePath , double dOffsetX , double dOffsetY , double dOffsetZ );

        private List<string> PartionNoList = new List<string>();

        public FormExtractData()
        {
            InitializeComponent();
        }

        /// <summary>
        /// close form
        /// </summary>
        /// <author>humkyung</author>
        /// <date>2017.07.25</date>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Close_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// test project database connection
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonPrjDatabaseTestConnection_Click(object sender, EventArgs e)
        {
            using(AppOracleDatabase database = new AppOracleDatabase()
            {
                Host = textBoxHost.Text,
                Service = textBoxSID.Text,
                Id = textBoxPrjDatabaseID.Text,
                Password = textBoxPrjDatabasePswd.Text
            })
            {
                try
                {
                    database.Connection.Open();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        /// <summary>
        /// test design database connection
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonDesignDatabaseTestConnection_Click(object sender, EventArgs e)
        {
            using (AppOracleDatabase database = new AppOracleDatabase()
            {
                Host = textBoxHost.Text,
                Service = textBoxSID.Text,
                Id = textBoxDesignDatabaseID.Text,
                Password = textBoxDesignDatabasePassword.Text
            })
            {
                try
                {
                    database.Connection.Open();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        /// <summary>
        /// extract neutral data from model
        /// </summary>
        /// <author>humkyung</author>
        /// <date>2017.07.26</date>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OK_Click(object sender, EventArgs e)
        {
			for (int i = 0; i < this.tabControlModel.TabPages.Count; ++i)
			{
				ListView oModelListView = this.tabControlModel.TabPages[i].Controls[0] as ListView;
				if (null != oModelListView)
				{
				}
			}
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
        public int DumpProjectDatabase(string sAccessFilePath)
        {
            int res = 0;
            using (AppOleDatabase access = new AppOleDatabase() { AccessFilePath = sAccessFilePath })
            {
                try
                {
                    using (framework.core.AppOracleDatabase database = new framework.core.AppOracleDatabase()
                    {
                        Host = this.textBoxHost.Text,
                        Service = this.textBoxSID.Text,
                        Id = this.textBoxPrjDatabaseID.Text,
                        Password = this.textBoxPrjDatabasePswd.Text
                    }
                    )
                    {
                        progressBarStatus.ForeColor = Color.Black;
                        progressBarStatus.Text = "Dump Project Database";
                        res = this.DumpProjectTable(access, database);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
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
        private int DumpProjectTable(AppOleDatabase access, framework.core.AppOracleDatabase database)
        {
            int res = 0;
            try
            {
                progressBarStatus.Minimum = 0;
                progressBarStatus.Maximum = 10;
                progressBarStatus.Step = 1;
                progressBarStatus.Value = 0;

                string[] sValues = new string[13];
                ///--------------------------------------------------------------------------------
                string sSql = "SELECT * FROM PDTABLE_101";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
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
                        progressBarStatus.Text = sSql;

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                progressBarStatus.Text = "SELECT * FROM PDTABLE_105";
                sSql = "SELECT * FROM PDTABLE_105";
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
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

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT model_index_no,discipline_indx_no,partition_no,model_no,model_file_spec,path_name,network_address FROM PDTABLE_113";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
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

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT discipline_indx_no,default_path_name,default_node FROM PDTABLE_115";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
                {
                    foreach (DataRow oDataRow in ds.Tables[0].Rows)
                    {
                        sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
                        sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;
                        sValues[2] = (null != oDataRow[2]) ? oDataRow[2].ToString() : string.Empty;

                        sSql = "INSERT INTO PDTABLE_115 (discipline_indx_no,default_path_name,default_node) VALUES('";
                        sSql += sValues[0] + "','" + sValues[1] + "','" + sValues[2] + "')";

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT sub_project_node,sub_project_path FROM PDTABLE_116";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
                {
                    foreach (DataRow oDataRow in ds.Tables[0].Rows)
                    {
                        sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
                        sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;

                        sSql = "INSERT INTO PDTABLE_116 (sub_project_node,sub_project_path) VALUES('";
                        sSql += sValues[0] + "','" + sValues[1] + "')";

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT * FROM PDTABLE_119";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
                {
                    foreach (DataRow oDataRow in ds.Tables[0].Rows)
                    {
                        sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
                        sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;

                        sSql = "INSERT INTO PDTABLE_119 (model_index_no,site_id) VALUES('";
                        sSql += sValues[0] + "','" + sValues[1] + "')";

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT dwg_index_no,drawing_title,default_scale,drawing_size,drawing_type,drawing_file_spec,path_name,network_address FROM PDTABLE_121";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
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

                        access.ExecuteNonQuery(access.GetStoredProcedureCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT dwg_view_index_no,dwg_view_no,dwg_view_name,dwg_view_scale,dwg_index_no,viewing_direction,composition_status,dwg_view_x_low,dwg_view_y_low,dwg_view_z_low,dwg_view_x_high,dwg_view_y_high,dwg_view_z_high FROM PDTABLE_122";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
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

                        access.ExecuteNonQuery(access.GetStoredProcedureCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT dwg_view_index_no,model_index_no FROM PDTABLE_123";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
                {
                    foreach (DataRow oDataRow in ds.Tables[0].Rows)
                    {
                        sValues[0] = (null != oDataRow[0]) ? oDataRow[0].ToString() : string.Empty;
                        sValues[1] = (null != oDataRow[1]) ? oDataRow[1].ToString() : string.Empty;

                        sSql = "INSERT INTO PDTABLE_123 (dwg_view_index_no,model_index_no) VALUES('";
                        sSql += sValues[0] + "','" + sValues[1] + "')";

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();

                ///--------------------------------------------------------------------------------
                sSql = "SELECT drawing_type,drawing_size,drawing_scale,default_path_name,default_node FROM PDTABLE_126";
                progressBarStatus.Text = sSql;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand(sSql)))
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

                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                }
                progressBarStatus.PerformStep();
                Application.DoEvents();
            }
            catch (OracleException ex)
            {
                progressBarStatus.ForeColor = Color.Red;
                progressBarStatus.Text = ex.Message;
                res = ex.ErrorCode;
            }
            catch (Exception ex1)
            {
                MessageBox.Show(ex1.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
        public int WriteModelFolder(string sIniFilePath)
        {
            int res = 0;

            using (framework.core.AppOracleDatabase database = new framework.core.AppOracleDatabase()
            {
                Host = this.textBoxHost.Text,
                Service = this.textBoxSID.Text,
                Id = this.textBoxPrjDatabaseID.Text,
                Password = this.textBoxPrjDatabasePswd.Text
            }
            )
            {
                progressBarStatus.Text = "Write Model Folder Information";
                StringBuilder oStrBuilder = new StringBuilder(256);

                string sApp = "Folder", sValue = string.Empty;
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("SELECT * FROM PDTABLE_115")))
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
                            GetPrivateProfileString(sApp, "Pipe", string.Empty, oStrBuilder, 256, sIniFilePath);
                            sValue = oStrBuilder.ToString();
                            if (string.IsNullOrEmpty(sValue))
                            {
                                WritePrivateProfileString(sApp, "Pipe", sFolderPath, sIniFilePath);
                            }
                        }
                        else if ("2" == sDisciplineIndexNo)
                        {
                            GetPrivateProfileString(sApp, "Equipment", string.Empty, oStrBuilder, 256, sIniFilePath);
                            sValue = oStrBuilder.ToString();
                            if (string.IsNullOrEmpty(sValue))
                            {
                                WritePrivateProfileString(sApp, "Equipment", sFolderPath, sIniFilePath);
                            }
                        }
                        else if ("5" == sDisciplineIndexNo)
                        {
                            GetPrivateProfileString(sApp, "CableT", string.Empty, oStrBuilder, 256, sIniFilePath);
                            sValue = oStrBuilder.ToString();
                            if (string.IsNullOrEmpty(sValue))
                            {
                                WritePrivateProfileString(sApp, "CableT", sFolderPath, sIniFilePath);
                            }
                        }
                        else if ("4" == sDisciplineIndexNo)
                        {
                            GetPrivateProfileString(sApp, "Misc", string.Empty, oStrBuilder, 256, sIniFilePath);
                            sValue = oStrBuilder.ToString();
                            if (string.IsNullOrEmpty(sValue))
                            {
                                WritePrivateProfileString(sApp, "Misc", sFolderPath, sIniFilePath);
                            }
                        }
                    }
                }

                /// structure
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("SELECT sub_project_node,sub_project_path FROM PDTABLE_116")))
                {
                    foreach (DataRow oDataRow in ds.Tables[0].Rows)
                    {
                        string sDefaultNode = oDataRow["sub_project_node"].ToString();
                        string sProjectPath = oDataRow["sub_project_path"].ToString();
                        int at = sProjectPath.IndexOf('\\');
                        if (at >= 0) sProjectPath = sProjectPath.Substring(at);
                        string sFolderPath = @"\\" + sDefaultNode + Path.Combine(sProjectPath, "frz");
                        GetPrivateProfileString(sApp, "Structure", string.Empty, oStrBuilder, 256, sIniFilePath);
                        sValue = oStrBuilder.ToString();
                        if (string.IsNullOrEmpty(sValue))
                        {
                            WritePrivateProfileString(sApp, "Structure", sFolderPath, sIniFilePath);
                        }
                    }
                }

                /// drawing
                using (DataSet ds = database.ExecuteDataSet(database.GetSqlStringCommand("SELECT default_node,default_path_name FROM PDTABLE_126")))
                {
                    foreach (DataRow oDataRow in ds.Tables[0].Rows)
                    {
                        string sDefaultNode = oDataRow["default_node"].ToString();
                        string sPathName = oDataRow["default_path_name"].ToString();
                        int at = sPathName.IndexOf('\\');
                        if (at >= 0) sPathName = sPathName.Substring(at);
                        string sFolderPath = @"\\" + sDefaultNode + sPathName;
                        GetPrivateProfileString(sApp, "Drawing", string.Empty, oStrBuilder, 256, sIniFilePath);
                        sValue = oStrBuilder.ToString();
                        if (string.IsNullOrEmpty(sValue))
                        {
                            WritePrivateProfileString(sApp, "Drawing", sFolderPath, sIniFilePath);
                        }
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
        private bool DumpDesignDatabase(string sAccessFilePath)
        {
            bool res = false;
            try
            {
                string[] sValues = new string[15];

                using (framework.core.AppOleDatabase access = new framework.core.AppOleDatabase()
                {
                    AccessFilePath = sAccessFilePath
                })
                {
                    using (framework.core.AppOracleDatabase oracle = new framework.core.AppOracleDatabase()
                    {
                        Host = this.textBoxHost.Text,
                        Service = this.textBoxSID.Text,
                        Id = this.textBoxDesignDatabaseID.Text,
                        Password = this.textBoxDesignDatabasePassword.Text
                    }
                    )
                    {
                        if (false == this.CreateCopyTable(access)) return res;
                        progressBarStatus.ForeColor = Color.Black;
                        progressBarStatus.Text = "Dump Design Database";

                        progressBarStatus.Minimum = 0;
                        progressBarStatus.Maximum = PartionNoList.Count;
                        progressBarStatus.Step = 1;
                        progressBarStatus.Value = 0;
                        foreach (string sPartNo in this.PartionNoList)
                        {
                            try
                            {
                                string sSql = "SELECT system_unique_no,line_number_label,line_id,unit_number,unit_code,train_number,fluid_code,line_sequence_no,nominal_piping_dia,piping_mater_class,insulation_purpose,insulation_thick FROM PDTABLE_12_" + sPartNo;
                                using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
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

                                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                progressBarStatus.ForeColor = Color.Red;
                                progressBarStatus.Text = ("PDTABLE_12_" + sPartNo + " - ") + e.Message.Trim();
                            }

                            try
                            {
                                string sSql = "SELECT system_unique_no,piping_comp_no,model_code,commodity_name,remarks FROM PDTABLE_34_" + sPartNo;
                                using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
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

                                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                progressBarStatus.ForeColor = Color.Red;
                                progressBarStatus.Text = ("PDTABLE_34_" + sPartNo + " - ") + e.Message.Trim();
                            }

                            try
                            {
                                string sSql = "SELECT system_unique_no,instrument_comp_no,model_code FROM PDTABLE_67_" + sPartNo;
                                using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
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

                                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                progressBarStatus.ForeColor = Color.Red;
                                progressBarStatus.Text = ("PDTABLE_67_" + sPartNo + " - ") + e.Message.Trim();
                            }

                            /// dump table for support
                            try
                            {
                                string sSql = "SELECT system_unique_no,pipe_support_no,details_for_shop,details_for_field,commodity_code FROM PDTABLE_80_" + sPartNo;
                                using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
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

                                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                progressBarStatus.ForeColor = Color.Red;
                                progressBarStatus.Text = ("PDTABLE_80_" + sPartNo + " - ") + e.Message.Trim();
                            }
                            /// up to here

                            try
                            {
                                string sSql = "SELECT equip_indx_no,equip_no,equip_descr_1,equip_descr_2,tutorial_no,equip_class,dry_weight,oper_weight_1,oper_weight_2,insulation_thk,construction_stat,equipment_division,approval_status FROM PDTABLE_21_" + sPartNo;
                                using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
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

                                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                progressBarStatus.ForeColor = Color.Red;
                                progressBarStatus.Text = ("PDTABLE_21_" + sPartNo + " - ") + e.Message.Trim();
                            }

                            try
                            {
                                string sSql = "SELECT nozzle_indx_no,nozzle_no,equip_indx_no,nominal_piping_dia,rating,preparation,piping_mater_class,unit_no,fluid_code,table_suffix FROM PDTABLE_22_" + sPartNo;
                                using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
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

                                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                progressBarStatus.ForeColor = Color.Red;
                                progressBarStatus.Text = ("PDTABLE_22_" + sPartNo + " - ") + e.Message.Trim();
                            }

                            try
                            {
                                string sSql = "SELECT nozzle_indx_no,equip_indx_no,parm_indx_no,parm_noz_no,nozzle_type,orientation_1,orientation_2,orientation_3,orientation_4,dimension_1,dimension_2,dimension_3,dimension_4,dimension_5,dimension_6 FROM PDTABLE_23_" + sPartNo;
                                using (DataSet ds = oracle.ExecuteDataSet(oracle.GetSqlStringCommand(sSql)))
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

                                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                progressBarStatus.ForeColor = Color.Red;
                                progressBarStatus.Text = ("PDTABLE_23_" + sPartNo + " - ") + e.Message.Trim();
                            }

                            progressBarStatus.PerformStep();
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
                    try
                    {
                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                    catch (Exception ex)
                    {
                        progressBarStatus.ForeColor = Color.Red;
                        progressBarStatus.Text = ex.Message;
                    }
                    Application.DoEvents();

                    try
                    {
                        sSql = "SELECT equip_indx_no,equip_no,equip_descr_1,equip_descr_2,tutorial_no,equip_class,dry_weight,oper_weight_1,oper_weight_2,insulation_thk,construction_stat,equipment_division,approval_status INTO PDTABLE_21_" + sTableNo + " From PDTABLE_21_";
                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                    catch (Exception ex)
                    {
                        progressBarStatus.ForeColor = Color.Red;
                        progressBarStatus.Text = ex.Message;
                    }
                    Application.DoEvents();

                    try
                    {
                        sSql = "SELECT nozzle_indx_no,nozzle_no,equip_indx_no,nominal_piping_dia,rating,preparation,piping_mater_class,unit_no,fluid_code,table_suffix INTO PDTABLE_22_" + sTableNo + " From PDTABLE_22_";
                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                    catch (Exception ex)
                    {
                        progressBarStatus.ForeColor = Color.Red;
                        progressBarStatus.Text = ex.Message;
                    }

                    try
                    {
                        sSql = "SELECT nozzle_indx_no,equip_indx_no,parm_indx_no,parm_noz_no,nozzle_type,orientation_1,orientation_2,orientation_3,orientation_4,dimension_1,dimension_2,dimension_3,dimension_4,dimension_5,dimension_6 INTO PDTABLE_23_" + sTableNo + " From PDTABLE_23_";
                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                    catch (Exception ex)
                    {
                        progressBarStatus.ForeColor = Color.Red;
                        progressBarStatus.Text = ex.Message;
                    }
                    Application.DoEvents();

                    try
                    {
                        sSql = "SELECT system_unique_no,piping_comp_no,model_code,commodity_name,remarks INTO PDTABLE_34_" + sTableNo + " From PDTABLE_34_";
                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                    catch (Exception ex)
                    {
                        progressBarStatus.ForeColor = Color.Red;
                        progressBarStatus.Text = ex.Message;
                    }
                    Application.DoEvents();

                    try
                    {
                        sSql = "SELECT system_unique_no,instrument_comp_no,model_code INTO PDTABLE_67_" + sTableNo + " From PDTABLE_67_";
                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                    catch (Exception ex)
                    {
                        progressBarStatus.ForeColor = Color.Red;
                        progressBarStatus.Text = ex.Message;
                    }
                    Application.DoEvents();

                    /// copy table for support
                    try
                    {
                        sSql = "SELECT system_unique_no,pipe_support_no,details_for_shop,details_for_field,commodity_code INTO PDTABLE_80_" + sTableNo + " From PDTABLE_80_";
                        access.ExecuteNonQuery(access.GetSqlStringCommand(sSql));
                    }
                    catch (Exception ex)
                    {
                        progressBarStatus.ForeColor = Color.Red;
                        progressBarStatus.Text = ex.Message;
                    }
                    /// up to here
                    Application.DoEvents();
                }
            }

            return res;
        }

		/// <summary>
		/// dump database
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2017.07.26</date>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void buttonDump_Click(object sender, EventArgs e)
		{
			FolderBrowserDialog dlg = new FolderBrowserDialog();
			if (DialogResult.OK != dlg.ShowDialog()) return;
			string sDestFolder = dlg.SelectedPath;
			string sPrjName = sDestFolder.Split(Path.DirectorySeparatorChar).Last();

			Close.Enabled = false;
			try
			{
				#region create folders
				StringBuilder oStrBuilder = new StringBuilder(256);

				string sPrjSettingFilePath = Path.Combine(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Setting"), "SmartDraw_Project.ini");
				uint uiFolderCount = GetPrivateProfileInt("Project_Folder", "Folder_Count", 0, sPrjSettingFilePath);
				for (int i = 0; i < uiFolderCount; ++i)
				{
					GetPrivateProfileString("Project_Folder", string.Format("Folder{0}", i), string.Empty, oStrBuilder, 256, sPrjSettingFilePath);
					if (!string.IsNullOrEmpty(oStrBuilder.ToString()))
					{
						Directory.CreateDirectory(Path.Combine(sDestFolder, oStrBuilder.ToString()));
					}
				}
				#endregion

				#region copy access file
				string sSrcAccessFilePath = Path.Combine(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Backup"), "PDS.mdb");
				string sAccessFilePath = Path.Combine(Path.Combine(sDestFolder, "Database"), string.Format("{0}.mdb", sPrjName));
				File.Copy(sSrcAccessFilePath, sAccessFilePath, true);
				#endregion

				string sIniFilePath = Path.Combine(Path.Combine(sDestFolder, "Setting"), string.Format("{0}.ini", sPrjName));

				DumpProjectDatabase(sAccessFilePath);
				WriteModelFolder(sIniFilePath);
				DumpDesignDatabase(sAccessFilePath);

				progressBarStatus.ForeColor = Color.Blue;
				progressBarStatus.Text = "Extracting Data is done.";
			}
			catch (Exception ex)
			{
				progressBarStatus.ForeColor = Color.Red;
				progressBarStatus.Text = ex.Message;
			}
			finally
			{
				Close.Enabled = true;
			}
		}

        /// <summary>
        /// extract neutral file from model
        /// </summary>
        /// <author>humkyung</author>
        private void ExtractDataFromModel()
        {
            ///Dgn2Sxf();
        }
    }
}