/*
 * Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
 *
 * You may not modify, use, reproduce, or distribute this software
 * except in compliance with the terms of the license at:
 * http://developer.sun.com/berkeley_license.html
 */

package session;

import entity.Product;
import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.Query;

/**
 *
 * @author tgiunipero
 */
@Stateless
public class ProductFacade extends AbstractFacade<Product> {
    @PersistenceContext(unitName = "AffableBeanPU")
    private EntityManager em;

    protected EntityManager getEntityManager() {
        return em;
    }
       public List<Product> findProductByName(String productName) {
        List<Product> products = null;
     
        try{
            Query query = em.createNamedQuery("Product.findByName");
            query.setParameter("productName", "%"+ productName+"%");            
            products = query.getResultList();
            // log Select request:
          // log.debug("Q[?]");
            
        }
        catch(Exception e){
            String err = e.getMessage();
            System.out.println(err);

        }           
        return products;
    }
       public List<Product> findProductByPrice(String price) {
        List<Product> products = null;
        float price1;
            float price2;
     
        try{
            Query query = em.createNamedQuery("Product.findByPrice");
            float value;
            System.out.println("In product facade");
        if(price.equals("belowtwo"))
        {
            price1=0;
            price2=2;
             query.setParameter("price1", 0);  
             query.setParameter("price2", 2);  
        }
        else
        {
           price1=2;
            price2=4;
             query.setParameter("price1", 2);  
             query.setParameter("price2", 4); 
        }
                     
            products = query.getResultList();
            for(int i=0;i<products.size();i++)
            {
                System.out.println("Products retrieved:"+products.get(i));
            }
            // log Select request:
          // log.debug("Q[?]");
            
        }
        catch(Exception e){
            String err = e.getMessage();
            System.out.println(err);

        }           
        return products;
    }
    
        @Override
    public void create(Product product) {
            
         //For logging insert new product
      //  log.debug("Q[?]");
        
        super.create(product);




    }

    public ProductFacade() {
        super(Product.class);
    }

}