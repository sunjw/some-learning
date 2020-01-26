package org.sunjw.learnand.ch2.provider;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.support.annotation.Nullable;
import android.util.Log;

import org.sunjw.learnand.ch2.util.DbOpenHelper;

/**
 * Created by sunjw on 16/5/16.
 */
public class BookProvider extends ContentProvider {

    private static final String TAG = BookProvider.class.getSimpleName();

    public static final String AUTHORITY = "org.sunjw.learnand.ch2.book.provider";

    public static final Uri BOOK_CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/book");
    public static final Uri USER_CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/user");

    public static final int BOOK_URI_CODE = 0;
    public static final int USER_URI_CODE = 1;

    private static final UriMatcher sUriMatcher = new UriMatcher(UriMatcher.NO_MATCH);

    static {
        sUriMatcher.addURI(AUTHORITY, "book", BOOK_URI_CODE);
        sUriMatcher.addURI(AUTHORITY, "user", USER_URI_CODE);
    }

    private Context context;
    private SQLiteDatabase db;

    private String getTableName(Uri uri) {
        String tableName = null;
        switch (sUriMatcher.match(uri)) {
            case BOOK_URI_CODE:
                tableName = DbOpenHelper.BOOK_TABLE_NAME;
                break;
            case USER_URI_CODE:
                tableName = DbOpenHelper.USER_TABLE_NAME;
                break;
        }

        return tableName;
    }

    @Override
    public boolean onCreate() {
        Log.d(TAG, "onCreate, current thread: " + Thread.currentThread().getName());

        context = getContext();
        initProviderData();

        return false;
    }

    private void initProviderData() {
        db = new DbOpenHelper(context).getWritableDatabase();

        db.execSQL("DELETE FROM " + DbOpenHelper.BOOK_TABLE_NAME);
        db.execSQL("DELETE FROM " + DbOpenHelper.USER_TABLE_NAME);

        db.execSQL("INSERT INTO book VALUES (3, 'Android');");
        db.execSQL("INSERT INTO book VALUES (4, 'iOS');");
        db.execSQL("INSERT INTO book VALUES (5, 'HTML5');");
        db.execSQL("INSERT INTO user VALUES (1, 'jake', 1);");
        db.execSQL("INSERT INTO user VALUES (2, 'jasmine', 0);");
    }

    @Nullable
    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder) {
        Log.d(TAG, "query, current thread: " + Thread.currentThread().getName());

        String table = getTableName(uri);
        if (table == null) {
            throw new IllegalArgumentException("Unsupported URI: " + uri);
        }

        return db.query(table, projection, selection, selectionArgs, null, null, sortOrder, null);
    }

    @Nullable
    @Override
    public String getType(Uri uri) {
        Log.d(TAG, "getType");
        return null;
    }

    @Nullable
    @Override
    public Uri insert(Uri uri, ContentValues values) {
        Log.d(TAG, "insert");

        String table = getTableName(uri);
        if (table == null) {
            throw new IllegalArgumentException("Unsupported URI: " + uri);
        }

        db.insert(table, null, values);
        context.getContentResolver().notifyChange(uri, null);

        return uri;
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        Log.d(TAG, "delete");

        String table = getTableName(uri);
        if (table == null) {
            throw new IllegalArgumentException("Unsupported URI: " + uri);
        }

        int count = db.delete(table, selection, selectionArgs);
        if (count > 0) {
            context.getContentResolver().notifyChange(uri, null);
        }

        return count;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
        Log.d(TAG, "update");

        String table = getTableName(uri);
        if (table == null) {
            throw new IllegalArgumentException("Unsupported URI: " + uri);
        }

        int row = db.update(table, values, selection, selectionArgs);
        if (row > 0) {
            context.getContentResolver().notifyChange(uri, null);
        }

        return row;
    }

}
