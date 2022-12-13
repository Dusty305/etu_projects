package org.example.client;

import com.google.gwt.user.client.rpc.RemoteService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;
import org.example.shared.BookReader;

import java.util.List;

/**
 * The client side stub for the RPC service.
 */
@RemoteServiceRelativePath("greet")
public interface GreetingService extends RemoteService {
  List<String> getReaderList();
  List<BookReader> getBookReaderList(String readerFIO)throws IllegalArgumentException;
  String greetServer(String name) throws IllegalArgumentException;
}
