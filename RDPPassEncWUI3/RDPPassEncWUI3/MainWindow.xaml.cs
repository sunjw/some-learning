using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Security.Cryptography;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Microsoft.UI.Xaml.Media.Animation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace RDPPassEncWUI3
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        private const int appInitWidth = 480;
        private const int appInitHeight = 480;

        private Page currentPage = null;

        public MainWindow()
        {
            InitializeComponent();

            AppWindow.Resize(new(appInitWidth, appInitHeight));

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);
        }

        private void MainFrame_Loaded(object sender, RoutedEventArgs e)
        {
            MainFrame.Navigate(typeof(MainPage));
        }

        private void MainFrame_Navigated(object sender, NavigationEventArgs e)
        {
            currentPage = e.Content as Page;
        }

        public void OnRedirected(string someArgs)
        {
            if (currentPage is MainPage)
            {
                (currentPage as MainPage).OnRedirected(someArgs);
            }
        }
    }
}
