using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Word = Microsoft.Office.Interop.Word;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Word;
using Microsoft.Office.Tools.Word.Extensions;
using System.Windows.Forms;

namespace Xivo_ClickToCall_Word
{
    public partial class ThisAddIn
    {
        private Word.Application myApplication;
        private Office.CommandBarButton myControl;

        private void ThisAddIn_Shutdown(object sender, System.EventArgs e)
        {
            RemoveExistingMenuItem();
        }

        private void ThisAddIn_Startup(object sender, System.EventArgs e)
        {
            myApplication = this.Application;
            RemoveExistingMenuItem();
            AddMenuItemTel();
            AddMenuItemTel2();
        }

 

        private void AddMenuItemTel()
        {
            Office.MsoControlType menuItem = Office.MsoControlType.msoControlButton;
            myControl = (Office.CommandBarButton)myApplication.CommandBars["Text"].Controls.Add(menuItem, missing, missing, 1, true);
            myControl.Style = Office.MsoButtonStyle.msoButtonIconAndCaption;
            myControl.Picture = getImage();
            myControl.Caption = "Appeler la sélection";
            myControl.Tag = "Xivo_Tel";
            myControl.Click += new Microsoft.Office.Core._CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);


        }
        private void AddMenuItemTel2()
        {
            Office.MsoControlType menuItem = Office.MsoControlType.msoControlButton;
            myControl = (Office.CommandBarButton)myApplication.CommandBars["Tables"].Controls.Add(menuItem, missing, missing, 1, true);
            myControl.Style = Office.MsoButtonStyle.msoButtonIconAndCaption;
            myControl.Picture = getImage();
            myControl.Caption = "Appeler la sélection";
            myControl.Tag = "Xivo_Tel";
            myControl.Click += new Microsoft.Office.Core._CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);


        }

        private void RemoveExistingMenuItem()
        {
            Office.CommandBar contextMenu = myApplication.CommandBars["Text"];
            
            Office.CommandBarButton control = (Office.CommandBarButton)contextMenu.FindControl(Office.MsoControlType.msoControlButton, missing,"Xivo_Tel", true, true);

            if ((control != null))
            {
                control.Delete(true);
            }
        }

        #region Traitement de l'image
        private stdole.IPictureDisp getImage()
        {
            stdole.IPictureDisp tempImage = null;
            System.Drawing.Icon newIcon = null;
            try
            {
                newIcon = Properties.Resources.tel;
                ImageList newImageList = new ImageList();
                newImageList.Images.Add(newIcon);
                tempImage = ConvertImage.Convert(newImageList.Images[0]);
            }
            catch (Exception ex)
            {
               MessageBox.Show("Erreur de Traitement du complément Xivo clickTocall : Contactez le support. erreur : "+ex.Message);
            }
            return tempImage;
        }


        sealed public class ConvertImage : System.Windows.Forms.AxHost
        {
            private ConvertImage()
                : base(null)
            {
            }
            public static stdole.IPictureDisp Convert
                (System.Drawing.Image image)
            {
                return (stdole.IPictureDisp)System.
                    Windows.Forms.AxHost
                    .GetIPictureDispFromPicture(image);
            }
        }
        #endregion

        void ControlTel_Click(Microsoft.Office.Core.CommandBarButton Ctrl, ref bool CancelDefault)
        {
           
            Word.Selection selection = myApplication.ActiveWindow.Selection;
            Xivo_Appel FnvAppel = new Xivo_Appel(selection.Text);
            FnvAppel.ShowDialog();
        }
      
        #region Code généré par VSTO

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InternalStartup()
        {
            this.Startup += new System.EventHandler(ThisAddIn_Startup);
            this.Shutdown += new System.EventHandler(ThisAddIn_Shutdown);
        }
        
        #endregion
    }
}
