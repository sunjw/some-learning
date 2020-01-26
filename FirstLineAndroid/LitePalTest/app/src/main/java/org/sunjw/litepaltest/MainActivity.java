package org.sunjw.litepaltest;

import android.provider.ContactsContract;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import org.litepal.crud.DataSupport;
import org.litepal.tablemanager.Connector;
import org.sunjw.Book;

import java.util.List;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        View btnCreateDatabase = findViewById(R.id.btn_create_database);
        btnCreateDatabase.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Connector.getDatabase();
            }
        });

        View btnAddData = findViewById(R.id.btn_add_data);
        btnAddData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Book book = new Book();
                book.setName("The Da Vinci Code");
                book.setAuthor("Dan Brown");
                book.setPages(454);
                book.setPrice(16.96);
                book.setPress("Unknown");
                book.save();
            }
        });

        View btnUpdateData = findViewById(R.id.btn_update_data);
        btnUpdateData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Book book = new Book();
                book.setName("The Lost Symbol");
                book.setAuthor("Dan Brown");
                book.setPages(510);
                book.setPrice(19.95);
                book.setPress("Unknown");
                book.save();

                Book book1 = new Book();
                book1.setPrice(14.95);
                book1.setPress("Anchor");
                book1.updateAll("name = ? and author = ?", "The Lost Symbol", "Dan Brown");
            }
        });

        View btnDeleteData = findViewById(R.id.btn_delete_data);
        btnDeleteData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DataSupport.deleteAll(Book.class, "price < ?", "15");
            }
        });

        View btnViewData = findViewById(R.id.btn_query_data);
        btnViewData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                List<Book> books = DataSupport.findAll(Book.class);
                for (Book book: books) {
                    Log.d("MainActivity", "Book, id=" + book.getId() + ", " +
                            "name=[" + book.getName() + "], " +
                            "author=[" + book.getAuthor() + "], " +
                            "pages=" + book.getPages() + ", " +
                            "price=" + book.getPrice() + ", " +
                            "press=[" + book.getPress() + "]");
                }
            }
        });
    }
}
