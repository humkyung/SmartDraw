using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace DataExchange
{
    public partial class WaitDlgModeless : Form
    {
        public WaitDlgModeless()
        {
            InitializeComponent();
            //

            _initialize();
        }

        private void _initialize()
        {
            //Events
            this.Load += new EventHandler(WaitDlgModeless_Load);

        }

        void WaitDlgModeless_Load(object sender, EventArgs e)
        {
            try
            {
                this.CenterToParent();
            }
            catch (System.Exception ex)
            {
#if DEBUG
                MessageBox.Show(ex.Message + "\n----\n" + ex.StackTrace);
#else
                MessageBox.Show(ex.Message);
#endif
            }
        }

        public void SetMessage(string strMessage)
        {
            ulblMessage.Text = strMessage;
        }
    }
}
