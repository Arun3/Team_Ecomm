CREATE DATABASE IF NOT EXISTS `rating_recommending`;
USE `rating_recommending`;
DROP TABLE IF EXISTS `rating`;
CREATE TABLE `rating` (
`rating_value` double NOT NULL,
`customer_id` INT UNSIGNED NOT NULL, 
`product_id` INT UNSIGNED NOT NULL,
PRIMARY KEY (`customer_id`, `product_id` ),
FOREIGN KEY (`customer_id` ) REFERENCES affablebean.customer(`id`),
FOREIGN KEY (`product_id` ) REFERENCES affablebean.product(`id`)
 );