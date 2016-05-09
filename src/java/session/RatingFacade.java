/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package session;

import entity.Rating;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

/**
 *
 * @author Arun
 */
@Stateless
public class RatingFacade extends AbstractFacade<Rating> {
    @PersistenceContext(unitName = "RecommenderPU")
    private EntityManager em;

    @Override
    protected EntityManager getEntityManager() {
        return em;
    }

    public void create(Rating rating) {
        //log insert new customer
        
    
        
        
        try {
          super.create(rating);
           } catch (Exception e) {
           
        }
        
        



    }
    public RatingFacade() {
        super(Rating.class);
    }
    
}
