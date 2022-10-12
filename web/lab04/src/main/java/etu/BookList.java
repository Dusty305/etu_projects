package etu;

import java.io.IOException;
import java.util.ResourceBundle;
import java.util.Locale;
import jakarta.servlet.ServletException;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

// библиотечный класс для работы с потоками вывода
import java.io.PrintWriter;

/**
 * Servlet implementation class BooksList
 */
// @WebServlet("/BookList")
public class BookList extends HttpServlet {
    private static final long serialVersionUID = 1L;
    /**
     * @see HttpServlet#HttpServlet()
     */
    public BookList() {
        super();
        // TODO Auto-generated constructor stub
    }
    /**
     * Processes requests for both HTTP <code>GET</code> and <code>POST</code>
     methods.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        // Чтение параметров из строки
        String lang = request.getParameter("lang");
        if(lang == null) {
            response.sendError(HttpServletResponse.SC_NOT_ACCEPTABLE,
                    "Ожидался параметр lang");
            return;
        }
        if(!"en".equalsIgnoreCase(lang) && !"ru".equalsIgnoreCase(lang)) {
            response.sendError(HttpServletResponse.SC_NOT_ACCEPTABLE,
                    "Параметр lang может принимать значения en или ru");
            return;
        }
        // Задание типа содержимого для ответа (в том числе кодировки)
        response.setContentType("text/html;charset=UTF-8");
        // Файлы ресурсов book.properties, book_en.properties и book_ru.properties
        // Установка локализации в соответствии с выбором пользователя
        ResourceBundle res = ResourceBundle.getBundle(
                "/Book", "en".equalsIgnoreCase(lang) ? Locale.ENGLISH : Locale.getDefault());
        // Получение потока для вывода ответа
        PrintWriter out = response.getWriter();
        try {
            // Создание HTML-страницы
            out.println("<html>");
            out.println("<head><title>");
            // Вывод строки с учетом локализации
            out.print(res.getString("title"));
            out.println("</title></head>");
            out.println("<body>");
            out.println("<h1>");
            out.print(res.getString("title"));
            out.println("</h1>");
            out.println("<table border='1'>");
            out.println("<tr><td><b>");
            out.print(res.getString("author"));
            out.println("</b></td><td><b>");
            out.print(res.getString("book.title"));
            out.println("</b></td><td><b>");
            out.print(res.getString("read"));
            out.println("</b></td></tr>");
            out.println("<tr><td>Булгаков</td><td>Мастер и Маргарита </td><td>Да</td></tr>");
            out.println("<tr><td>Пелевин</td><td>Чапаев и пустота</td><td>Нет</td></tr>");
            out.println("</table>");
            out.println("</body>");
            out.println("</html>");
        } finally {
            // Закрытие потока вывода
            out.close();
        }
    }
    /**
     * Handles the HTTP
     * <code>GET</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }
    /**
     * Handles the HTTP
     * <code>POST</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }
}