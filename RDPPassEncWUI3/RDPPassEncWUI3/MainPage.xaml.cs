using System;
using System.Collections.Generic;
using System.CommandLine.Parsing;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Windows.ApplicationModel.DataTransfer;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace RDPPassEncWUI3
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private Encoding u16LE = Encoding.Unicode;

        public MainPage()
        {
            InitializeComponent();
        }

        public void OnRedirected(string someArgs)
        {
            string[] splitArgs = CommandLineStringSplitter.Instance.Split(someArgs).ToArray();
            string linesArgs = string.Join("\r\n", splitArgs) + "\r\n";
            TextBoxDecrypted.Text = linesArgs;
        }

        private string EncryptPassword(string strDecrypted)
        {
            string strEncrypted = "";

            try
            {
                byte[] bytesDecrypted = u16LE.GetBytes(strDecrypted);
                byte[] bytesEncrypted = ProtectedData.Protect(bytesDecrypted, null, DataProtectionScope.CurrentUser);
                foreach (var byteValue in bytesEncrypted)
                {
                    strEncrypted += String.Format("{0:X2}", byteValue);
                }
            }
            catch (Exception)
            {
                strEncrypted = "Encryption error.";
            }

            return strEncrypted;
        }

        private string DecryptPassword(string strEncrypted)
        {
            string strDecrypted = "";

            try
            {
                int encryptedBytesLength = strEncrypted.Length / 2;
                byte[] bytesEncrypted = new byte[encryptedBytesLength];
                for (int i = 0; i < encryptedBytesLength; i++)
                {
                    bytesEncrypted[i] = Convert.ToByte(strEncrypted.Substring(i * 2, 2), 16);
                }
                byte[] bytesDecrypted = ProtectedData.Unprotect(bytesEncrypted, null, DataProtectionScope.CurrentUser);
                strDecrypted = u16LE.GetString(bytesDecrypted);
            }
            catch (Exception)
            {
                strDecrypted = "Decryption error.";
            }

            return strDecrypted;
        }

        private void ButtonDecryptedClear_Click(object sender, RoutedEventArgs e)
        {
            TextBoxDecrypted.Text = "";
        }

        private void ButtonEncryptedClear_Click(object sender, RoutedEventArgs e)
        {
            TextBoxEncrypted.Text = "";
        }

        private void ButtonEncrypt_Click(object sender, RoutedEventArgs e)
        {
            TextBoxEncrypted.Text = EncryptPassword(TextBoxDecrypted.Text);
        }

        private void ButtonDecrypt_Click(object sender, RoutedEventArgs e)
        {
            TextBoxDecrypted.Text = DecryptPassword(TextBoxEncrypted.Text);
        }

        private void ButtonAbout_Click(object sender, RoutedEventArgs e)
        {
            Frame.Navigate(typeof(AboutPage));
        }

        private void GridRoot_DragOver(object sender, DragEventArgs e)
        {
            e.AcceptedOperation = DataPackageOperation.Copy;
        }

        private async void GridRoot_Drop(object sender, DragEventArgs e)
        {
            List<string> strDropFilesPath = await WinUIHelper.GetDropFilesPath(e);
            DispatcherQueue.TryEnqueue(() => TextBoxDecrypted.Text = string.Join("\r\n", strDropFilesPath) + "\r\n");
        }
    }
}
