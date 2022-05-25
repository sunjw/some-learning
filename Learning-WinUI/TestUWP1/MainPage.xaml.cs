using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

namespace TestUWP1
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private CoreApplicationViewTitleBar m_coreAppViewTitleBar;
        private ApplicationViewTitleBar m_appViewTitleBar;
        private MenuFlyout m_menuFlyoutTextMain;

        private UISettings m_uiSettings;
        private long m_tokenThemeChanged;
        private Thickness m_imageAppIconMargin;

        private Paragraph m_paragraphMain;
        private int m_testCount = 0;

        public MainPage()
        {
            InitializeComponent();

            m_coreAppViewTitleBar = CoreApplication.GetCurrentView().TitleBar;
            m_appViewTitleBar = ApplicationView.GetForCurrentView().TitleBar;

            ApplicationView.PreferredLaunchViewSize = new Size(640, 400);
            ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.PreferredLaunchViewSize;

            Window.Current.SizeChanged += WindowSizeChanged;

            InitCustomTitleBar();

            m_imageAppIconMargin = ImageAppIcon.Margin;

            m_menuFlyoutTextMain = new MenuFlyout();
            MenuFlyoutItem menuItemCopy = new MenuFlyoutItem();
            menuItemCopy.Text = "Copy";
            MenuFlyoutItem menuItemGoogle = new MenuFlyoutItem();
            menuItemGoogle.Text = "Google";
            m_menuFlyoutTextMain.Items.Add(menuItemCopy);
            m_menuFlyoutTextMain.Items.Add(menuItemGoogle);
        }

        private UIElement GetRootFrame()
        {
            return Window.Current.Content;
        }

        private void ChangeThemeLight()
        {
            ChangeTheme(ElementTheme.Light);
        }

        private void ChangeTheme(ElementTheme theme)
        {
            FrameworkElement root = (FrameworkElement)GetRootFrame();
            root.RequestedTheme = theme;
            UpdateControlColor();
        }

        private void UpdateControlColor()
        {
            UpdateTitleBarColor();
        }

        private void InitCustomTitleBar()
        {
            m_coreAppViewTitleBar.ExtendViewIntoTitleBar = true;
            Window.Current.SetTitleBar(GridTitleBarCustom);

            UpdateTitleBarColor();
        }

        private void UpdateTitleBarColor()
        {
            if (m_appViewTitleBar == null)
            {
                return;
            }

            Color bgColor = Colors.Transparent;
            Color fgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlPageTextBaseHighBrush"]).Color;
            Color inactivefgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundChromeDisabledLowBrush"]).Color;
            Color hoverbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListLowBrush"]).Color;
            Color hoverfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
            Color pressedbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListMediumBrush"]).Color;
            Color pressedfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
            m_appViewTitleBar.ButtonBackgroundColor = bgColor;
            m_appViewTitleBar.ButtonForegroundColor = fgColor;
            m_appViewTitleBar.ButtonInactiveBackgroundColor = bgColor;
            m_appViewTitleBar.ButtonInactiveForegroundColor = inactivefgColor;
            m_appViewTitleBar.ButtonHoverBackgroundColor = hoverbgColor;
            m_appViewTitleBar.ButtonHoverForegroundColor = hoverfgColor;
            m_appViewTitleBar.ButtonPressedBackgroundColor = pressedbgColor;
            m_appViewTitleBar.ButtonPressedForegroundColor = pressedfgColor;
        }

        private void ShowPopupAbout()
        {
            UpdatePopupAboutSize();

            if (!PopupAbout.IsOpen)
            {
                GridMain.Visibility = Visibility.Collapsed;
                ImageAppIcon.Margin = new Thickness(48, 0, 0, 0);
                PopupAbout.IsOpen = true;
            }
        }

        public void HidePopupAbout()
        {
            if (PopupAbout.IsOpen)
            {
                PopupAbout.IsOpen = false;
                ImageAppIcon.Margin = m_imageAppIconMargin;
                GridMain.Visibility = Visibility.Visible;
            }
        }

        private void UpdatePopupAboutSize()
        {
            if (PopupAboutContent == null)
            {
                FindName("PopupAboutContent");
            }

            Rect windowBounds = Window.Current.Bounds;
            PopupAboutContent.Width = windowBounds.Width;
            PopupAboutContent.Height = windowBounds.Height;
        }

        private void ScrollTextMainToBottom()
        {
            ScrollViewerMain.Measure(ScrollViewerMain.RenderSize);
            ScrollViewerMain.ChangeView(null, ScrollViewerMain.ScrollableHeight, null);
        }

        private Point GetCursorPointRelatedToRootFrame()
        {
            Point cursorPoint = Window.Current.CoreWindow.PointerPosition;
            Rect windowBount = Window.Current.Bounds;
            Point relativePoint = new Point(cursorPoint.X - windowBount.X, cursorPoint.Y - windowBount.Y);
            return relativePoint;
        }

        private Run GenRunFromString(string strContent)
        {
            Run run = new Run();
            run.Text = strContent;
            return run;
        }

        private Underline GenUnderlineFromString(string strContent)
        {
            Underline underline = new Underline();
            underline.Inlines.Add(GenRunFromString(strContent));
            return underline;
        }

        private Hyperlink GenHyperlinkFromString(string strContent, TypedEventHandler<Hyperlink, HyperlinkClickEventArgs> clickHandler)
        {
            Hyperlink hyperlink = new Hyperlink();
            hyperlink.Inlines.Add(GenRunFromString(strContent));
            hyperlink.UnderlineStyle = UnderlineStyle.None;
            if (clickHandler != null)
            {
                hyperlink.Click += clickHandler;
            }
            return hyperlink;
        }

        private Hyperlink GenHyperlinkFromStringForTextMain(string strContent)
        {
            return GenHyperlinkFromString(strContent, TextMainHyperlink_Click);
        }

        private void InitContent()
        {
            m_paragraphMain = new Paragraph();
            m_paragraphMain.FontFamily = new FontFamily("Consolas");

            Run runInit = new Run();
            runInit.Text = "将文件拖入或点击打开，开始计算。";
            m_paragraphMain.Inlines.Add(runInit);

            RichTextMain.Blocks.Add(m_paragraphMain);
        }

        private void DoTest1()
        {
            Span span1 = new Span();

            span1.Inlines.Add(GenRunFromString("文件名: C:\\Users\\Sun Junwen\\OneDrive\\Apps\\fHash\\fHash64-2.3.0-win64.zip\r\n"));
            span1.Inlines.Add(GenRunFromString("文件大小: 383692 字节(374.70 KB)\r\n"));
            span1.Inlines.Add(GenRunFromString("修改日期: 2022-03-28 15:17\r\n"));
            span1.Inlines.Add(GenRunFromString("MD5: "));
            span1.Inlines.Add(GenHyperlinkFromStringForTextMain("c0232b5bc37e70eebad95fbfc6499d92"));
            span1.Inlines.Add(GenRunFromString("\r\n"));
            span1.Inlines.Add(GenRunFromString("SHA1: "));
            span1.Inlines.Add(GenHyperlinkFromStringForTextMain("14017068424f5ca356b9440cb025eb0f848e08d9"));
            span1.Inlines.Add(GenRunFromString("\r\n"));
            span1.Inlines.Add(GenRunFromString("SHA256: "));
            span1.Inlines.Add(GenHyperlinkFromStringForTextMain("ce25ecf0650ebfa1f8f2d8ebc557ad3db0cd75e80bda647c59314668e0281a35"));
            span1.Inlines.Add(GenRunFromString("\r\n"));
            span1.Inlines.Add(GenRunFromString("SHA512: "));
            span1.Inlines.Add(GenHyperlinkFromStringForTextMain("aeda1930fc0ae1feda19b68170b78074f4a408ec50080256110dd0d9eda005abaa7167ae6d62ca302f6995f60f3d038af6c21667ea922e1206bb3670bc1c5e71"));
            span1.Inlines.Add(GenRunFromString("\r\n\r\n"));

            m_paragraphMain.Inlines.Clear();
            m_paragraphMain.Inlines.Add(span1);
            ScrollTextMainToBottom();
            ProgressBarMain.Value = 30;
        }

        private void DoTest2()
        {
            Span span2 = new Span();

            span2.Inlines.Add(GenRunFromString("Name: /Users/sunjw/Library/CloudStorage/OneDrive-个人/Apps/fHash/fHash-2.3.0-macOS.dmg\r\n"));
            span2.Inlines.Add(GenRunFromString("File Size: 656793 Byte(s)(656.79 KB)\r\n"));
            span2.Inlines.Add(GenRunFromString("Modified Date: 2022-03-28 14:58\r\n"));
            span2.Inlines.Add(GenRunFromString("MD5: "));
            span2.Inlines.Add(GenHyperlinkFromStringForTextMain("1C880E1191F884EB225F9CFA36CDC355"));
            span2.Inlines.Add(GenRunFromString("\r\n"));
            span2.Inlines.Add(GenRunFromString("SHA1: "));
            span2.Inlines.Add(GenHyperlinkFromStringForTextMain("4528265C32082A3FB115C05CB29FA36E96A942E5"));
            span2.Inlines.Add(GenRunFromString("\r\n"));
            span2.Inlines.Add(GenRunFromString("SHA256: "));
            span2.Inlines.Add(GenHyperlinkFromStringForTextMain("B704FD5E9E6818F1AC8E7EBECF97F0A56E1FDB8DB9AB9E01FE0F6B574A586FEF"));
            span2.Inlines.Add(GenRunFromString("\r\n"));
            span2.Inlines.Add(GenRunFromString("SHA512: "));
            span2.Inlines.Add(GenHyperlinkFromStringForTextMain("15F634DF65D0A41E943A9024CA69FF5C31E4B05446180FE5900B7CB0068CFF9DF2820EFD3F8AFCB48924C3FE45D0113FDC8E81F5E4E6CDD60830BE4581F8B11C"));
            span2.Inlines.Add(GenRunFromString("\r\n\r\n"));

            m_paragraphMain.Inlines.Add(span2);
            ScrollTextMainToBottom();
            ProgressBarMain.Value = 60;
        }

        private void DoTest3()
        {
            Span span3 = new Span();

            span3.Inlines.Add(GenRunFromString("文件名: C:\\Users\\Sun Junwen\\OneDrive\\新建文件夹\\jstool\\JSMinNPP.dll\r\n"));
            span3.Inlines.Add(GenRunFromString("文件大小: 432640 字节(422.50 KB)\r\n"));
            span3.Inlines.Add(GenRunFromString("修改日期: 2022-05-24 12:58\r\n"));
            span3.Inlines.Add(GenRunFromString("版本: 1.2205.0.0\r\n"));
            span3.Inlines.Add(GenRunFromString("MD5: "));
            span3.Inlines.Add(GenHyperlinkFromStringForTextMain("68373918248f972c784cf6fd921670dd"));
            span3.Inlines.Add(GenRunFromString("\r\n"));
            span3.Inlines.Add(GenRunFromString("SHA1: "));
            span3.Inlines.Add(GenHyperlinkFromStringForTextMain("9a0acfb02d983bc1feaf0364cd04884dd554c47d"));
            span3.Inlines.Add(GenRunFromString("\r\n"));
            span3.Inlines.Add(GenRunFromString("SHA256: "));
            span3.Inlines.Add(GenHyperlinkFromStringForTextMain("f828b4973c72721b78aafeb9caa82362465ca95ebb8a296f39ef3c45aa8f0d10"));
            span3.Inlines.Add(GenRunFromString("\r\n"));
            span3.Inlines.Add(GenRunFromString("SHA512: "));
            span3.Inlines.Add(GenHyperlinkFromStringForTextMain("e2f8f248b0dbada7799f02eeacccd53ca4d5ef4296504e6826e72cb6aa1259ad61d15c222f9ea1638d06990328a433e3dc871761fc38aef22dfa7bc786145213"));
            span3.Inlines.Add(GenRunFromString("\r\n\r\n"));

            m_paragraphMain.Inlines.Add(span3);
            ScrollTextMainToBottom();
            ProgressBarMain.Value = 100;
        }

        private void ColorValuesChanged(UISettings sender, object e)
        {
            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() => {
                UpdateControlColor();
            }));
        }

        private void RequestedThemeChanged(DependencyObject sender, DependencyProperty dp)
        {
            if (RequestedThemeProperty == dp)
            {
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() => {
                    UpdateControlColor();
                }));
            }
        }

        private void WindowSizeChanged(object sender, WindowSizeChangedEventArgs e)
        {
            UpdatePopupAboutSize();
        }

        private void GridRoot_Loaded(object sender, RoutedEventArgs e)
        {
            // Theme changed callback
            m_uiSettings = new UISettings();
            Frame rootFrame = (Frame)GetRootFrame();
            m_uiSettings.ColorValuesChanged += ColorValuesChanged;
            // RequestedThemeProperty seems not work at all...
            m_tokenThemeChanged = rootFrame.RegisterPropertyChangedCallback(RequestedThemeProperty, RequestedThemeChanged);

            // Init ui
            RichTextMain.TextWrapping = TextWrapping.NoWrap;

            // Init content
            InitContent();
        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            switch (m_testCount)
            {
                case 0:
                    DoTest1();
                    break;
                case 1:
                    DoTest2();
                    break;
                case 2:
                    DoTest3();
                    break;
            }

            m_testCount++;
        }

        private void ButtonAbout_Click(object sender, RoutedEventArgs e)
        {
            ShowPopupAbout();
        }

        private void TextMainHyperlink_Click(Hyperlink sender, HyperlinkClickEventArgs args)
        {
            m_menuFlyoutTextMain.ShowAt(GetRootFrame(), GetCursorPointRelatedToRootFrame());
        }

    }
}
