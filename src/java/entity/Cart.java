/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package entity;

import java.io.Serializable;
import java.util.Date;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.EmbeddedId;
import javax.persistence.Entity;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.validation.constraints.NotNull;
import javax.xml.bind.annotation.XmlRootElement;

/**
 *
 * @author Arun
 */
@Entity
@Table(name = "cart", schema = "affablebean")
@XmlRootElement
@NamedQueries({
    @NamedQuery(name = "Cart.findAll", query = "SELECT c FROM Cart c"),
    @NamedQuery(name = "Cart.findByUserId", query = "SELECT c FROM Cart c WHERE c.cartPK.userId = :userId"),
    @NamedQuery(name = "Cart.findByProductId", query = "SELECT c FROM Cart c WHERE c.cartPK.productId = :productId"),
    @NamedQuery(name = "Cart.findByCreatedTime", query = "SELECT c FROM Cart c WHERE c.createdTime = :createdTime"),
    @NamedQuery(name = "Cart.findByQuantity", query = "SELECT c FROM Cart c WHERE c.quantity = :quantity")})
public class Cart implements Serializable {
    private static final long serialVersionUID = 1L;
    @EmbeddedId
    protected CartPK cartPK;
    @Basic(optional = false)
    @NotNull
    @Column(name = "createdTime")
    @Temporal(TemporalType.TIMESTAMP)
    private Date createdTime;
    @Basic(optional = false)
    @NotNull
    @Column(name = "quantity")
    private int quantity;
    @JoinColumn(name = "user_id", referencedColumnName = "id", insertable = false, updatable = false)
    @ManyToOne(optional = false)
    private Customer customer;
    @JoinColumn(name = "product_id", referencedColumnName = "id", insertable = false, updatable = false)
    @ManyToOne(optional = false)
    private Product product;

    public Cart() {
    }

    public Cart(CartPK cartPK) {
        this.cartPK = cartPK;
    }

    public Cart(CartPK cartPK, Date createdTime, int quantity) {
        this.cartPK = cartPK;
        this.createdTime = createdTime;
        this.quantity = quantity;
    }

    public Cart(int userId, int productId) {
        this.cartPK = new CartPK(userId, productId);
    }

    public CartPK getCartPK() {
        return cartPK;
    }

    public void setCartPK(CartPK cartPK) {
        this.cartPK = cartPK;
    }

    public Date getCreatedTime() {
        return createdTime;
    }

    public void setCreatedTime(Date createdTime) {
        this.createdTime = createdTime;
    }

    public int getQuantity() {
        return quantity;
    }

    public void setQuantity(int quantity) {
        this.quantity = quantity;
    }

    public Customer getCustomer() {
        return customer;
    }

    public void setCustomer(Customer customer) {
        this.customer = customer;
    }

    public Product getProduct() {
        return product;
    }

    public void setProduct(Product product) {
        this.product = product;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (cartPK != null ? cartPK.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof Cart)) {
            return false;
        }
        Cart other = (Cart) object;
        if ((this.cartPK == null && other.cartPK != null) || (this.cartPK != null && !this.cartPK.equals(other.cartPK))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "entity.Cart[ cartPK=" + cartPK + " ]";
    }
    
}
