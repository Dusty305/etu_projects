package etu;

import java.io.IOException;

import jakarta.servlet.RequestDispatcher;
import jakarta.servlet.ServletException;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.*;

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
     * @param req servlet request
     * @param res servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest req, HttpServletResponse
            res)
            throws ServletException, IOException {
        String name = req.getParameter("user");
        String pwd = req.getParameter("password");
        if (name.equals("12345") && pwd.equals("passw0rd")) {
            HttpSession session = req.getSession();
            session.setAttribute("user", name);
            Cookie ck1 = new Cookie("user", name);
            Cookie ck2 = new Cookie("pwd", pwd);
            res.addCookie(ck1);
            res.addCookie(ck2);
            PrintWriter out = res.getWriter();
            out.write("Login successfull...");
        } else {
            req.setAttribute("error", "ERROR");
            RequestDispatcher rd = req.getRequestDispatcher("/index.jsp");
            rd.forward(req, res);
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