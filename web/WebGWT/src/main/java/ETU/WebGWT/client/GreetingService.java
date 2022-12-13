package ETU.WebGWT.client;

import java.util.List;
import com.google.gwt.user.client.rpc.RemoteService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;
import ETU.WebGWT.shared.BookReader;
@RemoteServiceRelativePath("greet")
public interface GreetingService extends RemoteService{
	List<String> getReaderList();
	List<BookReader> getBookReaderList(String readerFIO)throws IllegalArgumentException;
}
