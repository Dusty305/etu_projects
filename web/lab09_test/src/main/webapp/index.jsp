<html>
<head>
    <title>login form</title>
</head>
<body>
<form method="post" action="/BookList">
    User:<input type="text" name="user" /><br/>
    Password:<input type="text" name="password" /><br/>
    <input type="submit" value="login" />
</form>
<p>Error: <output name="error"><% if (request.getAttribute("error") !=null)
    out.println(request.getAttribute("error")); %></output></p>
</body>
</html>