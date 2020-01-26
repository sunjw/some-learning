package org.sunjw.learnand.ch2.util;

import android.os.Environment;

import java.io.File;

/**
 * Created by sunjw on 16/4/20.
 */
public class Ch2Util {

    static public final int MSG_FROM_CLIENT = 0x0001;
    static public final int MSG_FROM_SERVER = 0x0002;

    static public final String PROVIDER_URI = "content://org.sunjw.learnand.ch2.book.provider";

    static public String getDataStoreDir() {
        File externalDir = Environment.getExternalStorageDirectory();
        String path = externalDir.getAbsolutePath();

        return path + "/learnand/";
    }

    static public String getDataStoreFile() {
        String strDir = getDataStoreDir();

        return strDir + "ch2.data";
    }
}
