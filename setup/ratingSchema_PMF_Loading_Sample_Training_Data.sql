-- This file is an exmaple file for loading recommender system training data into
-- a training database called rating_recommending_pmf_test
USE rating_recommending_pmf_test;
-- help from here: http://dev.mysql.com/doc/refman/5.1/en/load-data.html
LOAD DATA INFILE '/path/to/pmftraining.csv' 
INTO TABLE rating 
FIELDS TERMINATED BY ',' ENCLOSED BY '"'
LINES TERMINATED BY '\n' (user_id, product_id, rating_value);
