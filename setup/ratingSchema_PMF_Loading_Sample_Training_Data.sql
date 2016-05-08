USE rating_recommending_pmf_test;
-- help from here: http://dev.mysql.com/doc/refman/5.1/en/load-data.html
LOAD DATA INFILE '/home/joe/Documents/Bayesian PMF/pmftraining.csv' 
INTO TABLE rating 
FIELDS TERMINATED BY ',' ENCLOSED BY '"'
LINES TERMINATED BY '\n' (customer_id, product_id, rating_value);
