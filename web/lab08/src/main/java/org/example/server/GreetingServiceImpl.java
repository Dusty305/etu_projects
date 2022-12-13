package org.example.server;

import org.example.client.GreetingService;
import org.example.shared.BookReader;
import org.example.shared.FieldVerifier;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

import java.util.*;

/**
 * The server side implementation of the RPC service.
 */
@SuppressWarnings("serial")
public class GreetingServiceImpl extends RemoteServiceServlet implements
    GreetingService {
  private Map<String, List<BookReader>> db = null;
  public List<String> getReaderList()throws IllegalArgumentException {
    if( db == null )
      initDB();
    String[] tmp = new String[db.keySet().size()];
    db.keySet().toArray(tmp);
    return Arrays.asList(tmp);

  }
  public List<BookReader> getBookReaderList(String readerFIO) throws
          IllegalArgumentException {
    if( db == null ){
      initDB();
    }
    if (readerFIO.equals("Иванов И. И.")||readerFIO.equals ("Петров П.П.") ||
            readerFIO.equals("Сидоров С.С.")) return db.get(readerFIO);
    else throw new IllegalArgumentException ("Нет читателя "+readerFIO);
  }
  private void initDB(){
    db = new HashMap<String, List<BookReader>>();
    List<BookReader> entries1 = new ArrayList<BookReader>();
    entries1.add(new BookReader("Достоевский", "Игрок", true));
    entries1.add(new BookReader("Толстой", "Анна Каренина", true));
    entries1.add(new BookReader("Достоевский", "Идиот", false));
    db.put("Иванов И. И.", entries1);
    List<BookReader> entries2 = new ArrayList<BookReader>();
    entries2.add(new BookReader("Толстой", "Анна Каренина", false));
    entries2.add(new BookReader("Достоевский", "Игрок", true));
    db.put("Петров П.П.", entries2);
    List<BookReader> entries3 = new ArrayList<BookReader>();
    entries3.add(new BookReader("Толстой", "Анна Каренина", false));
    entries3.add(new BookReader("Булгаков", "Белая гвардия", true));
    db.put("Сидоров С.С.", entries3);
  }

  public String greetServer(String input) throws IllegalArgumentException {
    // Verify that the input is valid.
    if (!FieldVerifier.isValidName(input)) {
      // If the input is not valid, throw an IllegalArgumentException back to
      // the client.
      throw new IllegalArgumentException(
          "Name must be at least 4 characters long");
    }

    String serverInfo = getServletContext().getServerInfo();
    String userAgent = getThreadLocalRequest().getHeader("User-Agent");

    // Escape data from the client to avoid cross-site script vulnerabilities.
    input = escapeHtml(input);
    userAgent = escapeHtml(userAgent);

    return "Hello, " + input + "!<br><br>I am running " + serverInfo
        + ".<br><br>It looks like you are using:<br>" + userAgent;
  }

  /**
   * Escape an html string. Escaping data received from the client helps to
   * prevent cross-site script vulnerabilities.
   *
   * @param html the html string to escape
   * @return the escaped string
   */
  private String escapeHtml(String html) {
    if (html == null) {
      return null;
    }
    return html.replaceAll("&", "&amp;").replaceAll("<", "&lt;").replaceAll(
        ">", "&gt;");
  }
}
