<%-- 
    Document   : foundProducts
    Created on : Sep 10, 2015, 3:41:47 PM
    Author     : lenovo
--%>

<div id="container"> Search Result:</div>

<table id="datatable">
    <thead>
        <tr>
            <th>Name</th>
            <th>Price</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <c:forEach var="product" items="${products}" varStatus="iter">

            <tr>
                <th>${product.name}</th>
                <td> &nbsp &nbsp ${product.price} $</td>
                <td> &nbsp &nbsp ${product.description}</td>
                <td>
                    <form action="<c:url value='addToCart'/>" method="post">
                        <input type="hidden"
                               name="productId"
                               value="${product.id}">
                        <input type="submit"
                               name="submit"
                               value="<fmt:message key='addToCart'/>">
                    </form>
                </td>
            </tr>

        </c:forEach>
    </tbody>
</table>
</div> 
