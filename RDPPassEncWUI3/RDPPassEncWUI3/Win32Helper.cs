using System;
using System.Drawing;
using Windows.Win32;
using Windows.Win32.Foundation;

namespace RDPPassEncWUI3
{
    public class Win32Helper
    {
        public static unsafe bool IsAppPackaged()
        {
            uint bufferLength = 0;
            WIN32_ERROR result = PInvoke.GetCurrentPackageId(ref bufferLength, null);
            bool isPackaged = true;
            if (result == WIN32_ERROR.APPMODEL_ERROR_NO_PACKAGE)
            {
                isPackaged = false;
            }
            return isPackaged;
        }

        public static double GetScaleFactor(IntPtr hWnd)
        {
            double dpi = PInvoke.GetDpiForWindow(new HWND(hWnd));
            return dpi / 96.0;
        }

        public static int GetScaledPixel(int pixel, double scale)
        {
            return (int)(pixel * scale);
        }

        public static Point GetPointerPoint()
        {
            Point pointCursor = new Point();
            PInvoke.GetCursorPos(out pointCursor);
            return pointCursor;
        }
    }
}
