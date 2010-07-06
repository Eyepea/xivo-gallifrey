using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace Xivo_ClickToCall_Outlook
{
    public partial class Xivo_Appel : Form
    {
        public Xivo_Appel(string numero)
        {
           InitializeComponent();
            tb_Numero.Text = VerifSaisie(numero);

        }

        private void xivo_appel_Load(object sender, EventArgs e)
        {

        }

        private void button_Appeler_Click(object sender, EventArgs e)
        {
            try
            {
                //récupération de la clef registre Xivo install
                Microsoft.Win32.RegistryKey cle = Microsoft.Win32.Registry.LocalMachine.OpenSubKey(@"SOFTWARE\XIVO\xivoclient\", false);
                //capture du chemin d'installation du client xivo
                string readValue = cle.GetValue("Install_Dir").ToString();
                readValue += @"\xivoclient.exe";
                //definition de l'argument
                string arg = @" tel:" + tb_Numero.Text;
                //création et lancement du process
                ProcessStartInfo processInfo = new ProcessStartInfo(readValue, arg);
                Process myProcess = Process.Start(processInfo);
                myProcess.Close();
                MessageBox.Show("Numéro transmis au Xivo Client");
                this.Close();
                this.Dispose();
                //utilisation d'un booleen pour bloquer l'ouverture de plus d'une fenêtre
                Globals.ThisAddIn.uneForm = false;
            }
            catch
            {
                MessageBox.Show("Erreur de transmission de donnée au Xivo Client");
                this.Close();
                this.Dispose();
                //utilisation d'un booleen pour bloquer l'ouverture de plus d'une fenêtre
                Globals.ThisAddIn.uneForm = false;
            }
        }

        private string VerifSaisie(string numero)
        {
            numero = numero.Trim();
            System.Text.RegularExpressions.Regex myRegex = new System.Text.RegularExpressions.Regex("[^0-9+]");
            return myRegex.Replace(numero, ""); //renvoi la chaine modifiée    
        }

        private void button_Fermer_Click_1(object sender, EventArgs e)
        {
            this.Close();
            this.Dispose();
            //utilisation d'un booleen pour bloquer l'ouverture de plus d'une fenêtre
            Globals.ThisAddIn.uneForm = false;
        }

        private void Xivo_Appel_FormClosed(object sender, FormClosedEventArgs e)
        {
            this.Dispose();
            //utilisation d'un booleen pour bloquer l'ouverture de plus d'une fenêtre
            Globals.ThisAddIn.uneForm = false;
        }
    }
}
