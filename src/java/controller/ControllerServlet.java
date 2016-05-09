/*
 * Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
 *
 * You may not modify, use, reproduce, or distribute this software
 * except in compliance with the terms of the license at:
 * http://developer.sun.com/berkeley_license.html
 */

package controller;

import cart.ShoppingCart;
import entity.Cart;
import entity.CartPK;
import entity.Category;
import entity.Customer;
import entity.Product;
import entity.Rating;
import entity.RatingPK;
import java.io.IOException;
import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import javax.ejb.EJB;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.*;
import javax.sound.midi.SysexMessage;
import session.CartFacade;
import session.CategoryFacade;
import session.OrderManager;
import session.ProductFacade;
import session.CustomerFacade;
import session.RatingFacade;
import validate.Validator;

/**
 *
 * @author tgiunipero
 */
@WebServlet(name = "Controller",
            loadOnStartup = 1,
            urlPatterns = {"/signin",
                           "/category",
                           "/addToCart",
                           "/viewCart",
                           "/updateCart",
                           "/checkout",
                           "/purchase",
                           "/chooseLanguage",
                           "/searchProduct",
                           "/filterProduct",
                           "/SignUp",
                           "/recommend",
                           "/rate"
                           })
public class ControllerServlet extends HttpServlet {

    private String surcharge;

    @EJB
    private CategoryFacade categoryFacade;
    @EJB
    private ProductFacade productFacade;
    @EJB
    private OrderManager orderManager;
    @EJB
    private CustomerFacade customerFacade;
    @EJB
    private RatingFacade ratingFacade;
    @EJB
    private CartFacade cartFacade;


    @Override
    public void init(ServletConfig servletConfig) throws ServletException {

        super.init(servletConfig);

        // initialize servlet with configuration information
        surcharge = servletConfig.getServletContext().getInitParameter("deliverySurcharge");

        // store category list in servlet context
        getServletContext().setAttribute("categories", categoryFacade.findAll());
    }


    /**
     * Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        String userPath = request.getServletPath();
        HttpSession session = request.getSession();
        Category selectedCategory;
        Collection<Product> categoryProducts;
        Collection<Cart> cartItems;
        
        


        // if category page is requested
         if (userPath.equals("/category")) {

            // get categoryId from request
            String categoryId = request.getQueryString();

            if (categoryId != null) {

                // get selected category
                selectedCategory = categoryFacade.find(Short.parseShort(categoryId));

                // place selected category in session scope
                session.setAttribute("selectedCategory", selectedCategory);

                // get all products for selected category
                categoryProducts = selectedCategory.getProductCollection();

                // place category products in session scope
                session.setAttribute("categoryProducts", categoryProducts);
            }


        // if cart page is requested
        } else if (userPath.equals("/viewCart")) {

            String clear = request.getParameter("clear");

            if ((clear != null) && clear.equals("true")) {

                ShoppingCart cart = (ShoppingCart) session.getAttribute("cart");
                cart.clear();
            }

            userPath = "/cart";


        // if checkout page is requested
        } 
         else if (userPath.equals("/recommend")) {

            List<Product> products = productFacade.findProductByRating();


            if (products != null /*&& customer.getCustomertId() == customer1.getCustomerId()*/) {
                request.getSession().setAttribute("products", products);//store customer in a session scoped variable, for next jobs
                System.out.println("Product found");
                response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                response.setHeader("Location", "foundProducts.jsp");//??
                userPath = "/foundProducts";
                return;
            }

        // if checkout page is requested
        }
        
        else if (userPath.equals("/checkout")) {

            ShoppingCart cart = (ShoppingCart) session.getAttribute("cart");

            // calculate total
            cart.calculateTotal(surcharge);

            // forward to checkout page and switch to a secure channel


        // if user switches language
        } else if (userPath.equals("/chooseLanguage")) {

            // get language choice
            String language = request.getParameter("language");

            // place in request scope
            request.setAttribute("language", language);

            String userView = (String) session.getAttribute("view");

            if ((userView != null) &&
                (!userView.equals("/index"))) {     // index.jsp exists outside 'view' folder
                                                    // so must be forwarded separately
                userPath = userView;
            } else {

                // if previous view is index or cannot be determined, send user to welcome page
                try {
                    request.getRequestDispatcher("/index.jsp").forward(request, response);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                return;
            }
        }

        // use RequestDispatcher to forward request internally
        String url = "/WEB-INF/view" + userPath + ".jsp";

        try {
            request.getRequestDispatcher(url).forward(request, response);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }


    /**
     * Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        request.setCharacterEncoding("UTF-8");  // ensures that user input is interpreted as
                                                // 8-bit Unicode (e.g., for Czech characters)

        String userPath = request.getServletPath();
        HttpSession session = request.getSession();
        ShoppingCart cart = (ShoppingCart) session.getAttribute("cart");
        Validator validator = new Validator();
        
        if (userPath.equals("/signin")) {  //??
            /////////checkLogin(request, response);
            String email = request.getParameter("email");
            String password = request.getParameter("password");

            
            Customer customer = customerFacade.aunthenticateCustomer(email, password);
            System.out.println("In controller");
            

            if ((customer.getEmail()).equals(email) && (customer.getPassword()).equals(password)){
                 {
                    request.getSession().setAttribute("customer", customer);//store customer in a session scoped variable, for next jobs
                    request.getSession().setAttribute("message","success");
                    System.out.println("In controller--------success");
                    int customerId = customer.getId();
                    System.out.println("Customer ID"+customerId);
                    session.setAttribute("customerId", customerId);
                    
                    Cart newcart= cartFacade.getCartItems(customerId);
                   Timestamp currentTimestamp = new java.sql.Timestamp(Calendar.getInstance().getTime().getTime());
                   
                    
                    
                    response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                    response.setHeader("Location", "index.jsp");//??
                    userPath = "/category";//??
                    return;
                }
            } 

            response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
            response.setHeader("Location", "index.jsp?err=-1");
            
            return;
        }
        
        else if (userPath.equals("/SignUp")) { 
            String name = request.getParameter("name");
            String email = request.getParameter("email");
            String password = request.getParameter("password");
            String phone = request.getParameter("phone");
            String address = request.getParameter("address");
            String cityRegion = request.getParameter("cityRegion");
            //String ccNumber = request.getParameter("creditcard");
            //Integer id = 3;//  ??
            String ccNumber = "";  //??
            Customer newCustomer = new Customer() ;
           
            
            newCustomer.setName(name);
            newCustomer.setEmail(email);
            newCustomer.setPassword(password);
            newCustomer.setPhone(phone);
            newCustomer.setAddress(address);
            newCustomer.setCityRegion(cityRegion);
            newCustomer.setCcNumber(ccNumber);
            
            //Customer newCustomer = new Customer(id, name, email, phone, address, cityRegion, ccNumber);//??  or set functions?
            //newCustomer = orderManager.addCustomer( name, email, phone, address, cityRegion, ccNumber);
            customerFacade.create(newCustomer);
           response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                    response.setHeader("Location", "index.jsp");//??
                    userPath = "/category";//??
                    return;
           
        }
        else if (userPath.equals("/searchProduct")) {  //??
            /////////checkLogin(request, response);
            String productName = request.getParameter("productName");
            System.out.println("input:"+productName);
            //String password = request.getParameter("password");

//            Product[] product = productFacade.findProductByName(productName);
            List<Product> products = productFacade.findProductByName(productName);


            if (products != null /*&& customer.getCustomertId() == customer1.getCustomerId()*/) {
                request.getSession().setAttribute("products", products);//store customer in a session scoped variable, for next jobs
                System.out.println("Product found");
                response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                response.setHeader("Location", "foundProducts.jsp");//??
                userPath = "/foundProducts";
                return;
            } else {

                response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                response.setHeader("Location", "index.jsp?err=-1");
            }
        }
        
        else if (userPath.equals("/rate")) {  //??
            /////////checkLogin(request, response);
            int productId = Integer.parseInt(request.getParameter("productId"));
            System.out.println("product ID" + productId);
            HttpSession session2=request.getSession();
            int customerId = (Integer)session2.getAttribute("customerId");
            System.out.println("input:"+customerId);
            float rating= Float.parseFloat(request.getParameter("rating"));
            float i=5;
            RatingPK newRatingPk = new RatingPK();
            Rating newRating = new Rating() ;
           
            newRatingPk.setUserId(customerId);
            newRatingPk.setProductId(productId);
           newRating.setRatingPK(newRatingPk);
           newRating.setRatingValue(rating);
            
           
            
            //Customer newCustomer = new Customer(id, name, email, phone, address, cityRegion, ccNumber);//??  or set functions?
            //newCustomer = orderManager.addCustomer( name, email, phone, address, cityRegion, ccNumber);
            ratingFacade.create(newRating);
            //String password = request.getParameter("password");

//            Product[] product = productFacade.findProductByName(productName);
//            List<Product> products = productFacade.findProductByName(productName);
//
//
//            if (products != null /*&& customer.getCustomertId() == customer1.getCustomerId()*/) {
//                request.getSession().setAttribute("products", products);//store customer in a session scoped variable, for next jobs
//                System.out.println("Product found");
//                response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
//                response.setHeader("Location", "foundProducts.jsp");//??
//                userPath = "/foundProducts";
//                return;
//            } else {

                response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                response.setHeader("Location", "index.jsp");
                userPath = "/category";
            
        }
        
        else if (userPath.equals("/filterProduct")) {  //??
            /////////checkLogin(request, response);
            String priceRange = request.getParameter("price");
            System.out.println("input:"+priceRange);
            //String password = request.getParameter("password");

//            Product[] product = productFacade.findProductByName(productName);
            List<Product> products = productFacade.findProductByPrice(priceRange);


            if (products != null /*&& customer.getCustomertId() == customer1.getCustomerId()*/) {
                request.getSession().setAttribute("products", products);//store customer in a session scoped variable, for next jobs
                System.out.println("Product found");
                response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                response.setHeader("Location", "foundProducts.jsp");//??
                userPath = "/foundProducts";
                return;
            } else {

                response.setStatus(HttpServletResponse.SC_MOVED_PERMANENTLY);
                response.setHeader("Location", "index.jsp?err=-1");
            }
        }



        // if addToCart action is called
       else if (userPath.equals("/addToCart")) {

            // if user is adding item to cart for first time
            // create cart object and attach it to user session
            if (cart == null) {

                cart = new ShoppingCart();
                session.setAttribute("cart", cart);
            }

            // get user input from request
            String productId = request.getParameter("productId");

            if (!productId.isEmpty()) {

                Product product = productFacade.find(Integer.parseInt(productId));
                cart.addItem(product);
            }
            
            System.out.println("product ID" + productId);
            HttpSession session2=request.getSession();
            int customerId = (Integer)session2.getAttribute("customerId");
            System.out.println("input:"+customerId);
            
            Cart newcart = new Cart();
            CartPK newcartpk = new CartPK();
            Integer intObject = new Integer(productId);
int productId1 = intObject.intValue();
            System.out.println("integer product ID"+productId1);
            Timestamp currentTimestamp = new java.sql.Timestamp(Calendar.getInstance().getTime().getTime());
            newcartpk.setProductId(23);
            newcartpk.setUserId(customerId);
            newcart.setCartPK(newcartpk);
            newcart.setCreatedTime(currentTimestamp);
            newcart.setQuantity(1);
            
            cartFacade.create(newcart);
            
            

            userPath = "/cart";


        // if updateCart action is called
        } else if (userPath.equals("/updateCart")) {

            // get input from request
            String productId = request.getParameter("productId");
            String quantity = request.getParameter("quantity");

            boolean invalidEntry = validator.validateQuantity(productId, quantity);

            if (!invalidEntry) {

                Product product = productFacade.find(Integer.parseInt(productId));
                cart.update(product, quantity);
            }

            userPath = "/cart";


        // if purchase action is called
        } else if (userPath.equals("/purchase")) {

            if (cart != null) {

                // extract user data from request
                String name = request.getParameter("name");
                String email = request.getParameter("email");
                String phone = request.getParameter("phone");
                String address = request.getParameter("address");
                String cityRegion = request.getParameter("cityRegion");
                String ccNumber = request.getParameter("creditcard");

                // validate user data
                boolean validationErrorFlag = false;
                validationErrorFlag = validator.validateForm(name, email, phone, address, cityRegion, ccNumber, request);

                // if validation error found, return user to checkout
                if (validationErrorFlag == true) {
                    request.setAttribute("validationErrorFlag", validationErrorFlag);
                    userPath = "/checkout";

                    // otherwise, save order to database
                } else {

                    int orderId = orderManager.placeOrder(name, email, phone, address, cityRegion, ccNumber, cart);

                    // if order processed successfully send user to confirmation page
                    if (orderId != 0) {

                        // in case language was set using toggle, get language choice before destroying session
                        Locale locale = (Locale) session.getAttribute("javax.servlet.jsp.jstl.fmt.locale.session");
                        String language = "";

                        if (locale != null) {

                            language = (String) locale.getLanguage();
                        }

                        // dissociate shopping cart from session
                        cart = null;

                        // end session
                        session.invalidate();

                        if (!language.isEmpty()) {                       // if user changed language using the toggle,
                                                                         // reset the language attribute - otherwise
                            request.setAttribute("language", language);  // language will be switched on confirmation page!
                        }

                        // get order details
                        Map orderMap = orderManager.getOrderDetails(orderId);

                        // place order details in request scope
                        request.setAttribute("customer", orderMap.get("customer"));
                        request.setAttribute("products", orderMap.get("products"));
                        request.setAttribute("orderRecord", orderMap.get("orderRecord"));
                        request.setAttribute("orderedProducts", orderMap.get("orderedProducts"));

                        userPath = "/confirmation";

                    // otherwise, send back to checkout page and display error
                    } else {
                        userPath = "/checkout";
                        request.setAttribute("orderFailureFlag", true);
                    }
                }
            }
        }

        // use RequestDispatcher to forward request internally
        String url = "/WEB-INF/view" + userPath + ".jsp";

        try {
            request.getRequestDispatcher(url).forward(request, response);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

}