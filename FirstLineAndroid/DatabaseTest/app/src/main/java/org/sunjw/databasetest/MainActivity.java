package org.sunjw.databasetest;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import org.sunjw.MyDatabaseHelper;

public class MainActivity extends AppCompatActivity {

    private MyDatabaseHelper myDbHelper = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        myDbHelper = new MyDatabaseHelper(this, "BookStore.db", null, 2);

        View btCreateDatabase = findViewById(R.id.bt_create_database);
        btCreateDatabase.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                myDbHelper.getWritableDatabase();
            }
        });

        View btAddData = findViewById(R.id.bt_add_data);
        btAddData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SQLiteDatabase db = myDbHelper.getWritableDatabase();
                ContentValues cv = new ContentValues();

                cv.put("name", "The Da Vinci Code");
                cv.put("author", "Dan Brown");
                cv.put("pages", 454);
                cv.put("price", 16.96);
                db.insert("Book", null, cv);

                cv.clear();
                cv.put("name", "The Lost Symbol");
                cv.put("author", "Dan Brown");
                cv.put("pages", 510);
                cv.put("price", 19.95);
                db.insert("Book", null, cv);
            }
        });

        View btUpdateData = findViewById(R.id.bt_update_data);
        btUpdateData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SQLiteDatabase db = myDbHelper.getWritableDatabase();
                ContentValues cv = new ContentValues();
                cv.put("price", 10.99);
                db.update("Book", cv, "name = ?", new String[]{"The Da Vinci Code"});
            }
        });

        View btDeleteData = findViewById(R.id.bt_delete_data);
        btDeleteData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SQLiteDatabase db = myDbHelper.getWritableDatabase();
                db.delete("Book", "pages > ?", new String[]{"500"});
            }
        });

        View btQueryData = findViewById(R.id.bt_query_data);
        btQueryData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SQLiteDatabase db = myDbHelper.getWritableDatabase();
                Cursor cursor = db.query("Book", null, null, null, null, null, null);
                if (cursor.moveToFirst()) {
                    do {
                        String name = cursor.getString(cursor.getColumnIndex("name"));
                        String author = cursor.getString(cursor.getColumnIndex("author"));
                        int pages = cursor.getInt(cursor.getColumnIndex("pages"));
                        double price = cursor.getDouble(cursor.getColumnIndex("price"));

                        Log.d("MainActivity", "Book name=[" + name + "], " +
                                "author=[" + author + "], pages=" + pages + ", " +
                                "price=" + price);
                    } while (cursor.moveToNext());
                }
            }
        });
    }
}
