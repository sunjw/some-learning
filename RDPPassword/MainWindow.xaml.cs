using System.Diagnostics;
using System.Reflection;
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

        private bool gridRootLoaded = false;
        private string currentFileVersion = "";

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
    }
}
