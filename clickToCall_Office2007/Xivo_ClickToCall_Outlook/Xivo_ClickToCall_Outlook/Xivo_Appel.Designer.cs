namespace Xivo_ClickToCall_Outlook
{
    partial class Xivo_Appel
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Xivo_Appel));
            this.tb_Numero = new System.Windows.Forms.TextBox();
            this.button_Appeler = new System.Windows.Forms.Button();
            this.button_Fermer = new System.Windows.Forms.Button();
            this.groupBox_NumeroaComposer = new System.Windows.Forms.GroupBox();
            this.label_Numero = new System.Windows.Forms.Label();
            this.groupBox_NumeroaComposer.SuspendLayout();
            this.SuspendLayout();
            // 
            // tb_Numero
            // 
            this.tb_Numero.Location = new System.Drawing.Point(62, 19);
            this.tb_Numero.Name = "tb_Numero";
            this.tb_Numero.Size = new System.Drawing.Size(185, 20);
            this.tb_Numero.TabIndex = 1;
            // 
            // button_Appeler
            // 
            this.button_Appeler.Location = new System.Drawing.Point(12, 66);
            this.button_Appeler.Name = "button_Appeler";
            this.button_Appeler.Size = new System.Drawing.Size(108, 23);
            this.button_Appeler.TabIndex = 14;
            this.button_Appeler.Text = "Début de l\'appel";
            this.button_Appeler.UseVisualStyleBackColor = true;
            this.button_Appeler.Click += new System.EventHandler(this.button_Appeler_Click);
            // 
            // button_Fermer
            // 
            this.button_Fermer.Location = new System.Drawing.Point(157, 66);
            this.button_Fermer.Name = "button_Fermer";
            this.button_Fermer.Size = new System.Drawing.Size(108, 23);
            this.button_Fermer.TabIndex = 15;
            this.button_Fermer.Text = "Fermer";
            this.button_Fermer.UseVisualStyleBackColor = true;
            this.button_Fermer.Click += new System.EventHandler(this.button_Fermer_Click_1);
            // 
            // groupBox_NumeroaComposer
            // 
            this.groupBox_NumeroaComposer.Controls.Add(this.tb_Numero);
            this.groupBox_NumeroaComposer.Controls.Add(this.label_Numero);
            this.groupBox_NumeroaComposer.Location = new System.Drawing.Point(12, 12);
            this.groupBox_NumeroaComposer.Name = "groupBox_NumeroaComposer";
            this.groupBox_NumeroaComposer.Size = new System.Drawing.Size(253, 50);
            this.groupBox_NumeroaComposer.TabIndex = 13;
            this.groupBox_NumeroaComposer.TabStop = false;
            this.groupBox_NumeroaComposer.Text = "Numéro à Composer";
            // 
            // label_Numero
            // 
            this.label_Numero.AutoSize = true;
            this.label_Numero.Location = new System.Drawing.Point(6, 22);
            this.label_Numero.Name = "label_Numero";
            this.label_Numero.Size = new System.Drawing.Size(50, 13);
            this.label_Numero.TabIndex = 0;
            this.label_Numero.Text = "Numéro :";
            // 
            // Xivo_Appel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(276, 101);
            this.Controls.Add(this.button_Appeler);
            this.Controls.Add(this.button_Fermer);
            this.Controls.Add(this.groupBox_NumeroaComposer);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(284, 135);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(284, 135);
            this.Name = "Xivo_Appel";
            this.Text = "xivo : Nouvelle appel";
            this.TopMost = true;
            this.groupBox_NumeroaComposer.ResumeLayout(false);
            this.groupBox_NumeroaComposer.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox tb_Numero;
        private System.Windows.Forms.Button button_Appeler;
        private System.Windows.Forms.Button button_Fermer;
        private System.Windows.Forms.GroupBox groupBox_NumeroaComposer;
        private System.Windows.Forms.Label label_Numero;

    }
}