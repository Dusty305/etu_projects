<%@ page import="java.net.URLDecoder" %>
<%@ page language="java" contentType="text/html; charset=UTF-8"
         pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>Отфильтрованный список книг</title>
</head>
<%
    String name = null, author = null, color = null;
    Cookie []cookies = request.getCookies();
    for(int i = 0; i < cookies.length; i++)
        if("book.name".equals(cookies[i].getName()))
            name = URLDecoder.decode(cookies[i].getValue());
        else if("book.author".equals(cookies[i].getName()))
            author = URLDecoder.decode(cookies[i].getValue());
        else if("book.color".equals(cookies[i].getName()))
            color = URLDecoder.decode(cookies[i].getValue());
%>
<body
<%
    // Определяем цвет фона html-страницы
    if("зеленый".equals(color))
        out.print(" style=\"background-color:#6AFF97;\" ");
    else if("красный".equals(color))
        out.print(" style=\"background-color:#FF6A6A;\" ");
    else if("синий".equals(color))
        out.print(" style=\"background-color:#6A97FF;\" ");
    else if("белый".equals(color))
        out.print(" style=\"background-color:#000000;\" ");
%>
>
<h1>Список книг <% out.print(name);%></h1>
<table border='1'>
    <tr>
        <td><b>Автор книги</b></td>
        <td><b>Название книги</b></td>
        <td><b>Прочитал</b></td>
    </tr>
    <% if("Булгаков".equals(author)) {%>
    <tr>
        <td>Булгаков</td>
        <td>Мастер и Маргарита</td>
        <td>Да</td>
    </tr>
    <% }
        if("Пелевин".equals(author))
        {%>
    <tr>
        <td>Пелевин</td>
        <td>Чапаев и пустота</td>
        <td>Нет</td>
    </tr>
    <% } %>
</table>
Последнее обращение:
<%
    String last_accessed_time = (String) session.getAttribute("last_time_accessed");
    out.print(last_accessed_time);
    session.setAttribute("last_time_accessed", session.getAttribute("new_last_time_accessed"));
%><br>
Количество обращений:
<%
    Integer count = (Integer)session.getAttribute("count");
    out.println(count);
%><br>
Список Cookie:<br>
<%
    Cookie []c = request.getCookies();
    for(int i = 0; i < c.length; i++)
        out.print(c[i].getName() + ": " + java.net.URLDecoder.decode(c[i].getValue()) + "<br>");
%>
</body>
</html>