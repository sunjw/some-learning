package org.sunjw.learnand.ch2.aidl;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Created by sunjw on 16/4/16.
 */
public class Book implements Parcelable {
    public int bookId;
    public String bookName;

    public Book() {

    }

    public Book(int bookId, String bookName) {
        this.bookId = bookId;
        this.bookName = bookName;
    }

    private Book(Parcel in) {
        bookId = in.readInt();
        bookName = in.readString();
    }

    @Override
    public String toString() {
        return "{ bookId(" + bookId + "), bookName(\"" + bookName + "\") }";
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(bookId);
        dest.writeString(bookName);
    }

    public static final Creator<Book> CREATOR = new Creator<Book>() {
        @Override
        public Book createFromParcel(Parcel source) {
            return new Book(source);
        }

        @Override
        public Book[] newArray(int size) {
            return new Book[size];
        }
    };

}
