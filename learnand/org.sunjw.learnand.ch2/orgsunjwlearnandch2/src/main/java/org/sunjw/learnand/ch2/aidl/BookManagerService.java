package org.sunjw.learnand.ch2.aidl;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.support.annotation.Nullable;
import android.util.Log;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by sunjw on 16/5/14.
 */
public class BookManagerService extends Service {

    private static final String TAG = BookManagerService.class.getSimpleName();

    private class ServiceWorker implements Runnable {
        @Override
        public void run() {
            while (!isServiceDestroyed.get()) {
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException ie) {

                }

                int bookId = bookList.size() + 1;
                Book newBook = new Book(bookId, "new book#" + bookId);
                addBookToList(newBook);
            }
        }
    }

    private AtomicBoolean isServiceDestroyed = new AtomicBoolean(false);

    private CopyOnWriteArrayList<Book> bookList = new CopyOnWriteArrayList<Book>();

    private RemoteCallbackList<IOnNewBookArrivedListener> listenerList =
            new RemoteCallbackList<IOnNewBookArrivedListener>();

    private Binder binder = new IBookManager.Stub() {
        @Override
        public List<Book> getBookList() throws RemoteException {
            Log.i(TAG, "getBookList");
            return bookList;
        }

        @Override
        public void addBook(Book book) throws RemoteException {
            Log.i(TAG, "addBook");
            addBookToList(book);
        }

        @Override
        public void registerListener(IOnNewBookArrivedListener listener) throws RemoteException {
            if (listenerList.register(listener)) {
                Log.i(TAG, listener + "registered");
            } else {
                Log.i(TAG, "Not registered");
            }
        }

        @Override
        public void unregisterListener(IOnNewBookArrivedListener listener) throws RemoteException {
            if (listenerList.unregister(listener)) {
                Log.i(TAG, "Unregistered");
            } else {
                Log.i(TAG, "Not unregistered");
            }
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();

        Log.i(TAG, "onCreate");

        bookList.add(new Book(1, "Android"));
        bookList.add(new Book(2, "iOS"));

        new Thread(new ServiceWorker()).start();
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "onDestroy");

        isServiceDestroyed.set(true);

        super.onDestroy();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    private void addBookToList(Book book) {
        bookList.add(book);

        try {
            onNewBookArrived(book);
        } catch (RemoteException re) {
            re.printStackTrace();
        }
    }

    private void onNewBookArrived(Book book) throws  RemoteException {
        final int count = listenerList.beginBroadcast();
        Log.i(TAG, "onNewBookArrived, notify listeners: " + count);

        for (int i = 0; i < count; ++i) {
            IOnNewBookArrivedListener listener = listenerList.getBroadcastItem(i);
            listener.onNewBookArrived(book);
        }

        listenerList.finishBroadcast();
    }
}
