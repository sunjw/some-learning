﻿using System.Collections.Generic;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Media;
using TestCLRBridge;
using Windows.ApplicationModel;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace RDPPassEncWUI3
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AboutPage : Page
    {
        public delegate void ThreadStoppedHandler();

        private MainWindow m_mainWindow;

        private bool m_textAboutInit = false;
        private Paragraph m_paragraphAbout = new();
        private Hyperlink m_hyperlinkClicked = null;
        private MenuFlyout m_menuFlyoutTextMain;

        private TestManagedClass m_testManagedClass = new("测试 ManagedClass");

        private ThreadStoppedHandler m_threadStopppedHandler = null;

        public bool ThreadRunning { get; private set; } = false;

        public AboutPage()
        {
            InitializeComponent();

            m_mainWindow = MainWindow.CurrentWindow;

            InitMenuFlyoutTextMain();

            m_testManagedClass.SetHWND(m_mainWindow.HWNDHandle);
        }

        private void InitMenuFlyoutTextMain()
        {
            m_menuFlyoutTextMain = new()
            {
                XamlRoot = m_mainWindow.Content.XamlRoot
            };

            MenuFlyoutItem menuItemCopy = new()
            {
                Text = "Copy"
            };
            menuItemCopy.Click += MenuItemCopy_Click;
            MenuFlyoutItem menuItemGoogle = new()
            {
                Text = "Google"
            };
            menuItemGoogle.Click += MenuItemGoogle_Click;

            m_menuFlyoutTextMain.Items.Add(menuItemCopy);
            m_menuFlyoutTextMain.Items.Add(new MenuFlyoutSeparator());
            m_menuFlyoutTextMain.Items.Add(menuItemGoogle);
        }

        public void UpdateDebugString(string strDebug)
        {
            DispatcherQueue.TryEnqueue(() => TextBlockDebug.Text = strDebug);
        }

        public void StopThread(ThreadStoppedHandler threadStoppedHandler)
        {
            m_threadStopppedHandler = threadStoppedHandler;
            m_testManagedClass.StopThread();
        }

        private void UpdateThreadHandler(bool running)
        {
            ThreadRunning = running;
            if (!ThreadRunning && m_threadStopppedHandler != null)
            {
                m_threadStopppedHandler();
            }
        }

        private void UpdateUIHandler(string someStr)
        {
            UpdateDebugString(someStr);
        }

        private Hyperlink GenHyperlinkFromStringForAboutPage(string strContent)
        {
            return WinUIHelper.GenHyperlinkFromString(strContent, TextAboutHyperlink_Click);
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

            string strWindowsInfo = m_testManagedClass.GetWindowsInfo();
            inlinesAbout.Add(WinUIHelper.GenRunFromString(strWindowsInfo));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));

            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));

            // test text
            inlinesAbout.Add(WinUIHelper.GenRunFromString("Name: C:\\Users\\sunj\\Downloads\\"));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("江苏省省道公路网规划（2023—2035 年）.pdf"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("File Size: 2417386 Byte(s) (2.31 MB)"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("Modified Date: 2024-04-11 14:18"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("MD5: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("E44B1734EA01D8C76D2CEABBBA8DF964"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA1: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("A79DE28FB4F6083661467457C13A6A3DC02F154D"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA256: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("74FA39D52397BCA1816DF251504490BCC2F5E987D2947DDF0C4D576CDC1A45AC"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA512: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("317084BEA676D335B0BFAC49421F0510B561CD1D967F1481AF119DFDA51B91EDA41B23D4BC2DFECB7A9CE6531D1412DAB17C805439573C545CCAD1AD737F797A"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));

            inlinesAbout.Add(WinUIHelper.GenRunFromString("文件名: /Users/sunjw/Library/CloudStorage/OneDrive-个人/Apps/fHash/insider/"));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("fHash-3.3.2-macOS.dmg"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("文件大小: 725422 字节 (725.42 KB)"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("修改日期: 2024-06-03 14:02"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("MD5: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("249B290A86B3AB1A6836682BA7C79160"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA1: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("054DD2135CD9397EEBEDFB3E4570C0CC0552E3B3"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA256: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("04F0D0D7BD3F95300F78D20D056331D0D11BA2FF4A4839E6AEF36AE650958CAB"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA512: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("A0E36C44680CC1A8B2FBAD283956300273D23F6693B47D6528792CCA56E69FC5871573CD87B0BDD5C286ABF9166F70346DB5FE81C40564567EBE97975A7AD9AE"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));

            inlinesAbout.Add(WinUIHelper.GenRunFromString("文件名: C:\\Users\\Sun Junwen\\OneDrive\\Apps\\fHash\\insider\\"));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("fHashUwpWap_3.3.1.0_Test.7z"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("文件大小: 83672897 字节 (79.80 MB)"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("修改日期: 2024-04-22 23:06"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("MD5: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("0A0B98B7C370D2DCB611C581AC9C9278"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA1: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("7385065E0B5A74E9F9D0B32FE0C9CF788FB543DD"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA256: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("B37645E34682AE0A2BF87DA2BB5F361ADD919D2BCFE27FEEEC1B2C97DDB60B69"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("SHA512: "));
            inlinesAbout.Add(GenHyperlinkFromStringForAboutPage("DBBFEAE9818B0AB23D4D546FC635F4CC60B2178F9AC19A8FD1C435D905F05B495E16BEBFDE789F40C556A18640719C48B949F1CD8784E67FEBBB233BD1113741"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));
            inlinesAbout.Add(WinUIHelper.GenRunFromString("\r\n"));

            // finish
            foreach (Inline inline in inlinesAbout)
            {
                m_paragraphAbout.Inlines.Add(inline);
            }

            WinUIHelper.ScrollViewerToBottom(ScrollViewerAbout);

            // some test
            m_testManagedClass.UpdateUIHandler += UpdateUIHandler;
            m_testManagedClass.UpdateThreadHandler += UpdateThreadHandler;
            TextBlockDebug.Text = m_testManagedClass.GetTestValue().ToString();

            ThreadRunning = true;
            m_testManagedClass.GoThread();
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

        private void RichTextBlockAbout_SelectionChanged(object sender, RoutedEventArgs e)
        {
            string strDebug = "";

            // cursor position
            double scale = m_mainWindow.Scale;
            System.Drawing.Point pointCursor = m_mainWindow.GetCursorRelativePoint();

            //strDebug = string.Format("{0:0.00} : {1:0.00}", pointCursor.X, pointCursor.Y);
            //TextBlockDebug.Text = strDebug;

            // ScrollView position
            GeneralTransform transformScrollView = ScrollViewerAbout.TransformToVisual(null);
            Windows.Foundation.Point pointScrollView = transformScrollView.TransformPoint(new Windows.Foundation.Point(0, 0));

            // cursor offset relative to ScrollView
            double cursorRelateScrollOffX = pointCursor.X - pointScrollView.X - (ScrollViewerAbout.Margin.Left * scale);
            double cursorRelateScrollOffY = pointCursor.Y - pointScrollView.Y - (ScrollViewerAbout.Margin.Top * scale);

            double scrollViewWidth = ScrollViewerAbout.ActualWidth * scale;
            double scrollViewHeight = ScrollViewerAbout.ActualHeight * scale;

            double cursorOutScrollWidthOffX = cursorRelateScrollOffX;
            if (cursorOutScrollWidthOffX > 0 && cursorOutScrollWidthOffX <= scrollViewWidth)
            {
                // X inside
                cursorOutScrollWidthOffX = 0;
            }
            else if (cursorOutScrollWidthOffX > scrollViewWidth)
            {
                // X outside right
                cursorOutScrollWidthOffX = cursorOutScrollWidthOffX - scrollViewWidth;
            }

            double cursorOutScrollHeightOffY = cursorRelateScrollOffY;
            if (cursorOutScrollHeightOffY > 0 && cursorOutScrollHeightOffY <= scrollViewHeight)
            {
                // Y inside
                cursorOutScrollHeightOffY = 0;
            }
            else if (cursorOutScrollHeightOffY > scrollViewHeight)
            {
                // Y outside right
                cursorOutScrollHeightOffY = cursorOutScrollHeightOffY - scrollViewHeight;
            }

            //strDebug = string.Format("{0:0.00} : {1:0.00}", cursorOutScrollWidthOffX, cursorOutScrollHeightOffY);

            if (cursorOutScrollWidthOffX == 0 && cursorOutScrollHeightOffY == 0)
            {
                // X and Y all inside
                strDebug = string.Format("{0:0.00} : {1:0.00}", cursorOutScrollWidthOffX, cursorOutScrollHeightOffY);
                //TextBlockDebug.Text = strDebug;
                return;
            }

            double scrollViewCurOffX = ScrollViewerAbout.HorizontalOffset;
            double scrollViewCurOffY = ScrollViewerAbout.VerticalOffset;
            double scrollViewNewOffX = scrollViewCurOffX + cursorOutScrollWidthOffX;
            double scrollViewNewOffY = scrollViewCurOffY + cursorOutScrollHeightOffY;

            strDebug = string.Format("{0:0.00} : {1:0.00}", scrollViewNewOffX, scrollViewNewOffY);
            WinUIHelper.ScrollViewerScrollTo(ScrollViewerAbout, scrollViewNewOffX, scrollViewNewOffY);

            //TextBlockDebug.Text = strDebug;
        }

        private void TextAboutHyperlink_Click(Hyperlink sender, HyperlinkClickEventArgs args)
        {
            if (sender == null || m_menuFlyoutTextMain == null)
            {
                return;
            }
            m_hyperlinkClicked = sender;
            double scale = m_mainWindow.Scale;
            int menuOffsetX = 4;
            int menuOffsetY = 2;
            System.Drawing.Point sdPointCursor = m_mainWindow.GetCursorRelativePoint();
            Windows.Foundation.Point wfPointCuror = new((sdPointCursor.X / scale) + menuOffsetX, (sdPointCursor.Y / scale) + menuOffsetY);
            m_menuFlyoutTextMain.ShowAt(null, wfPointCuror);
        }

        private void MenuItemCopy_Click(object sender, RoutedEventArgs e)
        {
            if (m_hyperlinkClicked == null)
            {
                return;
            }
            string strHash = WinUIHelper.GetTextFromHyperlink(m_hyperlinkClicked);
            WinUIHelper.CopyStringToClipboard(strHash);
        }

        private void MenuItemGoogle_Click(object sender, RoutedEventArgs e)
        {
            if (m_hyperlinkClicked == null)
            {
                return;
            }

            string strHash = WinUIHelper.GetTextFromHyperlink(m_hyperlinkClicked);
            string strUrl = string.Format("https://www.google.com/search?q={0}&ie=utf-8&oe=utf-8", strHash);
            WinUIHelper.OpenUrl(strUrl);
        }
    }
}
