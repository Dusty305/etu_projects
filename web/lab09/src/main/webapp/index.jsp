<%@ page language="java" contentType="text/html; charset=UTF-8"
         pageEncoding="UTF-8"%>
<%@ page import="java.io.PrintWriter" %>
<html>
<body>
<h2>
<%
    PrintWriter printWriter = response.getWriter();
    if(request.getAttribute("error") == null)
        printWriter.println("Hello world!");
    else
        printWriter.println("Имя не было введено в качестве параметра");
%>
</h2>
</body>
</html>
