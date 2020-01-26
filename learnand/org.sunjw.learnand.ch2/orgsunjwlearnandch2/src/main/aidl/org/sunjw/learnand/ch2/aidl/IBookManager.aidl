// IBookManager.aidl
package org.sunjw.learnand.ch2.aidl;

import org.sunjw.learnand.ch2.aidl.Book;
import org.sunjw.learnand.ch2.aidl.IOnNewBookArrivedListener;

interface IBookManager {
    List<Book> getBookList();
    void addBook(in Book book);
    void registerListener(IOnNewBookArrivedListener listener);
    void unregisterListener(IOnNewBookArrivedListener listener);
}
