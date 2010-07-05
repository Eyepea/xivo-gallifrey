using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace Xivo_ClickToCall_Word
{
    public partial class Xivo_Appel : Form
    {
        private string _numero;
        
        public Xivo_Appel(string numero)
        {
            InitializeComponent();
            _numero = VerifSaisie(numero);
            tb_Numero.Text = _numero.Trim();
        }

        private string VerifSaisie(string numero)
        {
            numero=numero.Trim();
            System.Text.RegularExpressions.Regex myRegex = new System.Text.RegularExpressions.Regex("[^0-9+]");
            return myRegex.Replace(numero,"");    
        }
  
        private void Xivo_Appel_Load(object sender, EventArgs e)
        {

        }

        private void button_Fermer_Click(object sender, EventArgs e)
        {
            this.Close();
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
            }
            catch {
                MessageBox.Show("Erreur de transmission de donnée au Xivo Client");
                this.Close();
            
            }
        }
    }
}
