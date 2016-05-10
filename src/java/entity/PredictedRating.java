/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package entity;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.EmbeddedId;
import javax.persistence.Entity;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.validation.constraints.NotNull;
import javax.xml.bind.annotation.XmlRootElement;

/**
 *
 * @author joe
 */
@Entity
@Table(name = "predicted_rating", schema = "rating_recommending")
@XmlRootElement
@NamedQueries({
    @NamedQuery(name = "PredictedRating.findAll", query = "SELECT p FROM PredictedRating p"),
    @NamedQuery(name = "PredictedRating.findByPredictedRatingValue", query = "SELECT p FROM PredictedRating p WHERE p.predictedRatingValue = :predictedRatingValue"),
    @NamedQuery(name = "PredictedRating.findByUserId", query = "SELECT p FROM PredictedRating p WHERE p.predictedRatingPK.userId = :userId"),
    @NamedQuery(name = "PredictedRating.findDescPredictedRatingsByUserId", query = "SELECT p FROM PredictedRating p WHERE p.predictedRatingPK.userId = :userId ORDER BY p.predictedRatingValue DESC"),
    @NamedQuery(name = "PredictedRating.findByProductId", query = "SELECT p FROM PredictedRating p WHERE p.predictedRatingPK.productId = :productId")})
public class PredictedRating implements Serializable {

    private static final long serialVersionUID = 1L;
    @EmbeddedId
    protected PredictedRatingPK predictedRatingPK;
    @Basic(optional = false)
    @NotNull
    @Column(name = "predicted_rating_value")
    private double predictedRatingValue;

    public PredictedRating() {
    }

    public PredictedRating(PredictedRatingPK predictedRatingPK) {
        this.predictedRatingPK = predictedRatingPK;
    }

    public PredictedRating(PredictedRatingPK predictedRatingPK, double predictedRatingValue) {
        this.predictedRatingPK = predictedRatingPK;
        this.predictedRatingValue = predictedRatingValue;
    }

    public PredictedRating(int userId, int productId) {
        this.predictedRatingPK = new PredictedRatingPK(userId, productId);
    }

    public PredictedRatingPK getPredictedRatingPK() {
        return predictedRatingPK;
    }

    public void setPredictedRatingPK(PredictedRatingPK predictedRatingPK) {
        this.predictedRatingPK = predictedRatingPK;
    }

    public double getPredictedRatingValue() {
        return predictedRatingValue;
    }

    public void setPredictedRatingValue(double predictedRatingValue) {
        this.predictedRatingValue = predictedRatingValue;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (predictedRatingPK != null ? predictedRatingPK.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof PredictedRating)) {
            return false;
        }
        PredictedRating other = (PredictedRating) object;
        if ((this.predictedRatingPK == null && other.predictedRatingPK != null) || (this.predictedRatingPK != null && !this.predictedRatingPK.equals(other.predictedRatingPK))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "entity.PredictedRating[ predictedRatingPK=" + predictedRatingPK + " ]";
    }
    
}
