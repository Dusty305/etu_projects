package etu;

import java.io.IOException;
import jakarta.servlet.ServletException;
import jakarta.servlet.ServletInputStream;
import jakarta.servlet.http.*;

import java.net.URLEncoder;
import java.util.Arrays;
import java.util.Date;

/**
 * Servlet implementation class AuthorProcessor
 */
public class AuthorProcessor extends HttpServlet {
    private static final long serialVersionUID = 1L;
    // Авторы
    private static final String [] authors = {"Булгаков", "Пелевин"};
    private static final String [] colors = {"зеленый", "красный", "синий", "белый"};
    public AuthorProcessor() {
        super();
    }
    protected void processRequest(HttpServletRequest request, HttpServletResponse
            response) throws ServletException, IOException {
        response.setContentType("text/html; charset=UTF-8");
        request.setCharacterEncoding("utf-8");
        ServletInputStream stream = request.getInputStream();
        byte[] b = new byte[8];
        System.out.println(stream.readLine(b, 0, 3));
        System.out.println(b);
        // Получение параметра из строки запроса
        final String author = request.getParameter("author");
        String color  = request.getParameter("color");
        String name   = request.getParameter("name");
        System.out.println("Автор - " + author);
        System.out.println("Цвет - " + color);
        System.out.println("Имя пользователя- " + name);

        // Сообщение об ошибке, если сервлет вызван без параметра
        if(author == "") {
            response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Не задано имя автора");
            return;
        }
        if(color == "") {
            response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Не задан цвет страницы");
            return;
        }
        if(name == ""){
            response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Не задано имя пользователя");
            return;
        }

        // Проверка, что автор и цвет из запроса существуют
        if(!Arrays.stream(authors).anyMatch(s->s.equals(author))) {
            response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Автор с фамилией " + author + " не найден");
            return;
        }
        if(!Arrays.stream(colors).anyMatch(s->s.equals(color))) {
            response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Цвет " + color + " не найден");
            return;
        }

        // Сохранение автора в Cookie
        Cookie authorCookie = new Cookie("book.author", URLEncoder.encode(author, "UTF-8"));
        Cookie colorCookie = new Cookie("book.color", URLEncoder.encode(color, "UTF-8"));
        Cookie nameCookie = new Cookie("book.name", URLEncoder.encode(name, "UTF-8"));
        // Установка времени жизни Cookie в секундах
        authorCookie.setMaxAge(100);
        colorCookie.setMaxAge(100);
        nameCookie.setMaxAge(100);
        response.addCookie(authorCookie);
        response.addCookie(colorCookie);
        response.addCookie(nameCookie);

        // Перенаправление на страницу книг
        response.sendRedirect(response.encodeRedirectURL(request.getContextPath() +
                "/AskAuthorName.jsp"));
        HttpSession session = request.getSession();
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws
            ServletException, IOException {
        processRequest(request, response);
    }
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }
}