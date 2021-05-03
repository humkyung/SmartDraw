namespace ExtractData
{
    partial class FormExtractData
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.progressBarDump = new System.Windows.Forms.ProgressBar();
			this.buttonDump = new System.Windows.Forms.Button();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.buttonDesignDatabaseTestConnection = new System.Windows.Forms.Button();
			this.textBoxDesignDatabasePassword = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.textBoxDesignDatabaseID = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.buttonPrjDatabaseTestConnection = new System.Windows.Forms.Button();
			this.textBoxPrjDatabasePswd = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.textBoxPrjDatabaseID = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.textBoxSID = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.textBoxHost = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.tabControlModel = new System.Windows.Forms.TabControl();
			this.tabPageDrawing = new System.Windows.Forms.TabPage();
			this.listViewDrawingModel = new System.Windows.Forms.ListView();
			this.DrawingFileName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tabPagePipe = new System.Windows.Forms.TabPage();
			this.listViewPipeModel = new System.Windows.Forms.ListView();
			this.columnHeaderPipeModel = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tabPageEquipment = new System.Windows.Forms.TabPage();
			this.listViewEquipmentModel = new System.Windows.Forms.ListView();
			this.columnHeaderEquipmentModel = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tabPageStructure = new System.Windows.Forms.TabPage();
			this.listViewStructureModel = new System.Windows.Forms.ListView();
			this.columnHeaderStructureModel = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tabPageCableTray = new System.Windows.Forms.TabPage();
			this.listViewCableTModel = new System.Windows.Forms.ListView();
			this.columnHeaderCableTModel = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tabPageMisc = new System.Windows.Forms.TabPage();
			this.listViewMiscModel = new System.Windows.Forms.ListView();
			this.columnHeaderMiscModel = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.OK = new System.Windows.Forms.Button();
			this.Close = new System.Windows.Forms.Button();
			this.progressBarStatus = new System.Windows.Forms.ProgressBar();
			this.groupBox1.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.tabControlModel.SuspendLayout();
			this.tabPageDrawing.SuspendLayout();
			this.tabPagePipe.SuspendLayout();
			this.tabPageEquipment.SuspendLayout();
			this.tabPageStructure.SuspendLayout();
			this.tabPageCableTray.SuspendLayout();
			this.tabPageMisc.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this.progressBarDump);
			this.groupBox1.Controls.Add(this.buttonDump);
			this.groupBox1.Controls.Add(this.groupBox3);
			this.groupBox1.Controls.Add(this.groupBox2);
			this.groupBox1.Controls.Add(this.textBoxSID);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.textBoxHost);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(689, 251);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Oracle Setting";
			// 
			// progressBarDump
			// 
			this.progressBarDump.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.progressBarDump.Location = new System.Drawing.Point(9, 218);
			this.progressBarDump.Name = "progressBarDump";
			this.progressBarDump.Size = new System.Drawing.Size(574, 23);
			this.progressBarDump.TabIndex = 13;
			// 
			// buttonDump
			// 
			this.buttonDump.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonDump.Location = new System.Drawing.Point(589, 215);
			this.buttonDump.Name = "buttonDump";
			this.buttonDump.Size = new System.Drawing.Size(90, 30);
			this.buttonDump.TabIndex = 12;
			this.buttonDump.Text = "Dump";
			this.buttonDump.UseVisualStyleBackColor = true;
			this.buttonDump.Click += new System.EventHandler(this.buttonDump_Click);
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.buttonDesignDatabaseTestConnection);
			this.groupBox3.Controls.Add(this.textBoxDesignDatabasePassword);
			this.groupBox3.Controls.Add(this.label5);
			this.groupBox3.Controls.Add(this.textBoxDesignDatabaseID);
			this.groupBox3.Controls.Add(this.label6);
			this.groupBox3.Location = new System.Drawing.Point(347, 84);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(332, 124);
			this.groupBox3.TabIndex = 5;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Design Database";
			// 
			// buttonDesignDatabaseTestConnection
			// 
			this.buttonDesignDatabaseTestConnection.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonDesignDatabaseTestConnection.Location = new System.Drawing.Point(210, 88);
			this.buttonDesignDatabaseTestConnection.Name = "buttonDesignDatabaseTestConnection";
			this.buttonDesignDatabaseTestConnection.Size = new System.Drawing.Size(116, 30);
			this.buttonDesignDatabaseTestConnection.TabIndex = 8;
			this.buttonDesignDatabaseTestConnection.Text = "Test Connection";
			this.buttonDesignDatabaseTestConnection.UseVisualStyleBackColor = true;
			this.buttonDesignDatabaseTestConnection.Click += new System.EventHandler(this.buttonDesignDatabaseTestConnection_Click);
			// 
			// textBoxDesignDatabasePassword
			// 
			this.textBoxDesignDatabasePassword.Location = new System.Drawing.Point(82, 52);
			this.textBoxDesignDatabasePassword.Name = "textBoxDesignDatabasePassword";
			this.textBoxDesignDatabasePassword.Size = new System.Drawing.Size(244, 22);
			this.textBoxDesignDatabasePassword.TabIndex = 7;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(6, 55);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(70, 14);
			this.label5.TabIndex = 6;
			this.label5.Text = "Password : ";
			// 
			// textBoxDesignDatabaseID
			// 
			this.textBoxDesignDatabaseID.Location = new System.Drawing.Point(82, 24);
			this.textBoxDesignDatabaseID.Name = "textBoxDesignDatabaseID";
			this.textBoxDesignDatabaseID.Size = new System.Drawing.Size(244, 22);
			this.textBoxDesignDatabaseID.TabIndex = 5;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(6, 27);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(31, 14);
			this.label6.TabIndex = 1;
			this.label6.Text = "ID : ";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.buttonPrjDatabaseTestConnection);
			this.groupBox2.Controls.Add(this.textBoxPrjDatabasePswd);
			this.groupBox2.Controls.Add(this.label4);
			this.groupBox2.Controls.Add(this.textBoxPrjDatabaseID);
			this.groupBox2.Controls.Add(this.label3);
			this.groupBox2.Location = new System.Drawing.Point(9, 84);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(332, 124);
			this.groupBox2.TabIndex = 4;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Project Database";
			// 
			// buttonPrjDatabaseTestConnection
			// 
			this.buttonPrjDatabaseTestConnection.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonPrjDatabaseTestConnection.Location = new System.Drawing.Point(210, 88);
			this.buttonPrjDatabaseTestConnection.Name = "buttonPrjDatabaseTestConnection";
			this.buttonPrjDatabaseTestConnection.Size = new System.Drawing.Size(116, 30);
			this.buttonPrjDatabaseTestConnection.TabIndex = 8;
			this.buttonPrjDatabaseTestConnection.Text = "Test Connection";
			this.buttonPrjDatabaseTestConnection.UseVisualStyleBackColor = true;
			this.buttonPrjDatabaseTestConnection.Click += new System.EventHandler(this.buttonPrjDatabaseTestConnection_Click);
			// 
			// textBoxPrjDatabasePswd
			// 
			this.textBoxPrjDatabasePswd.Location = new System.Drawing.Point(82, 52);
			this.textBoxPrjDatabasePswd.Name = "textBoxPrjDatabasePswd";
			this.textBoxPrjDatabasePswd.Size = new System.Drawing.Size(244, 22);
			this.textBoxPrjDatabasePswd.TabIndex = 7;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(6, 55);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(70, 14);
			this.label4.TabIndex = 6;
			this.label4.Text = "Password : ";
			// 
			// textBoxPrjDatabaseID
			// 
			this.textBoxPrjDatabaseID.Location = new System.Drawing.Point(82, 24);
			this.textBoxPrjDatabaseID.Name = "textBoxPrjDatabaseID";
			this.textBoxPrjDatabaseID.Size = new System.Drawing.Size(244, 22);
			this.textBoxPrjDatabaseID.TabIndex = 5;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(6, 27);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(31, 14);
			this.label3.TabIndex = 1;
			this.label3.Text = "ID : ";
			// 
			// textBoxSID
			// 
			this.textBoxSID.Location = new System.Drawing.Point(56, 56);
			this.textBoxSID.Name = "textBoxSID";
			this.textBoxSID.Size = new System.Drawing.Size(285, 22);
			this.textBoxSID.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(6, 59);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(38, 14);
			this.label2.TabIndex = 2;
			this.label2.Text = "SID : ";
			// 
			// textBoxHost
			// 
			this.textBoxHost.Location = new System.Drawing.Point(56, 28);
			this.textBoxHost.Name = "textBoxHost";
			this.textBoxHost.Size = new System.Drawing.Size(285, 22);
			this.textBoxHost.TabIndex = 1;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(6, 31);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(44, 14);
			this.label1.TabIndex = 0;
			this.label1.Text = "Host : ";
			// 
			// tabControlModel
			// 
			this.tabControlModel.Controls.Add(this.tabPageDrawing);
			this.tabControlModel.Controls.Add(this.tabPagePipe);
			this.tabControlModel.Controls.Add(this.tabPageEquipment);
			this.tabControlModel.Controls.Add(this.tabPageStructure);
			this.tabControlModel.Controls.Add(this.tabPageCableTray);
			this.tabControlModel.Controls.Add(this.tabPageMisc);
			this.tabControlModel.Location = new System.Drawing.Point(12, 269);
			this.tabControlModel.Name = "tabControlModel";
			this.tabControlModel.SelectedIndex = 0;
			this.tabControlModel.Size = new System.Drawing.Size(689, 261);
			this.tabControlModel.TabIndex = 1;
			// 
			// tabPageDrawing
			// 
			this.tabPageDrawing.Controls.Add(this.listViewDrawingModel);
			this.tabPageDrawing.Location = new System.Drawing.Point(4, 23);
			this.tabPageDrawing.Name = "tabPageDrawing";
			this.tabPageDrawing.Size = new System.Drawing.Size(681, 234);
			this.tabPageDrawing.TabIndex = 5;
			this.tabPageDrawing.Text = "Drawing";
			this.tabPageDrawing.UseVisualStyleBackColor = true;
			// 
			// listViewDrawingModel
			// 
			this.listViewDrawingModel.CheckBoxes = true;
			this.listViewDrawingModel.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.DrawingFileName});
			this.listViewDrawingModel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listViewDrawingModel.Location = new System.Drawing.Point(0, 0);
			this.listViewDrawingModel.Name = "listViewDrawingModel";
			this.listViewDrawingModel.Size = new System.Drawing.Size(681, 234);
			this.listViewDrawingModel.TabIndex = 0;
			this.listViewDrawingModel.UseCompatibleStateImageBehavior = false;
			this.listViewDrawingModel.View = System.Windows.Forms.View.Details;
			// 
			// DrawingFileName
			// 
			this.DrawingFileName.Text = "Name";
			this.DrawingFileName.Width = 350;
			// 
			// tabPagePipe
			// 
			this.tabPagePipe.Controls.Add(this.listViewPipeModel);
			this.tabPagePipe.Location = new System.Drawing.Point(4, 23);
			this.tabPagePipe.Name = "tabPagePipe";
			this.tabPagePipe.Padding = new System.Windows.Forms.Padding(3);
			this.tabPagePipe.Size = new System.Drawing.Size(681, 234);
			this.tabPagePipe.TabIndex = 0;
			this.tabPagePipe.Text = "Pipe";
			this.tabPagePipe.UseVisualStyleBackColor = true;
			// 
			// listViewPipeModel
			// 
			this.listViewPipeModel.CheckBoxes = true;
			this.listViewPipeModel.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderPipeModel});
			this.listViewPipeModel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listViewPipeModel.Location = new System.Drawing.Point(3, 3);
			this.listViewPipeModel.Name = "listViewPipeModel";
			this.listViewPipeModel.Size = new System.Drawing.Size(675, 228);
			this.listViewPipeModel.TabIndex = 1;
			this.listViewPipeModel.UseCompatibleStateImageBehavior = false;
			this.listViewPipeModel.View = System.Windows.Forms.View.Details;
			// 
			// columnHeaderPipeModel
			// 
			this.columnHeaderPipeModel.Text = "Name";
			this.columnHeaderPipeModel.Width = 350;
			// 
			// tabPageEquipment
			// 
			this.tabPageEquipment.Controls.Add(this.listViewEquipmentModel);
			this.tabPageEquipment.Location = new System.Drawing.Point(4, 23);
			this.tabPageEquipment.Name = "tabPageEquipment";
			this.tabPageEquipment.Padding = new System.Windows.Forms.Padding(3);
			this.tabPageEquipment.Size = new System.Drawing.Size(681, 234);
			this.tabPageEquipment.TabIndex = 1;
			this.tabPageEquipment.Text = "Equipment";
			this.tabPageEquipment.UseVisualStyleBackColor = true;
			// 
			// listViewEquipmentModel
			// 
			this.listViewEquipmentModel.CheckBoxes = true;
			this.listViewEquipmentModel.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderEquipmentModel});
			this.listViewEquipmentModel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listViewEquipmentModel.Location = new System.Drawing.Point(3, 3);
			this.listViewEquipmentModel.Name = "listViewEquipmentModel";
			this.listViewEquipmentModel.Size = new System.Drawing.Size(675, 228);
			this.listViewEquipmentModel.TabIndex = 2;
			this.listViewEquipmentModel.UseCompatibleStateImageBehavior = false;
			this.listViewEquipmentModel.View = System.Windows.Forms.View.Details;
			// 
			// columnHeaderEquipmentModel
			// 
			this.columnHeaderEquipmentModel.Text = "Name";
			this.columnHeaderEquipmentModel.Width = 350;
			// 
			// tabPageStructure
			// 
			this.tabPageStructure.Controls.Add(this.listViewStructureModel);
			this.tabPageStructure.Location = new System.Drawing.Point(4, 23);
			this.tabPageStructure.Name = "tabPageStructure";
			this.tabPageStructure.Size = new System.Drawing.Size(681, 234);
			this.tabPageStructure.TabIndex = 2;
			this.tabPageStructure.Text = "Structure";
			this.tabPageStructure.UseVisualStyleBackColor = true;
			// 
			// listViewStructureModel
			// 
			this.listViewStructureModel.CheckBoxes = true;
			this.listViewStructureModel.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderStructureModel});
			this.listViewStructureModel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listViewStructureModel.Location = new System.Drawing.Point(0, 0);
			this.listViewStructureModel.Name = "listViewStructureModel";
			this.listViewStructureModel.Size = new System.Drawing.Size(681, 234);
			this.listViewStructureModel.TabIndex = 3;
			this.listViewStructureModel.UseCompatibleStateImageBehavior = false;
			this.listViewStructureModel.View = System.Windows.Forms.View.Details;
			// 
			// columnHeaderStructureModel
			// 
			this.columnHeaderStructureModel.Text = "Name";
			this.columnHeaderStructureModel.Width = 350;
			// 
			// tabPageCableTray
			// 
			this.tabPageCableTray.Controls.Add(this.listViewCableTModel);
			this.tabPageCableTray.Location = new System.Drawing.Point(4, 23);
			this.tabPageCableTray.Name = "tabPageCableTray";
			this.tabPageCableTray.Size = new System.Drawing.Size(681, 234);
			this.tabPageCableTray.TabIndex = 3;
			this.tabPageCableTray.Text = "CableT";
			this.tabPageCableTray.UseVisualStyleBackColor = true;
			// 
			// listViewCableTModel
			// 
			this.listViewCableTModel.CheckBoxes = true;
			this.listViewCableTModel.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderCableTModel});
			this.listViewCableTModel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listViewCableTModel.Location = new System.Drawing.Point(0, 0);
			this.listViewCableTModel.Name = "listViewCableTModel";
			this.listViewCableTModel.Size = new System.Drawing.Size(681, 234);
			this.listViewCableTModel.TabIndex = 4;
			this.listViewCableTModel.UseCompatibleStateImageBehavior = false;
			this.listViewCableTModel.View = System.Windows.Forms.View.Details;
			// 
			// columnHeaderCableTModel
			// 
			this.columnHeaderCableTModel.Text = "Name";
			this.columnHeaderCableTModel.Width = 350;
			// 
			// tabPageMisc
			// 
			this.tabPageMisc.Controls.Add(this.listViewMiscModel);
			this.tabPageMisc.Location = new System.Drawing.Point(4, 23);
			this.tabPageMisc.Name = "tabPageMisc";
			this.tabPageMisc.Size = new System.Drawing.Size(681, 234);
			this.tabPageMisc.TabIndex = 4;
			this.tabPageMisc.Text = "Misc";
			this.tabPageMisc.UseVisualStyleBackColor = true;
			// 
			// listViewMiscModel
			// 
			this.listViewMiscModel.CheckBoxes = true;
			this.listViewMiscModel.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderMiscModel});
			this.listViewMiscModel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listViewMiscModel.Location = new System.Drawing.Point(0, 0);
			this.listViewMiscModel.Name = "listViewMiscModel";
			this.listViewMiscModel.Size = new System.Drawing.Size(681, 234);
			this.listViewMiscModel.TabIndex = 5;
			this.listViewMiscModel.UseCompatibleStateImageBehavior = false;
			this.listViewMiscModel.View = System.Windows.Forms.View.Details;
			// 
			// columnHeaderMiscModel
			// 
			this.columnHeaderMiscModel.Text = "Name";
			this.columnHeaderMiscModel.Width = 350;
			// 
			// OK
			// 
			this.OK.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.OK.Location = new System.Drawing.Point(515, 536);
			this.OK.Name = "OK";
			this.OK.Size = new System.Drawing.Size(90, 30);
			this.OK.TabIndex = 9;
			this.OK.Text = "OK";
			this.OK.UseVisualStyleBackColor = true;
			this.OK.Click += new System.EventHandler(this.OK_Click);
			// 
			// Close
			// 
			this.Close.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.Close.Location = new System.Drawing.Point(611, 536);
			this.Close.Name = "Close";
			this.Close.Size = new System.Drawing.Size(90, 30);
			this.Close.TabIndex = 10;
			this.Close.Text = "Close";
			this.Close.UseVisualStyleBackColor = true;
			this.Close.Click += new System.EventHandler(this.Close_Click);
			// 
			// progressBarStatus
			// 
			this.progressBarStatus.Location = new System.Drawing.Point(12, 539);
			this.progressBarStatus.Name = "progressBarStatus";
			this.progressBarStatus.Size = new System.Drawing.Size(497, 23);
			this.progressBarStatus.TabIndex = 11;
			// 
			// FormExtractData
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 14F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(707, 574);
			this.Controls.Add(this.progressBarStatus);
			this.Controls.Add(this.Close);
			this.Controls.Add(this.OK);
			this.Controls.Add(this.tabControlModel);
			this.Controls.Add(this.groupBox1);
			this.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormExtractData";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Extract PDS Data";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.tabControlModel.ResumeLayout(false);
			this.tabPageDrawing.ResumeLayout(false);
			this.tabPagePipe.ResumeLayout(false);
			this.tabPageEquipment.ResumeLayout(false);
			this.tabPageStructure.ResumeLayout(false);
			this.tabPageCableTray.ResumeLayout(false);
			this.tabPageMisc.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox textBoxSID;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxHost;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox textBoxPrjDatabasePswd;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBoxPrjDatabaseID;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button buttonPrjDatabaseTestConnection;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button buttonDesignDatabaseTestConnection;
        private System.Windows.Forms.TextBox textBoxDesignDatabasePassword;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBoxDesignDatabaseID;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TabControl tabControlModel;
        private System.Windows.Forms.TabPage tabPagePipe;
        private System.Windows.Forms.TabPage tabPageEquipment;
        private System.Windows.Forms.Button OK;
        private System.Windows.Forms.Button Close;
        private System.Windows.Forms.TabPage tabPageStructure;
        private System.Windows.Forms.TabPage tabPageCableTray;
        private System.Windows.Forms.ProgressBar progressBarStatus;
        private System.Windows.Forms.TabPage tabPageDrawing;
        private System.Windows.Forms.TabPage tabPageMisc;
        private System.Windows.Forms.ListView listViewDrawingModel;
        private System.Windows.Forms.ColumnHeader DrawingFileName;
        private System.Windows.Forms.ListView listViewPipeModel;
        private System.Windows.Forms.ColumnHeader columnHeaderPipeModel;
        private System.Windows.Forms.ListView listViewEquipmentModel;
        private System.Windows.Forms.ColumnHeader columnHeaderEquipmentModel;
        private System.Windows.Forms.ListView listViewStructureModel;
        private System.Windows.Forms.ColumnHeader columnHeaderStructureModel;
        private System.Windows.Forms.ListView listViewCableTModel;
        private System.Windows.Forms.ColumnHeader columnHeaderCableTModel;
        private System.Windows.Forms.ListView listViewMiscModel;
        private System.Windows.Forms.ColumnHeader columnHeaderMiscModel;
		private System.Windows.Forms.Button buttonDump;
		private System.Windows.Forms.ProgressBar progressBarDump;
    }
}

