namespace Xivo_ClickToCall_Outlook
{
    partial class Ribbon_ClickToCall_Xivo : Microsoft.Office.Tools.Ribbon.OfficeRibbon
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        public Ribbon_ClickToCall_Xivo()
        {
            InitializeComponent();
        }

        /// <summary> 
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur de composants

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.tab1 = new Microsoft.Office.Tools.Ribbon.RibbonTab();
            this.Xivo_Group = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.button1 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.tab1.SuspendLayout();
            this.Xivo_Group.SuspendLayout();
            this.SuspendLayout();
            // 
            // tab1
            // 
            this.tab1.ControlId.ControlIdType = Microsoft.Office.Tools.Ribbon.RibbonControlIdType.Office;
            this.tab1.ControlId.OfficeId = "TabContact";
            this.tab1.Groups.Add(this.Xivo_Group);
            this.tab1.Label = "TabContact";
            this.tab1.Name = "tab1";
            // 
            // Xivo_Group
            // 
            this.Xivo_Group.Items.Add(this.button1);
            this.Xivo_Group.Label = "Xivo";
            this.Xivo_Group.Name = "Xivo_Group";
            this.Xivo_Group.Position = Microsoft.Office.Tools.Ribbon.RibbonPosition.BeforeOfficeId("GroupCommunicate");
            // 
            // button1
            // 
            this.button1.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge;
            this.button1.Image = global::Xivo_ClickToCall_Outlook.Properties.Resources.xivo1;
            this.button1.Label = " ";
            this.button1.Name = "button1";
            this.button1.ShowImage = true;
            this.button1.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.button1_Click);
            // 
            // Ribbon_ClickToCall_Xivo
            // 
            this.Name = "Ribbon_ClickToCall_Xivo";
            this.RibbonType = "Microsoft.Outlook.Contact";
            this.Tabs.Add(this.tab1);
            this.tab1.ResumeLayout(false);
            this.tab1.PerformLayout();
            this.Xivo_Group.ResumeLayout(false);
            this.Xivo_Group.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonTab tab1;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup Xivo_Group;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button1;
    }

    partial class ThisRibbonCollection : Microsoft.Office.Tools.Ribbon.RibbonReadOnlyCollection
    {
        internal Ribbon_ClickToCall_Xivo Ribbon_ClickToCall_Xivo
        {
            get { return this.GetRibbon<Ribbon_ClickToCall_Xivo>(); }
        }
    }
}
