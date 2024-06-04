using System;
using System.Security.Cryptography;
using System.Text;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace RDPPassEncWUI3
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            InitializeComponent();
        }

        private string EncryptPassword(string strDecrypted)
        {
            string strEncrypted = "";
            Encoding u16LE = Encoding.Unicode;
            byte[] bytesDecrypted = u16LE.GetBytes(strDecrypted);
            byte[] bytesEncrypted = ProtectedData.Protect(bytesDecrypted, null, DataProtectionScope.CurrentUser);
            foreach (var byteValue in bytesEncrypted)
            {
                strEncrypted += String.Format("{0:X2}", byteValue);
            }

            return strEncrypted;
        }

        private void TextBoxDecrypted_TextChanged(object sender, TextChangedEventArgs e)
        {
            string strDecrypted = TextBoxDecrypted.Text;
            string strEncrypted = EncryptPassword(strDecrypted);
            TextBoxEncrypted.Text = strEncrypted;
        }

        private void TextBoxEncrypted_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void ButtonAbout_Click(object sender, RoutedEventArgs e)
        {
            Frame.Navigate(typeof(AboutPage));
        }
    }
}
