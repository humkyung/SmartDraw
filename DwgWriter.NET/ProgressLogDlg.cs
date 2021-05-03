using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace aDwgWriter
{
	public partial class ProgressLogDlg : Form
	{
		private static ProgressLogDlg Instance_;
		public static ProgressLogDlg Instance
		{
			get
			{
				if (ProgressLogDlg.Instance_ == null)
					ProgressLogDlg.Instance_ = new ProgressLogDlg();
				return ProgressLogDlg.Instance_;
			}
		}
		public event EventHandler StartWork;

		public ProgressLogDlg()
		{
			InitializeComponent();
			this.Disposed += new EventHandler(this.ProgressLogDlg_Disposed);
		}

		private void ProgressLogDlg_Disposed(object sender, EventArgs e)
		{
			ProgressLogDlg.Instance_ = (ProgressLogDlg)null;
		}

		private void ProgressLogDlg_Load(object sender, EventArgs e)
		{
			CenterToParent();
			if (null != StartWork) StartWork(ProgressLogDlg.Instance, e);
		}

		private void buttonClose_Click(object sender, EventArgs e)
		{
			Close();
		}
	}
}
