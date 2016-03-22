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
                  
            Happy shopping!!!
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