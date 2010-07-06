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
           String numeroVerifie = VerifSaisie(numero);
           tb_Numero.Text = numeroVerifie;
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
            Boolean plusEnPremier = false;
            String resultatfinal = "";
            numero = numero.Trim();
            //filtre regex on ne garde que ce qui est chiffre et '+'
            System.Text.RegularExpressions.Regex myRegex = new System.Text.RegularExpressions.Regex("[^0-9+]");
            string resultat1 = myRegex.Replace(numero, "");
            
            if (resultat1.Length > 0)//Si le résultat du premier chiffre donne autre chose qu'une chainbe vide on continue
            {
                if (resultat1[0] == '+')//on verifie si le premier élement de la collection de caractére est un "+", si oui on valorise un booleen
                { plusEnPremier = true; }
                myRegex = new System.Text.RegularExpressions.Regex("[^0-9]");//on garde que les chiffres dans la chaine
            }
            //test du boleen si oui on rajoute le caractére '+' 
            if (plusEnPremier)
            {
                resultatfinal = "+" + myRegex.Replace(resultat1, "");
            }
            else
            {
                resultatfinal = myRegex.Replace(resultat1, "");
            }

            return resultatfinal; //renvoi la chaine modifiée    
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
