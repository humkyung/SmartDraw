namespace aDwgWriter
{
	partial class ProgressLogDlg
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
			this.textBoxFileName = new System.Windows.Forms.TextBox();
			this.progressBarStatus = new ProgressBarWithCaption();
			this.buttonClose = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// textBoxFileName
			// 
			this.textBoxFileName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxFileName.Location = new System.Drawing.Point(12, 12);
			this.textBoxFileName.Name = "textBoxFileName";
			this.textBoxFileName.ReadOnly = true;
			this.textBoxFileName.Size = new System.Drawing.Size(447, 21);
			this.textBoxFileName.TabIndex = 0;
			// 
			// progressBarStatus
			// 
			this.progressBarStatus.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.progressBarStatus.Location = new System.Drawing.Point(12, 39);
			this.progressBarStatus.Name = "progressBarStatus";
			this.progressBarStatus.Size = new System.Drawing.Size(447, 23);
			this.progressBarStatus.TabIndex = 1;
			// 
			// buttonClose
			// 
			this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonClose.Image = global::DwgWriter.Properties.Resources.but_Ok;
			this.buttonClose.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.buttonClose.Location = new System.Drawing.Point(384, 68);
			this.buttonClose.Name = "buttonClose";
			this.buttonClose.Size = new System.Drawing.Size(75, 30);
			this.buttonClose.TabIndex = 2;
			this.buttonClose.Text = "Close";
			this.buttonClose.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.buttonClose.UseVisualStyleBackColor = true;
			this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
			// 
			// ProgressLogDlg
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(471, 108);
			this.Controls.Add(this.buttonClose);
			this.Controls.Add(this.progressBarStatus);
			this.Controls.Add(this.textBoxFileName);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ProgressLogDlg";
			this.Text = "ProgressLogDlg";
			this.Load += new System.EventHandler(this.ProgressLogDlg_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		public System.Windows.Forms.TextBox textBoxFileName;
		public ProgressBarWithCaption progressBarStatus;
		public System.Windows.Forms.Button buttonClose;
	}
}