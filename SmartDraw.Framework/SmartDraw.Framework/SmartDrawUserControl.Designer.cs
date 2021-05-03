namespace SmartDrawFramework
{
    partial class SmartDrawUserControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.progressBarDump = new System.Windows.Forms.ProgressBar();
			this.labelStatus = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// progressBarDump
			// 
			this.progressBarDump.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.progressBarDump.Location = new System.Drawing.Point(0, 14);
			this.progressBarDump.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
			this.progressBarDump.Name = "progressBarDump";
			this.progressBarDump.Size = new System.Drawing.Size(286, 10);
			this.progressBarDump.TabIndex = 0;
			// 
			// labelStatus
			// 
			this.labelStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.labelStatus.Location = new System.Drawing.Point(0, 0);
			this.labelStatus.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
			this.labelStatus.Name = "labelStatus";
			this.labelStatus.Size = new System.Drawing.Size(286, 12);
			this.labelStatus.TabIndex = 1;
			// 
			// SmartDrawUserControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 14F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.labelStatus);
			this.Controls.Add(this.progressBarDump);
			this.Font = new System.Drawing.Font("Arial", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
			this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
			this.Name = "SmartDrawUserControl";
			this.Size = new System.Drawing.Size(286, 25);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ProgressBar progressBarDump;
		private System.Windows.Forms.Label labelStatus;
    }
}
