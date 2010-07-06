using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Outlook = Microsoft.Office.Interop.Outlook;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Core;
using Microsoft.Office.Interop.Outlook;
using System.Windows.Forms;

namespace Xivo_ClickToCall_Outlook
{
    public partial class ThisAddIn
    {
        public Boolean uneForm = false; //booleen pour bloquer l'ouverture de plus d'une fenêtre

        private void ThisAddIn_Startup(object sender, System.EventArgs e)
        {
            this.Application.ItemContextMenuDisplay += new Microsoft.Office.Interop.Outlook.ApplicationEvents_11_ItemContextMenuDisplayEventHandler(Application_ItemContextMenuDisplay);
        }

        void Application_ItemContextMenuDisplay(Microsoft.Office.Core.CommandBar commandBar, Microsoft.Office.Interop.Outlook.Selection selection)
        {
            CommandBarPopup rootButton = null;
            ContactItem contactSelect = null;
            MsoControlType linkType = MsoControlType.msoControlButton;
            if (Application.ActiveExplorer().Selection[1] is ContactItem)
            {
               
                try
                {

                    contactSelect = (ContactItem)Application.ActiveExplorer().Selection[1];   
                    rootButton = (CommandBarPopup)commandBar.Controls.Add(Office.MsoControlType.msoControlPopup, Type.Missing, "Xivo_tel", commandBar.Controls.Count + 1, Type.Missing);
                    rootButton.BeginGroup = true;
                    rootButton.Tag = "Xivo_tel";
                    rootButton.Caption = "Xivo Appeler...";
                    rootButton.Visible = true;

                    if (contactSelect.BusinessTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "BusinessTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "BusinessTelephoneNumber";
                        // folderLink.Caption = "Business Telephone Number : " + contactSelect.BusinessTelephoneNumber;
                        folderLink.Caption = "Bureau : " + contactSelect.BusinessTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
            

                    if (contactSelect.Business2TelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "Business2TelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "Business2TelephoneNumber";
                        //  folderLink.Caption = "Business2 Telephone Number : " + contactSelect.Business2TelephoneNumber;
                        folderLink.Caption = "Bureau2 : " + contactSelect.Business2TelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
           

                    if (contactSelect.AssistantTelephoneNumber != null)
                    {

                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "AssistantTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "AssistantTelephoneNumber";
                        //folderLink.Caption = "Assistant Telephone Number : " + contactSelect.AssistantTelephoneNumber;
                        folderLink.Caption = "Assistant : " + contactSelect.AssistantTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                

                    if (contactSelect.CallbackTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "CallbackTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "CallbackTelephoneNumber";
                        // folderLink.Caption = "Callback Telephone Number : " + contactSelect.CallbackTelephoneNumber;
                        folderLink.Caption = "Rappel : " + contactSelect.CallbackTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                

                    if (contactSelect.CarTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "CarTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "CarTelephoneNumber";
                        // folderLink.Caption = "Car Telephone Number :"+contactSelect.CarTelephoneNumber;
                        folderLink.Caption = "Voiture :" + contactSelect.CarTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                

                    if (contactSelect.CompanyMainTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "CompanyMainTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "CompanyMainTelephoneNumber";
                        // folderLink.Caption = "Company Main Telephone Number : "+contactSelect.CompanyMainTelephoneNumber;
                        folderLink.Caption = "Société : " + contactSelect.CompanyMainTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
               
                    if (contactSelect.Home2TelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "Home2TelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "Home2TelephoneNumber";
                        //folderLink.Caption = "Home2 Telephone Number : "+contactSelect.Home2TelephoneNumber;
                        folderLink.Caption = "Domicile2 : " + contactSelect.Home2TelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                
                    if (contactSelect.HomeTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "HomeTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "HomeTelephoneNumber";
                        //folderLink.Caption = "Home Telephone Number : " + contactSelect.HomeTelephoneNumber ;
                        folderLink.Caption = "Domicile : " + contactSelect.HomeTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                
                    if (contactSelect.ISDNNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "ISDNNumber", 1, Type.Missing);
                        folderLink.Tag = "ISDNNumber";
                        // folderLink.Caption = "ISDN Number : " + contactSelect.ISDNNumber;
                        folderLink.Caption = "RNIS : " + contactSelect.ISDNNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                
                    if (contactSelect.MobileTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "MobileTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "MobileTelephoneNumber";
                        //folderLink.Caption = "Mobile Telephone Number : "+contactSelect.MobileTelephoneNumber;
                        folderLink.Caption = "Telephone mobile : " + contactSelect.MobileTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                
                    if (contactSelect.OtherTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "OtherTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "OtherTelephoneNumber";
                        //folderLink.Caption = "Other Telephone Number : "+contactSelect.OtherTelephoneNumber;
                        folderLink.Caption = "Autre : " + contactSelect.OtherTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                
                    if (contactSelect.PrimaryTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "PrimaryTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "PrimaryTelephoneNumber";
                        //folderLink.Caption = "Primary Telephone Number : "+contactSelect.PrimaryTelephoneNumber;
                        folderLink.Caption = "Principal : " + contactSelect.PrimaryTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                
                    if (contactSelect.RadioTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "RadioTelephoneNumber", 1, Type.Missing);
                        folderLink.Tag = "RadioTelephoneNumber";
                        //folderLink.Caption = "Radio Telephone Number : "+contactSelect.RadioTelephoneNumber;
                        folderLink.Caption = "Radio : " + contactSelect.RadioTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                
                    if (contactSelect.TelexNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "TelexNumber", 1, Type.Missing);
                        folderLink.Tag = "TelexNumber";
                        //folderLink.Caption = "Telex Number : "+contactSelect.TelexNumber;
                        folderLink.Caption = "Telex : " + contactSelect.TelexNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }

                    if (contactSelect.TTYTDDTelephoneNumber != null)
                    {
                        CommandBarControl folderLink = rootButton.Controls.Add(linkType, Type.Missing, "TTYTDD Telephone Number", 1, Type.Missing);
                        folderLink.Tag = "TTYTDDTelephoneNumber";
                        // folderLink.Caption = "TTYTDD Telephone Number : " + contactSelect.TTYTDDTelephoneNumber;
                        folderLink.Caption = "TTY/TDD : " + contactSelect.TTYTDDTelephoneNumber;
                        (folderLink as CommandBarButton).Click += new _CommandBarButtonEvents_ClickEventHandler(ControlTel_Click);
                    }
                }
                catch (InvalidCastException e)
                {
                    MessageBox.Show("Contacter le service support erreur Xivo click to call :" + e.ToString());
                }
            }
        }

        void ControlTel_Click(Microsoft.Office.Core.CommandBarButton Ctrl, ref bool CancelDefault)
        { //utilisation d'un booleen pour bloquer l'ouverture de plus d'une fenêtre
            if (!uneForm)
            {
                String numero = Ctrl.accName;
                Xivo_Appel formAppel = new Xivo_Appel(numero.Substring(numero.IndexOf(':') + 2));
                formAppel.Show();
                uneForm = true;
            }
        }


        #region Traitement de l'image

        private stdole.IPictureDisp getImage(string image)
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
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message);
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

        private void ThisAddIn_Shutdown(object sender, System.EventArgs e)
        {
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
