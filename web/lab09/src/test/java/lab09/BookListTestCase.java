package lab09;

import etu.BookList;
import jakarta.servlet.RequestDispatcher;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import junit.framework.TestCase;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.io.PrintWriter;
import java.io.StringWriter;

import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class BookListTestCase extends TestCase {
    @Mock
    HttpServletRequest correctRequest, wrongRequest;
    @Mock
    HttpServletResponse correctResponse, wrongResponse;
    @Mock
    RequestDispatcher requestDispatcher;

    @Before
    protected void setUp()
    {
        MockitoAnnotations.openMocks(this);
    }

    @Test
    public void test() throws Exception
    {
        final String name = "Ivan";
        when(correctRequest.getParameter("name")).thenReturn(name);
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        when(correctResponse.getWriter()).thenReturn(pw);

        when(wrongRequest.getParameter("name")).thenReturn(null);
        when(wrongRequest.getRequestDispatcher("/index.jsp")).thenReturn(requestDispatcher);

        BookList bookList = new BookList();
        bookList.doPost(correctRequest, correctResponse);
        verify(correctRequest).setCharacterEncoding("utf-8");
        verify(correctResponse).setContentType("text/html;charset=UTF-8");
        String result = sw.getBuffer().toString();
        assertEquals(
                "<html>\n" +
                        "<head><title>Список книг</title></head>\n" +
                        "<body>\n" +
                        "<h1>Список книг читателя " + name + "</h1>\n" +
                        "<table border='1'>\n" +
                        "<tr><td><b>Автор книги</b></td><td><b>Название книги</b></td><td><b>Прочитал</b></td></tr>\n" +
                        "<tr><td>Булгаков</td><td>Мастер и Маргарита</td><td>Да</td></tr>\n" +
                        "<tr><td>Пелевин</td><td>Чапаев и пустота</td><td>Нет</td></tr>\n" +
                        "</table>\n" +
                        "</body>\n" +
                        "</html>\n",
                result
        );
        bookList.doPost(wrongRequest, wrongResponse);
        verify(correctRequest).setCharacterEncoding("utf-8");
        verify(wrongRequest).setAttribute("error", "ERROR");
        verify(requestDispatcher).forward(wrongRequest, wrongResponse);
    }
}
