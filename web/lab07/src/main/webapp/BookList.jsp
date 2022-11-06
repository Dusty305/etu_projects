<%@ page import="java.util.Date" %>
<%@ page import="java.text.SimpleDateFormat" %>
<%@ page language="java" contentType="text/html; charset=UTF-8"
         pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>Введите имя автора</title>
  </head>
  <body>
  <%
    //Подсчет даты последнего обращения
    String new_last_accessed_time = new SimpleDateFormat("dd.MM.yyyy HH.mm.ss").format(new java.util.Date());
    session.setAttribute("new_last_time_accessed", new_last_accessed_time);
    System.out.println("new_last_accessed_time - " + session.getAttribute("new_last_time_accessed"));
    System.out.println("last_accessed_time - " + session.getAttribute("last_time_accessed"));


    // Подсчет количества обращений
    Integer count = (Integer)session.getAttribute("count");
    if(count == null)
    {
      count = 1;
      session.setAttribute("count", count);
    }
    else
      session.setAttribute("count", ++count);
  %>
  <form METHOD=GET action="AuthorProcessor">
    Введите имя пользователя <br>
    <INPUT TYPE=TEXT NAME="name"
      <%
        Cookie [] c = request.getCookies();
        if(c != null)
          // Находим Cookie с именем book.color
          for(int i = 0; i < c.length; i++)
            if("book.name".equals(c[i].getName())) {
              // Запись значения в поле ввода, если найден Cookie
              out.print(" value='" + java.net.URLDecoder.decode(c[i].getValue(), "UTF-8") + "' ");
              break;
            }
      %>
    > <br>
    Введите ФИО автора книги <br>
    <INPUT TYPE=TEXT NAME="author"
      <%
        // Выбор всех Cookie
        if(c != null)
          // Находим Cookie с именем book.author
          for(int i = 0; i < c.length; i++)
            if("book.author".equals(c[i].getName())) {
              // Запись значения в поле ввода, если найден Cookie
              out.print(" value='" + java.net.URLDecoder.decode(c[i].getValue(), "UTF-8") + "' ");
              break;
            }
      %>
    > <br>
    Введите цвет страницы: зеленый, красный, синий, белый <br>
    <INPUT TYPE=TEXT NAME="color"
      <%
        if(c != null)
          // Находим Cookie с именем book.color
          for(int i = 0; i < c.length; i++)
            if("book.color".equals(c[i].getName())) {
              // Запись значения в поле ввода, если найден Cookie
              out.print(" value='" + java.net.URLDecoder.decode(c[i].getValue(), "UTF-8") + "' ");
              break;
            }
      %>
    > <br>
    <INPUT TYPE=SUBMIT value="Ввод">
  </form>
  </body>
</html>