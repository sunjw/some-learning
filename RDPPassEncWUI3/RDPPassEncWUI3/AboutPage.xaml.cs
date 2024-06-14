using System.Collections.Generic;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Media;
using Windows.ApplicationModel;
using System.Drawing;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace RDPPassEncWUI3
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AboutPage : Page
    {
        private bool m_textAboutInit = false;
        private Paragraph m_paragraphAbout = new Paragraph();

        public AboutPage()
        {
            InitializeComponent();
        }

        private void GridRoot_Loaded(object sender, RoutedEventArgs e)
        {
            if (m_textAboutInit)
            {
                return;
            }

            m_textAboutInit = true;

            m_paragraphAbout.FontFamily = new FontFamily("Consolas");
            m_paragraphAbout.LineHeight = 18;
            m_paragraphAbout.LineStackingStrategy = LineStackingStrategy.BlockLineHeight;

            RichTextBlockAbout.TextWrapping = TextWrapping.NoWrap;
            RichTextBlockAbout.Blocks.Clear();
            RichTextBlockAbout.Blocks.Add(m_paragraphAbout);

            List<Inline> inlinesAbout = new List<Inline>();

            // version
            PackageVersion packVersion = Package.Current.Id.Version;
            string strVersion = string.Format("{0}.{1}.{2}.{3}",
                packVersion.Major, packVersion.Minor, packVersion.Build, packVersion.Revision);
            strVersion = "RDPPassEncWUI3: " + strVersion;
            inlinesAbout.Add(WinUIHelper.GenRunFromString(strVersion));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));

            // test text
            inlinesAbout.Add(WinUIHelper.GenRunFromString("Name: C:\\Users\\sunj\\Downloads\\江苏省省道公路网规划（2023—2035 年）.pdf"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));

            // finish
            foreach (Inline inline in inlinesAbout)
            {
                m_paragraphAbout.Inlines.Add(inline);
            }
        }

        private void RichTextBlockAbout_SelectionChanged(object sender, RoutedEventArgs e)
        {
            string strDebug = "";

            // cursor position
            MainWindow mainWindow = MainWindow.CurrentWindow;
            if (mainWindow == null)
            {
                return;
            }
            Point pointCursor = mainWindow.GetCursorRelativePoint();

            // ScrollView position
            GeneralTransform transformScrollView = ScrollViewAbout.TransformToVisual(null);
            Windows.Foundation.Point pointScrollView = transformScrollView.TransformPoint(new Windows.Foundation.Point(0, 0));

            // cursor offset relative to ScrollView
            double cursorRelateScrollOffX = pointCursor.X - pointScrollView.X;
            double cursorRelateScrollOffY = pointCursor.Y - pointScrollView.Y;

            strDebug = string.Format("{0:0.00} : {1:0.00}", cursorRelateScrollOffX, cursorRelateScrollOffY);

            TextBlockDebug.Text = strDebug;
        }

        private void ButtonClose_Click(object sender, RoutedEventArgs e)
        {
            //if (Frame.CanGoBack)
            //{
            //    // Frame.GoBack(new SuppressNavigationTransitionInfo());
            //    Frame.GoBack();
            //}
            Frame.Navigate(typeof(MainPage));
        }

        public void UpdateDebugString(string strDebug)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                //TextBlockDebug.Text = strDebug;
            });
        }
    }
}
