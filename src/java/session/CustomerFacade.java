/*
 * Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
 *
 * You may not modify, use, reproduce, or distribute this software
 * except in compliance with the terms of the license at:
 * http://developer.sun.com/berkeley_license.html
 */

package session;

import entity.Customer;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.Query;

/**
 *
 * @author tgiunipero
 */
@Stateless
public class CustomerFacade extends AbstractFacade<Customer> {
    @PersistenceContext(unitName = "AffableBeanPU")
    private EntityManager em;

    protected EntityManager getEntityManager() {
        return em;
    }

    
    
     public Customer aunthenticateCustomer(String email,String password) {
        Customer customer = null;
        try {
            Query query = em.createNamedQuery("Customer.findByEmail");
            query.setParameter("email", email);
            //@SuppressWarnings("unchecked")
            customer = (Customer) query.getSingleResult();
            
                
        } catch (Exception e) {
            //s.setFirstName(e.getMessage());            
        }
        return customer;
    }
     
     
      @Override
    public void create(Customer customer) {
        //log insert new customer
        
    
        
        
        try {
          super.create(customer);
           } catch (Exception e) {
           
        }
        
        



    }
    public CustomerFacade() {
        super(Customer.class);
    }

     

}