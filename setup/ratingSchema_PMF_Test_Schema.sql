-- building the test schema and inserting customer and product dummy data
-- for easily testing the recommender system
-- by only changing the name of the schemas used in the recommender system.

DROP DATABASE IF EXISTS `rating_recommending_pmf_test`;
DROP DATABASE IF EXISTS `affablebean_pmf_test`;


CREATE DATABASE IF NOT EXISTS `affablebean_pmf_test`;
USE `affablebean_pmf_test`;
DROP TABLE IF EXISTS `customer`;
DROP TABLE IF EXISTS `product`;

CREATE TABLE `customer` (
`id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
`dummy_name` VARCHAR(55),
PRIMARY KEY (`id`)
);

-- help from here: http://stackoverflow.com/questions/7408088/syntax-error-for-mysql-declaration-of-variable
DELIMITER //
-- this will create num_customers_to_generate dummy customers
-- also help from here: http://stackoverflow.com/questions/27614277/mysql-unknown-column-in-field-list
CREATE PROCEDURE dummy_customer_generator(IN example_name VARCHAR(55))
BEGIN
	DECLARE i INT DEFAULT 0;
	DECLARE num_customers_to_generate INT DEFAULT 6040;
	WHILE i < num_customers_to_generate DO
		INSERT INTO customer (dummy_name) VALUES (example_name);
		SET i = i + 1;
	END WHILE;
END//

DELIMITER ;

CALL dummy_customer_generator( 'examplecustomer' );

CREATE TABLE `product` (
`id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
`dummy_name` VARCHAR(55),
PRIMARY KEY (`id`)
);

DELIMITER //
-- this will create num_products_to_generate dummy products
CREATE PROCEDURE dummy_product_generator(IN example_name VARCHAR(55))
BEGIN
	DECLARE i INT DEFAULT 0;
	DECLARE num_products_to_generate INT DEFAULT 3952;
	WHILE i < num_products_to_generate DO
		INSERT INTO product (dummy_name) VALUES (example_name);
		SET i = i + 1;
	END WHILE;
END//

DELIMITER ;

-- executing this procedure
CALL dummy_product_generator( 'exampleproduct' );


CREATE DATABASE IF NOT EXISTS `rating_recommending_pmf_test`;
USE `rating_recommending_pmf_test`;
DROP TABLE IF EXISTS `rating`;

CREATE TABLE `rating` (
`rating_value` float NOT NULL,
`user_id` INT UNSIGNED NOT NULL, 
`product_id` INT UNSIGNED NOT NULL,
PRIMARY KEY (`user_id`, `product_id` ),
FOREIGN KEY (`user_id` ) REFERENCES affablebean_pmf_test.customer(`id`),
FOREIGN KEY (`product_id` ) REFERENCES affablebean_pmf_test.product(`id`)
 );