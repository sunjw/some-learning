package org.sunjw.learnand.ch2.provider;

import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;

import org.sunjw.learnand.ch2.R;
import org.sunjw.learnand.ch2.aidl.Book;
import org.sunjw.learnand.ch2.util.User;

public class ProviderActivity extends ActionBarActivity {

    private static final String TAG = ProviderActivity.class.getName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_provider);

        Uri bookUri = BookProvider.BOOK_CONTENT_URI;

        ContentValues values = new ContentValues();
        values.put("_id", 6);
        values.put("name", "Art of Design");

        getContentResolver().insert(bookUri, values);

        Cursor bookCursor = getContentResolver().query(bookUri, new String[]{"_id", "name"},
                null, null, null);
        while (bookCursor.moveToNext()) {
            Book book = new Book();
            book.bookId = bookCursor.getInt(0);
            book.bookName = bookCursor.getString(1);

            Log.d(TAG, "queried book: " + book.toString());
        }
        bookCursor.close();

        Uri userUri = BookProvider.USER_CONTENT_URI;
        Cursor userCursor = getContentResolver().query(userUri, new String[]{"_id", "name", "sex"},
                null, null, null);
        while (userCursor.moveToNext()) {
            User user = new User();
            user.userId = userCursor.getInt(0);
            user.userName = userCursor.getString(1);
            user.isMale = (userCursor.getInt(2) == 1);

            Log.d(TAG, "queried user: " + user.toString());
        }
        userCursor.close();

    }
}
