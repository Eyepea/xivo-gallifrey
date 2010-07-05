using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Office.Tools.Ribbon;
using Microsoft.Office.Interop.Outlook;
using System.Windows.Forms;

namespace Xivo_ClickToCall_Outlook
{
    public partial class Ribbon_ClickToCall_Xivo
    {
        
       


        private void Ribbon_ClickToCall_Xivo_Load(object sender, RibbonUIEventArgs e)
        {
        }


        private void button1_Click(object sender, RibbonControlEventArgs e)
        {
            Xivo_Appel formAppel = new Xivo_Appel("");
            formAppel.Show();
        }

    }
}
