using System.Diagnostics;
using System.Reflection;
using System.Windows;

namespace RDPPassword
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
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

            ShowSelfFileVersion();
        }
    }
}
