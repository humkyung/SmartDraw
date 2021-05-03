using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.EditorInput;

using acApp = Autodesk.AutoCAD.ApplicationServices.Application;

namespace aDwgWriter
{
	public partial class CustomToolbar : Form
	{
		private static CustomToolbar s_CustomToolbar;
		public static CustomToolbar Instance
		{
			get
			{
				if (CustomToolbar.s_CustomToolbar == null)
					CustomToolbar.s_CustomToolbar = new CustomToolbar();
				return CustomToolbar.s_CustomToolbar;
			}
		}
		private System.Drawing.Point StartLocation;

		static CustomToolbar()
		{
		}

		public CustomToolbar()
		{
			this.InitializeComponent();
		}

		private void CustomToolbar_Load(object sender, EventArgs e)
		{
			try
			{
				CenterToParent();
				this.Height = toolStripWeldingMap.Size.Height;
				this.Width = this.pictureBoxFrontSpace.Width + this.toolStripWeldingMap.Size.Width + 5;
			}
			catch (Exception ex)
			{
				acApp.ShowAlertDialog(ex.Message);
				this.Close();
			}
		}

		private void pictureBoxFrontSpace_MouseDown(object sender, MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Left) return;
			pictureBoxFrontSpace.MouseMove += new MouseEventHandler(this.pictureBoxFrontSpace_MouseMove);
			this.StartLocation = new System.Drawing.Point(e.X, e.Y);
		}

		private void pictureBoxFrontSpace_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				pictureBoxFrontSpace.MouseMove -= new MouseEventHandler(this.pictureBoxFrontSpace_MouseMove);
			}
			else
			{
				if (e.Button != MouseButtons.Right) return;
			}
		}

		private void pictureBoxFrontSpace_MouseEnter(object sender, EventArgs e)
		{
			this.Activate();
		}

		private void pictureBoxFrontSpace_MouseLeave(object sender, EventArgs e)
		{
			Autodesk.AutoCAD.Internal.Utils.SetFocusToDwgView();
		}

		private void pictureBoxFrontSpace_MouseMove(object sender, MouseEventArgs e)
		{
			System.Drawing.Point point = this.PointToScreen(new System.Drawing.Point(e.X, e.Y));
			point.Offset(-this.StartLocation.X, -this.StartLocation.Y);
			this.Location = point;
		}

		private void toolStripButtonPickPosition_Click(object sender, EventArgs e)
		{
			Autodesk.AutoCAD.Internal.Utils.SetFocusToDwgView();

			PromptPointResult oResult = AcEx.Editor.GetPoint(new PromptPointOptions("\n Pick a point :"));
			if (oResult.Status == Autodesk.AutoCAD.EditorInput.PromptStatus.OK)
			{
				Point3d pt = oResult.Value;
				string sFilePath = Path.Combine(Path.GetTempPath(), "aPickTool.dat");
				using (StreamWriter oWriter = new StreamWriter(sFilePath))
				{
					oWriter.Write(string.Format("{0},{1},{2}", pt.X, pt.Y, pt.Z));
				}
			}
		}

		private void toolStripButtonCompareIsoDwg_MouseEnter(object sender, EventArgs e)
		{
			this.Activate();
		}

		private void CustomToolbar_MouseLeave(object sender, EventArgs e)
		{
			Autodesk.AutoCAD.Internal.Utils.SetFocusToDwgView();
		}
	}
}
