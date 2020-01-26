package org.sunjw.learnand.ch2.aidl;

import android.os.IBinder;
import android.os.IInterface;
import android.os.RemoteException;

import java.util.List;

/**
 * Created by sunjw on 16/4/19.
 */
public interface IBookManager extends IInterface {

    static final String DESCRIPTOR = "org.sunjw.learnand.ch2.aidl.IBookManager";

    static final int TRANSACTION_getBookList = IBinder.FIRST_CALL_TRANSACTION + 0;
    static final int TRANSACTION_addBook = IBinder.FIRST_CALL_TRANSACTION + 1;

    public List<Book> getBookList() throws RemoteException;

    public void addBook(Book book) throws RemoteException;

}
