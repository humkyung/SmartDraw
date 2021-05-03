namespace aDwgWriter
{
	partial class CustomToolbar
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
			this.toolStripWeldingMap = new System.Windows.Forms.ToolStrip();
			this.toolStripButtonPickPosition = new System.Windows.Forms.ToolStripButton();
			this.pictureBoxFrontSpace = new System.Windows.Forms.PictureBox();
			this.toolStripWeldingMap.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxFrontSpace)).BeginInit();
			this.SuspendLayout();
			// 
			// toolStripWeldingMap
			// 
			this.toolStripWeldingMap.Dock = System.Windows.Forms.DockStyle.None;
			this.toolStripWeldingMap.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonPickPosition});
			this.toolStripWeldingMap.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow;
			this.toolStripWeldingMap.Location = new System.Drawing.Point(15, 0);
			this.toolStripWeldingMap.Name = "toolStripWeldingMap";
			this.toolStripWeldingMap.Size = new System.Drawing.Size(35, 25);
			this.toolStripWeldingMap.TabIndex = 1;
			this.toolStripWeldingMap.Text = "toolStrip1";
			// 
			// toolStripButtonPickPosition
			// 
			this.toolStripButtonPickPosition.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonPickPosition.Image = global::DwgWriter.Properties.Resources.mouse_pick;
			this.toolStripButtonPickPosition.ImageTransparentColor = System.Drawing.Color.DarkGray;
			this.toolStripButtonPickPosition.Name = "toolStripButtonPickPosition";
			this.toolStripButtonPickPosition.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonPickPosition.Text = "Pick position";
			this.toolStripButtonPickPosition.MouseEnter += new System.EventHandler(this.toolStripButtonCompareIsoDwg_MouseEnter);
			this.toolStripButtonPickPosition.Click += new System.EventHandler(this.toolStripButtonPickPosition_Click);
			// 
			// pictureBoxFrontSpace
			// 
			this.pictureBoxFrontSpace.Image = global::DwgWriter.Properties.Resources.FrontSpace_16;
			this.pictureBoxFrontSpace.Location = new System.Drawing.Point(0, 0);
			this.pictureBoxFrontSpace.Name = "pictureBoxFrontSpace";
			this.pictureBoxFrontSpace.Size = new System.Drawing.Size(12, 25);
			this.pictureBoxFrontSpace.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.pictureBoxFrontSpace.TabIndex = 2;
			this.pictureBoxFrontSpace.TabStop = false;
			this.pictureBoxFrontSpace.MouseLeave += new System.EventHandler(this.pictureBoxFrontSpace_MouseLeave);
			this.pictureBoxFrontSpace.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBoxFrontSpace_MouseDown);
			this.pictureBoxFrontSpace.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBoxFrontSpace_MouseUp);
			this.pictureBoxFrontSpace.MouseEnter += new System.EventHandler(this.pictureBoxFrontSpace_MouseEnter);
			// 
			// CustomToolbar
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(57, 25);
			this.Controls.Add(this.pictureBoxFrontSpace);
			this.Controls.Add(this.toolStripWeldingMap);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "CustomToolbar";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "SmartDrawToolbar";
			this.Load += new System.EventHandler(this.CustomToolbar_Load);
			this.MouseLeave += new System.EventHandler(this.CustomToolbar_MouseLeave);
			this.toolStripWeldingMap.ResumeLayout(false);
			this.toolStripWeldingMap.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBoxFrontSpace)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.PictureBox pictureBoxFrontSpace;
		private System.Windows.Forms.ToolStripButton toolStripButtonPickPosition;
		private System.Windows.Forms.ToolStrip toolStripWeldingMap;

	}
}