package org.sunjw.learnand.ch2.aidl;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;

import org.sunjw.learnand.ch2.R;

import java.util.List;

public class BookManagerActivity extends ActionBarActivity {

    private static final String TAG = BookManagerActivity.class.getSimpleName();

    private static final int MESSAGE_NEW_BOOK_ARRIVED = 1;

    private IBookManager remoteBookManager = null;

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_NEW_BOOK_ARRIVED:
                    Log.i(TAG, "Received new book: " + msg.obj);
                    break;
            }

            super.handleMessage(msg);
        }
    };

    private IOnNewBookArrivedListener bookArrivedListener = new IOnNewBookArrivedListener.Stub() {
        @Override
        public void onNewBookArrived(Book newBook) throws RemoteException {
            handler.obtainMessage(MESSAGE_NEW_BOOK_ARRIVED, newBook).sendToTarget();
        }
    };

    private ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            remoteBookManager = IBookManager.Stub.asInterface(service);
            try {
                remoteBookManager.registerListener(bookArrivedListener);

                List<Book> list = remoteBookManager.getBookList();
                Log.i(TAG, "Query book list, list type: " + list.getClass().getCanonicalName());
                Log.i(TAG, "Query book list: " + list.toString());

                Book newBook = new Book(3, "Android 开发艺术");
                remoteBookManager.addBook(newBook);

                list = remoteBookManager.getBookList();
                Log.i(TAG, "Query book list: " + list.toString());
            } catch (RemoteException re) {
                re.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            try {
                remoteBookManager.unregisterListener(bookArrivedListener);
            } catch (RemoteException re) {
                re.printStackTrace();
            }
            remoteBookManager = null;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_book_manager);

        Intent intent = new Intent(this, BookManagerService.class);
        bindService(intent, connection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onDestroy() {
        if (remoteBookManager != null &&
                remoteBookManager.asBinder().isBinderAlive()) {
            try {
                remoteBookManager.unregisterListener(bookArrivedListener);
            }  catch (RemoteException re) {
                re.printStackTrace();
            }
        }

        unbindService(connection);

        super.onDestroy();
    }
}
