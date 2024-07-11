using System;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.Windows.AppLifecycle;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.System;

namespace RDPPassEncWUI3
{
    public class WinUIHelper
    {

        public static AppActivationArguments GetCurrentActivatedEventArgs()
        {
            return AppInstance.GetCurrent().GetActivatedEventArgs();
        }

        public static string GetLaunchActivatedEventArgs(AppActivationArguments appActivationArgs)
        {
            string launchArgs = null;
            if (appActivationArgs.Kind == ExtendedActivationKind.Launch)
            {
                ILaunchActivatedEventArgs launchActivatedEventArgs = appActivationArgs.Data as ILaunchActivatedEventArgs;
                launchArgs = launchActivatedEventArgs?.Arguments;
            }
            return launchArgs;
        }

        public static void ScrollViewerScrollTo(ScrollViewer scrollViewer, double offsetX, double offsetY)
        {
            if (offsetX < 0)
            {
               offsetX = 0;
            }
            if (offsetY < 0)
            {
               offsetY = 0;
            }
            if (offsetX > scrollViewer.ScrollableWidth)
            {
               offsetX = scrollViewer.ScrollableWidth;
            }
            if (offsetY > scrollViewer.ScrollableHeight)
            {
               offsetY = scrollViewer.ScrollableHeight;
            }
            scrollViewer.ChangeView(offsetX, offsetY, null);
        }


        public static void ScrollViewerToBottom(ScrollViewer scrollViewer)
        {
            scrollViewer.Measure(scrollViewer.RenderSize); // must Measure now
            scrollViewer.ChangeView(null, scrollViewer.ScrollableHeight, null);
        }

        public static void CopyStringToClipboard(string text)
        {
            DataPackage dataPackage = new DataPackage { RequestedOperation = DataPackageOperation.Copy };
            dataPackage.SetText(text);
            Clipboard.SetContent(dataPackage);
            Clipboard.Flush();
        }

        public static void OpenUrl(string url)
        {
            Uri uri = new Uri(url);
            _ = Launcher.LaunchUriAsync(uri);
        }

        public static Run GenRunFromString(string strContent)
        {
            Run run = new Run();
            run.Text = strContent;
            return run;
        }

        public static Hyperlink GenHyperlinkFromString(string strContent, TypedEventHandler<Hyperlink, HyperlinkClickEventArgs> clickHandler)
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

        public static string GetTextFromHyperlink(Hyperlink hyperlink)
        {
            if (hyperlink.Inlines.Count == 0)
            {
                return null;
            }
            Run run = (Run)hyperlink.Inlines[0];
            return run.Text;
        }
    }
}
