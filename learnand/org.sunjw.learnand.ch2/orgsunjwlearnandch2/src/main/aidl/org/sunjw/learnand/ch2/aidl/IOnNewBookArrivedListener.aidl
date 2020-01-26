// IOnNewBookArrivedListener.aidl
package org.sunjw.learnand.ch2.aidl;

import org.sunjw.learnand.ch2.aidl.Book;

interface IOnNewBookArrivedListener {
    void onNewBookArrived(in Book newBook);
}
