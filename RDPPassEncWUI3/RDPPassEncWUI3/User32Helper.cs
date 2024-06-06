using System;
using System.Runtime.InteropServices;

namespace RDPPassEncWUI3
{
    public class User32Helper
    {
        [DllImport("User32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        public static extern int GetDpiForWindow(IntPtr hwnd);
    }
}
