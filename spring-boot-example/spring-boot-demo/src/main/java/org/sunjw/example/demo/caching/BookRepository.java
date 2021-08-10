package org.sunjw.example.demo.caching;

public interface BookRepository {

    Book getByIsbn(String isbn);
}
