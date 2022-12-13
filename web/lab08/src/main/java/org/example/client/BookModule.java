package org.example.client;

import com.google.gwt.user.cellview.client.CellTable;
import com.google.gwt.user.cellview.client.HasKeyboardSelectionPolicy;
import com.google.gwt.user.cellview.client.TextColumn;
import com.google.gwt.user.client.ui.*;
import org.example.shared.BookReader;
import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.GWT;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.event.dom.client.KeyUpHandler;
import com.google.gwt.user.client.rpc.AsyncCallback;

import java.util.List;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class BookModule implements EntryPoint {
  /**
   * The message displayed to the user when the server cannot be reached or
   * returns an error.
   */
  private static final String SERVER_ERROR = "An error occurred while "
      + "attempting to contact the server. Please check your network "
      + "connection and try again.";

  /**
   * Create a remote service proxy to talk to the server-side Greeting service.
   */
  private final GreetingServiceAsync greetingService = GWT.create(GreetingService.class);

  private final Messages messages = GWT.create(Messages.class);

  private static final String SRV_ERR = "Ошибка сервера! ";
  private static final String SRV_ERR_GET_READER_LIST = "Невозможно получить список читателей.";
  private static final String SRV_ERR_GET_BOOK_LIST = "Невозможно получить список книг читателя.";
  private static final String GET_READER_LIST_BTN = "Получить список книг.";
  private static final String CLOSE_BTN = "Закрыть";
  private static final String BOOKS_WND_TITLE = "Список книг читателя ";


  /**
   * This is the entry point method.
   */
  public void onModuleLoad() {
    final ListBox readerListBox = new ListBox(false);
    final MultiWordSuggestOracle oracle = new MultiWordSuggestOracle();
    final Label errorLabel = new Label();
    final Button sendButton = new Button(GET_READER_LIST_BTN);
    sendButton.addStyleName("sendButton");
    RootPanel.get("readerListBoxContainer").add(readerListBox);
    RootPanel.get("errorLabelContainer").add(errorLabel);
    RootPanel.get("sendButtonContainer").add(sendButton);
    readerListBox.setFocus(true);
    greetingService.getReaderList(new AsyncCallback<List<String>>() {
      public void onFailure(Throwable caught) {
        errorLabel.setText (SRV_ERR+SRV_ERR_GET_READER_LIST);
      }
      public void onSuccess(List<String> result) {
        oracle.clear();
        oracle.addAll(result);
        for(String r : result){readerListBox.addItem(r);

        }
      }
    });
    final DialogBox dialogBox = new DialogBox();
    dialogBox.setText(BOOKS_WND_TITLE);
    dialogBox.setAnimationEnabled(true);
    final Button closeButton = new Button(CLOSE_BTN);
    closeButton.getElement().setId("closeButton");
    final HTML serverResponseLabel = new HTML();
    VerticalPanel dialogVPanel = new VerticalPanel();
    dialogVPanel.addStyleName("dialogVPanel");

    final CellTable<BookReader> table = createCellTable();
    dialogVPanel.add(table);

    dialogVPanel.setHorizontalAlignment(VerticalPanel.ALIGN_RIGHT);
    dialogVPanel.add(closeButton);
    dialogBox.setWidget(dialogVPanel);
    closeButton.addClickHandler(new ClickHandler() {
      public void onClick(ClickEvent event) {
        dialogBox.hide();
        sendButton.setEnabled(true);
        sendButton.setFocus(true);
      }
    });

    class RPCClickHandler implements ClickHandler, KeyUpHandler {
      public void onClick(ClickEvent event) {
        sendReaderFIOToServer();
      }
      public void onKeyUp(KeyUpEvent event) {
        if (event.getNativeKeyCode() == KeyCodes.KEY_ENTER) {
          sendReaderFIOToServer();
        }
      }
      private void sendReaderFIOToServer() {
        errorLabel.setText("");
        final String readerFIO = readerListBox.getValue(readerListBox.getSelectedIndex());
        sendButton.setEnabled(false);
        greetingService.getBookReaderList(readerFIO,
                new AsyncCallback<List<BookReader>>() {
                  public void onFailure(Throwable caught) {
                    dialogBox.setText(SRV_ERR);
                    serverResponseLabel.addStyleName("serverResponseLabelError");
                    serverResponseLabel.setHTML(SRV_ERR+SRV_ERR_GET_BOOK_LIST);
                    dialogBox.center();
                    closeButton.setFocus(true);
                  }
                  public void onSuccess(List<BookReader> result) {
                    dialogBox.setText(BOOKS_WND_TITLE + readerFIO);
                    table.setRowCount(result.size(), true);
                    table.setRowData(0, result);
                    dialogBox.center();
                    closeButton.setFocus(true);
                  }
                });
      }
    }
    RPCClickHandler handler = new RPCClickHandler();
    sendButton.addClickHandler(handler);
  }
  private CellTable<BookReader> createCellTable() {
    final CellTable<BookReader> table = new CellTable<BookReader>();
    table.setKeyboardSelectionPolicy(HasKeyboardSelectionPolicy.KeyboardSelectionPolicy.ENABLED);
    TextColumn<BookReader> authorColumn = new TextColumn<BookReader>() {
      public String getValue(BookReader object) {
        return object.getAuthor();
      }
    };
    table.addColumn(authorColumn, "Автор книги");
    TextColumn<BookReader> titleColumn = new TextColumn<BookReader>() {
      public String getValue(BookReader object) {
        return object.getTitle();
      }
    };
    table.addColumn(titleColumn, "Название книги");
    TextColumn<BookReader> isReadColumn = new TextColumn<BookReader>() {
      public String getValue(BookReader object) {
        return object.isReader() ? "Да":"Нет";
      }
    };
    table.addColumn(isReadColumn, "Прочитал");
    return table;
  }
}

