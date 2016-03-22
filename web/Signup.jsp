<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>
<%@ taglib uri="http://java.sun.com/jsp/jstl/fmt" prefix="fmt" %>
<%--
    Document   : checkout
    Created on : May 21, 2010, 12:20:23 AM
    Author     : tgiunipero
--%>


<%-- Set session-scoped variable to track the view user is coming from.
     This is used by the language mechanism in the Controller so that
     users view the same page when switching between English and Czech. --%>
<c:set var='view' value='/checkout' scope='session' />

<script src="js/jquery.validate.js" type="text/javascript"></script>

<script type="text/javascript">

    $(document).ready(function(){
        $("#checkoutForm").validate({
            rules: {
                name: "required",
                email: {
                    required: true,
                    email: true
                },
                phone: {
                    required: true,
                    number: true,
                    minlength: 9
                },
                password: {
                    required: true,
                    number: true,
                    minlength: 4
                },
                address: {
                    required: true
                },
                creditcard: {
                    required: true,
                    creditcard: true
                }
            }
        });
    });
</script>

<div id="singleColumn">

    <h2>Sign Up</h2>

    <p> Please Provide the following Information to create an account</p>

    <c:if test="${!empty orderFailureFlag}">
        <p class="error"><fmt:message key="orderFailureError"/></p>
    </c:if>

    <form id="checkoutForm" action="SignUp" method="post">
        <table id="checkoutTable">
            <c:if test="${!empty validationErrorFlag}">
                <tr>
                    <td colspan="2" style="text-align:left">
                        <span class="error smallText">Please provide valid entries for the following field(s):

                            <c:if test="${!empty nameError}">
                                <br><span class="indent"><strong>name</strong> (e.g., Bilbo Baggins)</span>
                            </c:if>
                            <c:if test="${!empty emailError}">
                                <br><span class="indent"><strong>email</strong> (e.g., b.baggins@hobbit.com)</span>
                            </c:if>

                            <c:if test="${!empty passwordError}">
                                <br><span class="indent"><strong>Password</strong> (e.g., 123456789)</span>
                            </c:if>


                            <c:if test="${!empty phoneError}">
                                <br><span class="indent"><strong>phone</strong> (e.g., 222333444)</span>
                            </c:if>
                            <c:if test="${!empty addressError}">
                                <br><span class="indent"><strong>address</strong> (e.g., Korunní 56)</span>
                            </c:if>
                            <c:if test="${!empty cityRegionError}">
                                <br><span class="indent"><strong>city region</strong> (e.g., 2)</span>
                            </c:if>
                            <c:if test="${!empty ccNumberError}">
                                <br><span class="indent"><strong>credit card</strong> (e.g., 1111222233334444)</span>
                            </c:if>

                        </span>
                    </td>
                </tr>
            </c:if>
            <tr>
                <td><label for="name"><fmt:message key="customerName"/></label></td>
                <td class="inputField">
                    <input type="text"
                           size="31"
                           maxlength="45"
                           id="name"
                           name="name"
                           value="${param.name}">
                </td>
            </tr>
            <tr>
                <td><label for="email"><fmt:message key="email"/></label></td>
                <td class="inputField">
                    <input type="text"
                           size="31"
                           maxlength="45"
                           id="email"
                           name="email"
                           value="${param.email}">
                </td>
            </tr>
            <tr>
                <td><label for="password">password</label></td>
                <td class="inputField">
                    <input type="text"
                           size="31"
                           maxlength="16"
                           id="password"
                           name="password"
                           value="${param.password}">
                </td>
            </tr>

            <tr>
                <td><label for="phone"><fmt:message key="phone"/></label></td>
                <td class="inputField">
                    <input type="text"
                           size="31"
                           maxlength="16"
                           id="phone"
                           name="phone"
                           value="${param.phone}">
                </td>
            </tr>

            <tr>
                <td><label for="address"><fmt:message key="address"/></label></td>
                <td class="inputField">
                    <input type="text"
                           size="31"
                           maxlength="45"
                           id="address"
                           name="address"
                           value="${param.address}">

                    <br>
                    <fmt:message key="prague"/>
                    <select name="cityRegion">
                        <c:forEach begin="1" end="10" var="regionNumber">
                            <option value="${regionNumber}"
                                    <c:if test="${param.cityRegion eq regionNumber}">selected</c:if>>${regionNumber}</option>
                        </c:forEach>
                    </select>
                </td>
            </tr>
            <tr>
                <td><label for="creditcard"><fmt:message key="creditCard"/></label></td>
                <td class="inputField">
                    <input type="text"
                           size="31"
                           maxlength="19"
                           id="creditcard"
                           name="creditcard"
                           value="${param.creditcard}">
                </td>
            </tr>
            <tr>
                <td colspan="2">
                    <input type="submit" value="SignUp">
                </td>
            </tr>
        </table>
    </form>

    <!--    <div id="infoBox">
    
            <ul>
                <li><fmt:message key="nextDayGuarantee"/></li>
                <li><fmt:message key="deliveryFee1"/>
    <fmt:formatNumber type="currency" currencySymbol="&euro; " value="${initParam.deliverySurcharge}"/>
    <fmt:message key="deliveryFee2"/></li>
</ul>

<table id="priceBox">
<tr>
    <td><fmt:message key="subtotal"/>:</td>
    <td class="checkoutPriceColumn">
    <fmt:formatNumber type="currency" currencySymbol="&euro; " value="${cart.subtotal}"/></td>
</tr>
<tr>
<td><fmt:message key="surcharge"/>:</td>
<td class="checkoutPriceColumn">
    <fmt:formatNumber type="currency" currencySymbol="&euro; " value="${initParam.deliverySurcharge}"/></td>
</tr>
<tr>
<td class="total"><fmt:message key="total"/>:</td>
<td class="total checkoutPriceColumn">
    <fmt:formatNumber type="currency" currencySymbol="&euro; " value="${cart.total}"/></td>
</tr>
</table>
</div>-->
</div>