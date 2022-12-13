package org.example.shared;

import java.io.Serializable;
public class BookReader implements Serializable {
    private static final long serialVersionUID = 1L;
    private String author;
    private String title;
    private boolean isReader;

    public BookReader() {
    }

    public BookReader(String _author, String _title, boolean _isReader) {
        this.isReader = _isReader;
        this.title = _title;
        this.author = _author;
    }

    public String getAuthor() {
        return author;
    }

    public String getTitle() {
        return title;
    }

    public boolean isReader() {
        return isReader;
    }

    public void setAuthor(String author) {
        this.author = author;
    }
}