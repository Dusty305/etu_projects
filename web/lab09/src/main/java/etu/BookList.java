package etu;

import java.io.IOException;

import jakarta.servlet.RequestDispatcher;
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
    protected void processRequest(HttpServletRequest request, HttpServletResponse
            response)
            throws ServletException, IOException {
// Задание типа кодировки для параметров запроса
        request.setCharacterEncoding("utf-8");
        // Чтение параметра name из запроса
        String name = request.getParameter("name");
        if(name == null)
        {
            request.setAttribute("error", "ERROR");
            RequestDispatcher rd = request.getRequestDispatcher("/index.jsp");
            rd.forward(request, response);
            return;
        }
        // Задание типа содержимого для ответа (в том числе кодировки)
        response.setContentType("text/html;charset=UTF-8");
        // Получение потока для вывода ответа
        PrintWriter out = response.getWriter();
        try {
            // Создание HTML-страницы
            out.write("<html>\n" +
                    "<head><title>Список книг</title></head>\n" +
                    "<body>\n" +
                    "<h1>Список книг читателя " + name + "</h1>\n" +
                    "<table border='1'>\n" +
                    "<tr><td><b>Автор книги</b></td><td><b>Название книги</b></td><td><b>Прочитал</b></td></tr>\n" +
                    "<tr><td>Булгаков</td><td>Мастер и Маргарита</td><td>Да</td></tr>\n" +
                    "<tr><td>Пелевин</td><td>Чапаев и пустота</td><td>Нет</td></tr>\n" +
                    "</table>\n" +
                    "</body>\n" +
                    "</html>\n");
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
    public void doGet(HttpServletRequest request, HttpServletResponse response)
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
    public void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }
}