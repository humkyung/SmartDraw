namespace DataExchange
{
    partial class WaitDlgModeless
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
			this.ulblMessage = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// ulblMessage
			// 
			this.ulblMessage.Dock = System.Windows.Forms.DockStyle.Fill;
			this.ulblMessage.Location = new System.Drawing.Point(0, 0);
			this.ulblMessage.Name = "ulblMessage";
			this.ulblMessage.Size = new System.Drawing.Size(400, 80);
			this.ulblMessage.TabIndex = 0;
			this.ulblMessage.Text = "Waiting ...";
			this.ulblMessage.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// WaitDlgModeless
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(400, 80);
			this.Controls.Add(this.ulblMessage);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Name = "WaitDlgModeless";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Waiting ...";
			this.TopMost = true;
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label ulblMessage;
    }
}