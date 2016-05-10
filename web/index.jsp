<%--
 * Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
 *
 * You may not modify, use, reproduce, or distribute this software
 * except in compliance with the terms of the license at:
 * http://developer.sun.com/berkeley_license.html

 * author: tgiunipero
--%>


<%-- Set session-scoped variable to track the view user is coming from.
     This is used by the language mechanism in the Controller so that
     users view the same page when switching between English and Czech. --%>
<c:set var='view' value='/index' scope='session' />


<%-- HTML markup starts below --%>
<form action="searchProduct" method="post" role="form">
    Search by Product Name<input type="text" name="productName"/>
    <button type="submit" name="submit">Search</button>
</form>
<form action="filterProduct" method="post" role="form">
    Select price range
    <input type="radio" name="price" value="belowtwo"/>Below 2$
    <input type="radio" name="price" value="abovetwo"/>Above 2$
    <button type="submit" name="submit">Filter</button>
</form>

<div id="indexLeftColumn">
    <div id="welcomeText">
        <p style="font-size: larger"><fmt:message key='greeting' /></p>

        <p><fmt:message key='introText' /></p>
        <c:set var="name" scope="session" value="${customer.name}"/>
        <%--
        * JOE is setting the  customer's id for the fetching of
        * recommended products. 
        <c:set var="user_id" scope="session" value="${customer.id}"/>
        --%>
        <c:choose>
<c:when test="${name == null}">
        <form action="signin" method="post" role="form">
        Username<input type="email" name="email" />
        Password<input type="password" name="password"/><br>
        <button type="submit" name="Log In" value="Log In">Log In</button>
        <a href="/AffableBean/Signup.jsp"> Not member?   Sign up</a>
        </form>
    </c:when>
        <c:otherwise>
                  
            Happy shopping!!!<br><br>
            <a href="<c:url value='recommend'/>">Products recommended for me</a>
            <%--
            * JOE will insert the top 10 recommended products here based on
            * the top 10 predicted products for the user.
            * In the ControllerServlet, this should call 
            * userPath.equals("/recommend") code.
            * This builds a list by 
            * List<Product> products = productFacade.findProductByRating().
            * The ProductFacade gets the results of the named query
            * called "Product.findByRating" by finding its corresponding query
            * in the Product class. The Product.findByRating query 
            * has this code:
            * "SELECT p FROM Product p WHERE p.rating = 5"
            * we want to alter this to show the top 10 according to the 
            * current logged-in user_id and the top 10 products from that.
            *
            * To do this: 
            * 1. send the customer.id variable to the session as
            * done above (test it). 
            * 2. Next, in the ControllerServlet, in this section:
            * "else if (userPath.equals("/recommend")) {"
            *   use HttpSession session2=request.getSession(); to
            *   get the current HTTP session. Then, call .getAttribute on it 
            *   like this and cast it: 
            *   Integer customerId = (Integer)session2.getAttribute("customerId"); 
            *   so we can have
            *   the current customerId to get products for.
            *   3. then, generate the list of products to return by 
            *   making a method 
            *   List<Product> recommendedProducts = 
            *   productFacade.findProductByPredictedRating(customerId.toString());
            * 4. In the ProductFacade class, change the method 
            *   "findProductByRating" to findProductByPredictedRating
            *   and give it an argument of String customerId. 
            *   In the function call,
            *   change Query query = 
            *   em.createNamedQuery("Product.findByRating"); into 
            *   em.createNamedQuery("Product.findByPredictedRating");
            *   add a line immediately below this one with 
            *   query.setParameter("customerId", "%"+ customerId+"%");
            * 5. In the Product class, alter this named query:
            *   @NamedQuery(name = "Product.findByRating" and change it to
            *   @NamedQuery(name = "Product.findByPredictedRating" and set
            *   its corresponding query to 
            *   query = "SELECT p FROM Product p WHERE p.rating = 5"
            *
            *
            * Last: look how the search or other function 
            * returns the list of products.
            *   
            --%>
        </c:otherwise>
    </c:choose>
            
        
    </div>
</div>

<div id="indexRightColumn">
    <c:forEach var="category" items="${categories}">
        <div class="categoryBox">
            <a href="<c:url value='category?${category.id}'/>">
                <span class="categoryLabel"></span>
                <span class="categoryLabelText"><fmt:message key='${category.name}'/></span>

                <img src="${initParam.categoryImagePath}${category.name}.jpg"
                     alt="<fmt:message key='${category.name}'/>" class="categoryImage">
            </a>
        </div>
    </c:forEach>
</div>