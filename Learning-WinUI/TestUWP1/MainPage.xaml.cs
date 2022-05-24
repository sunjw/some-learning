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

        private UISettings m_uiSettings;
        private long m_tokenThemeChanged;
        private Thickness m_imageAppIconMargin;

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

        private void ColorValuesChanged(Windows.UI.ViewManagement.UISettings sender, object e)
        {
            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() => {
                UpdateTitleBarColor(); 
            }));
        }

        private void RequestedThemeChanged(DependencyObject sender, DependencyProperty dp)
        {
            if (RequestedThemeProperty == dp)
            {
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() => {
                    UpdateTitleBarColor();
                }));
            }
        }

        private void WindowSizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            UpdatePopupAboutSize();
        }

        private void GridRoot_Loaded(object sender, RoutedEventArgs e)
        {
            // Theme changed callback
            m_uiSettings = new UISettings();
            Frame rootFrame = (Frame)Window.Current.Content;
            m_uiSettings.ColorValuesChanged += ColorValuesChanged;
            // RequestedThemeProperty seems not work at all...
            m_tokenThemeChanged = rootFrame.RegisterPropertyChangedCallback(RequestedThemeProperty, RequestedThemeChanged);

            // Init ui
            RichTextMain.TextWrapping = TextWrapping.NoWrap;

            // Test content
            InitTestContent();
        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            if (ProgressBarMain.Value < 100)
            {
                ProgressBarMain.Value += 10;
            }
        }

        private void ButtonAbout_Click(object sender, RoutedEventArgs e)
        {
            ShowPopupAbout();
        }

        private void InitTestContent()
        {
            Paragraph paragraph = new Paragraph();

            Run run1 = new Run();
            string strRun1 = "文件名: C:\\Users\\Sun Junwen\\OneDrive\\Apps\\fHash\\fHash64-2.3.0-win64.zip\r\n";
            strRun1 += "文件大小: 383692 字节(374.70 KB)\r\n";
            strRun1 += "修改日期: 2022-03-28 15:17\r\n";
            strRun1 += "MD5: c0232b5bc37e70eebad95fbfc6499d92\r\n";
            strRun1 += "SHA1: 14017068424f5ca356b9440cb025eb0f848e08d9\r\n";
            strRun1 += "SHA256: ce25ecf0650ebfa1f8f2d8ebc557ad3db0cd75e80bda647c59314668e0281a35\r\n";
            strRun1 += "SHA512: aeda1930fc0ae1feda19b68170b78074f4a408ec50080256110dd0d9eda005abaa7167ae6d62ca302f6995f60f3d038af6c21667ea922e1206bb3670bc1c5e71\r\n";
            strRun1 += "\r\n";
            run1.Text = strRun1;
            paragraph.Inlines.Add(run1);

            Run run2 = new Run();
            string strRun2 = "Name: /Users/sunjw/Library/CloudStorage/OneDrive-个人/Apps/fHash/fHash-2.3.0-macOS.dmg\r\n";
            strRun2 += "File Size: 656793 Byte(s)(656.79 KB)\r\n";
            strRun2 += "Modified Date: 2022-03-28 14:58\r\n";
            strRun2 += "MD5: 1C880E1191F884EB225F9CFA36CDC355\r\n";
            strRun2 += "SHA1: 4528265C32082A3FB115C05CB29FA36E96A942E5\r\n";
            strRun2 += "SHA256: B704FD5E9E6818F1AC8E7EBECF97F0A56E1FDB8DB9AB9E01FE0F6B574A586FEF\r\n";
            strRun2 += "SHA512: 15F634DF65D0A41E943A9024CA69FF5C31E4B05446180FE5900B7CB0068CFF9DF2820EFD3F8AFCB48924C3FE45D0113FDC8E81F5E4E6CDD60830BE4581F8B11C\r\n";
            strRun2 += "\r\n";
            run2.Text = strRun2;
            paragraph.Inlines.Add(run2);

            Run run3 = new Run();
            string strRun3 = "文件名: C:\\Users\\Sun Junwen\\OneDrive\\新建文件夹\\jstool\\JSMinNPP.dll\r\n";
            strRun3 += "文件大小: 432640 字节(422.50 KB)\r\n";
            strRun3 += "修改日期: 2022-05-24 12:58\r\n";
            strRun3 += "版本: 1.2205.0.0\r\n";
            strRun3 += "MD5: 68373918248f972c784cf6fd921670dd\r\n";
            strRun3 += "SHA1: 9a0acfb02d983bc1feaf0364cd04884dd554c47d\r\n";
            strRun3 += "SHA256: f828b4973c72721b78aafeb9caa82362465ca95ebb8a296f39ef3c45aa8f0d10\r\n";
            strRun3 += "SHA512: e2f8f248b0dbada7799f02eeacccd53ca4d5ef4296504e6826e72cb6aa1259ad61d15c222f9ea1638d06990328a433e3dc871761fc38aef22dfa7bc786145213\r\n";
            strRun3 += "\r\n";
            run3.Text = strRun3;
            paragraph.Inlines.Add(run3);

            RichTextMain.Blocks.Add(paragraph);
        }

    }
}
