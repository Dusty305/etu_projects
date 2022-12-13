package ETU.WebGWT.server;
import ETU.WebGWT.client.GreetingService;
import ETU.WebGWT.shared.BookReader;
import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

/**
 * The server side implementation of the RPC service.
 */
@SuppressWarnings("serial")
public class GreetingServiceImpl extends RemoteServiceServlet implements GreetingService {
	private Map<String, List<BookReader>> db = null;
	public List<String> getReaderList()throws IllegalArgumentException {
		if( db == null ){
			initDB();
		}
		String[] tmp = new String[db.keySet().size()];
		db.keySet().toArray(tmp);
		 return Arrays.asList(tmp);}
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
}
