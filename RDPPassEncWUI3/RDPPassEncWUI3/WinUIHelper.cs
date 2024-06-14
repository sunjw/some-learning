using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Documents;
using Microsoft.Windows.AppLifecycle;
using Windows.ApplicationModel.Activation;

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

        public static Run GenRunFromString(string strContent)
        {
            Run run = new Run();
            run.Text = strContent;
            return run;
        }
    }
}
