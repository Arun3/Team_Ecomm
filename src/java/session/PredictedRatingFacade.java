/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package session;

import entity.PredictedRating;
import java.util.List;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.Query;

/**
 *
 * @author joe
 */
@Stateless
public class PredictedRatingFacade extends AbstractFacade<PredictedRating> {

    @PersistenceContext(unitName = "AffableBeanPU")
    private EntityManager em;

    @Override
    protected EntityManager getEntityManager() {
        return em;
    }

    public List<PredictedRating> findTopXPredictedRatingsForCurrentUser(int customerId, int numTopResults) {
        List<PredictedRating> topXPredictedRatingsForCurrentUser = null;
        try {
            Query query = em.createNamedQuery("PredictedRating.findDescPredictedRatingByUserId");
            query.setParameter("userId", customerId );
            query.setFirstResult(0);
            query.setMaxResults(numTopResults);
            topXPredictedRatingsForCurrentUser = query.getResultList();
        } catch (Exception e) {
            String err = e.getMessage();
            System.out.println(err);
        }
        return topXPredictedRatingsForCurrentUser;
    }

    public PredictedRatingFacade() {
        super(PredictedRating.class);
    }

}
