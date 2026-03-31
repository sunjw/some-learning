using System.Diagnostics;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Windows;
using System.Windows.Threading;

namespace RDPPassword
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const string IconCheckMark = "\ue73e";
        private const string IconCopy = "\ue8c8";
        private Encoding U16LE = Encoding.Unicode;

        private bool gridRootLoaded = false;
        private string currentFileVersion = "";

        private bool UseBase64Mode => CheckBoxBase64.IsChecked == true;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void ShowSelfFileVersion()
        {
            try
            {
                string selfPath = Assembly.GetExecutingAssembly().Location;
                FileVersionInfo fileVersionInfo = FileVersionInfo.GetVersionInfo(selfPath);
                if (fileVersionInfo != null && fileVersionInfo.FileVersion != null)
                {
                    currentFileVersion = fileVersionInfo.FileVersion;
                }
            }
            catch (Exception)
            { }

            TextBlockInfo.Text = currentFileVersion;
        }

        private string EncryptPassword(string strDecrypted)
        {
            string strEncrypted = "";

            try
            {
                byte[] bytesDecrypted = U16LE.GetBytes(strDecrypted);
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
                strDecrypted = U16LE.GetString(bytesDecrypted);
            }
            catch (Exception)
            {
                strDecrypted = "Decryption error.";
            }

            return strDecrypted;
        }

        private string EncryptPasswordBase64(string plainText)
        {
            try
            {
                byte[] bytesDecrypted = U16LE.GetBytes(plainText);
                byte[] bytesEncrypted = ProtectedData.Protect(bytesDecrypted, null, DataProtectionScope.CurrentUser);
                return Convert.ToBase64String(bytesEncrypted);
            }
            catch (Exception)
            {
                return "Encryption error.";
            }
        }

        private string DecryptPasswordBase64(string encodedPassword)
        {
            try
            {
                byte[] bytesEncrypted = Convert.FromBase64String(encodedPassword);
                byte[] bytesDecrypted = ProtectedData.Unprotect(bytesEncrypted, null, DataProtectionScope.CurrentUser);
                return U16LE.GetString(bytesDecrypted).Replace("\0", string.Empty);
            }
            catch (Exception)
            {
                return "Decryption error.";
            }
        }


        private void GridRoot_Loaded(object sender, RoutedEventArgs e)
        {
            gridRootLoaded = true;

            // Fix small buttons
            double buttonHeight = ButtonDecrypt.ActualHeight;
            ButtonClear.Height = buttonHeight;
            ButtonCopy.Height = buttonHeight;

            ShowSelfFileVersion();

            TextBoxMain.Focus();
        }

        private void ButtonClear_Click(object sender, RoutedEventArgs e)
        {
            TextBoxMain.Clear();
            TextBoxMain.Focus();
        }

        private void ButtonCopy_Click(object sender, RoutedEventArgs e)
        {
            string mainText = TextBoxMain.Text;
            mainText = mainText.Trim();
            Clipboard.SetText(mainText);

            TextBoxMain.Focus();

            // Change icon
            IconButtonCopy.Glyph = IconCheckMark;
            DispatcherTimer timer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(3)
            };
            timer.Tick += (s, args) =>
            {
                IconButtonCopy.Glyph = IconCopy;
                timer.Stop();
            };
            timer.Start();
        }

        private void ButtonEncrypt_Click(object sender, RoutedEventArgs e)
        {
            string mainText = TextBoxMain.Text;
            mainText = mainText.Trim();
            string encryptedText = UseBase64Mode ? EncryptPasswordBase64(mainText) : EncryptPassword(mainText);
            TextBoxMain.Text = encryptedText;
        }

        private void ButtonDecrypt_Click(object sender, RoutedEventArgs e)
        {
            string mainText = TextBoxMain.Text;
            mainText = mainText.Trim();
            string decryptedText = UseBase64Mode ? DecryptPasswordBase64(mainText) : DecryptPassword(mainText);
            TextBoxMain.Text = decryptedText;
        }
    }
}
