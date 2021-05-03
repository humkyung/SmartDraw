namespace DataExchange
{
	partial class FormElevationTypeBySpec
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.dataGridViewElevationTypeBySpec = new System.Windows.Forms.DataGridView();
			this.buttonClose = new System.Windows.Forms.Button();
			this.buttonOK = new System.Windows.Forms.Button();
			this.Spec = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.ElevationType = new System.Windows.Forms.DataGridViewComboBoxColumn();
			((System.ComponentModel.ISupportInitialize)(this.dataGridViewElevationTypeBySpec)).BeginInit();
			this.SuspendLayout();
			// 
			// dataGridViewElevationTypeBySpec
			// 
			this.dataGridViewElevationTypeBySpec.AllowUserToAddRows = false;
			this.dataGridViewElevationTypeBySpec.AllowUserToDeleteRows = false;
			this.dataGridViewElevationTypeBySpec.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.dataGridViewElevationTypeBySpec.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridViewElevationTypeBySpec.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Spec,
            this.ElevationType});
			this.dataGridViewElevationTypeBySpec.Location = new System.Drawing.Point(12, 12);
			this.dataGridViewElevationTypeBySpec.Name = "dataGridViewElevationTypeBySpec";
			this.dataGridViewElevationTypeBySpec.RowTemplate.Height = 23;
			this.dataGridViewElevationTypeBySpec.Size = new System.Drawing.Size(475, 317);
			this.dataGridViewElevationTypeBySpec.TabIndex = 0;
			// 
			// buttonClose
			// 
			this.buttonClose.Location = new System.Drawing.Point(412, 335);
			this.buttonClose.Name = "buttonClose";
			this.buttonClose.Size = new System.Drawing.Size(75, 25);
			this.buttonClose.TabIndex = 1;
			this.buttonClose.Text = "Close";
			this.buttonClose.UseVisualStyleBackColor = true;
			this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
			// 
			// buttonOK
			// 
			this.buttonOK.Location = new System.Drawing.Point(331, 335);
			this.buttonOK.Name = "buttonOK";
			this.buttonOK.Size = new System.Drawing.Size(75, 25);
			this.buttonOK.TabIndex = 2;
			this.buttonOK.Text = "OK";
			this.buttonOK.UseVisualStyleBackColor = true;
			this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
			// 
			// Spec
			// 
			this.Spec.HeaderText = "Spec";
			this.Spec.Name = "Spec";
			this.Spec.ReadOnly = true;
			// 
			// ElevationType
			// 
			this.ElevationType.HeaderText = "ElevationType";
			this.ElevationType.Items.AddRange(new object[] {
            "CENTER",
            "BOP",
            "TOP",
            "Mixed(CL + BOP)"});
			this.ElevationType.Name = "ElevationType";
			// 
			// FormElevationTypeBySpec
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(499, 372);
			this.Controls.Add(this.buttonOK);
			this.Controls.Add(this.buttonClose);
			this.Controls.Add(this.dataGridViewElevationTypeBySpec);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormElevationTypeBySpec";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Elevation Type By Spec";
			this.TopMost = true;
			this.Load += new System.EventHandler(this.FormElevationTypeBySpec_Load);
			((System.ComponentModel.ISupportInitialize)(this.dataGridViewElevationTypeBySpec)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.DataGridView dataGridViewElevationTypeBySpec;
		private System.Windows.Forms.Button buttonClose;
		private System.Windows.Forms.Button buttonOK;
		private System.Windows.Forms.DataGridViewTextBoxColumn Spec;
		private System.Windows.Forms.DataGridViewComboBoxColumn ElevationType;
	}
}