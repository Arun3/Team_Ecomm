/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package session;

import entity.Cart;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.Query;


/**
 *
 * @author Arun
 */
@Stateless
public class CartFacade extends AbstractFacade<Cart> {
    @PersistenceContext(unitName = "AffableBeanPU")
    private EntityManager em;

    @Override
    protected EntityManager getEntityManager() {
        return em;
    }

    public Cart getCartItems(int customer_id) {
        Cart cart = null;
        try {
            Query query = em.createNamedQuery("Cart.findByUserId");
            query.setParameter("userId", customer_id);
            //@SuppressWarnings("unchecked")
            cart = (Cart) query.getSingleResult();
            
                
        } catch (Exception e) {
            //s.setFirstName(e.getMessage());            
        }
        return cart;
    }
    public void create(Cart cart) {
        //log insert new customer
        
    
        
        
        try {
          super.create(cart);
           } catch (Exception e) {
           
        }
    }
    public CartFacade() {
        super(Cart.class);
    }
    
}
