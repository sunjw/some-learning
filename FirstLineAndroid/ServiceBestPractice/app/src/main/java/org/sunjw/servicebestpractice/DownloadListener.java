package org.sunjw.servicebestpractice;

/**
 * Created by sunjw on 2017/8/3.
 */

public interface DownloadListener {

    void onProgress(int progress);

    void onSuccess();

    void onFailed();

    void onPaused();

    void onCancel();

}
