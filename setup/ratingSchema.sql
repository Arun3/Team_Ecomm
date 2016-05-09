CREATE DATABASE IF NOT EXISTS `rating_recommending`;
USE `rating_recommending`;
DROP TABLE IF EXISTS `rating`;
CREATE TABLE `rating` (
`rating_value` INT NOT NULL,
`user_id` INT UNSIGNED NOT NULL, 
`product_id` INT UNSIGNED NOT NULL,
PRIMARY KEY (`user_id`, `product_id` ),
FOREIGN KEY (`user_id` ) REFERENCES affablebean.customer(`id`),
FOREIGN KEY (`product_id` ) REFERENCES affablebean.product(`id`)
 );

DROP TABLE IF EXISTS `predicted_rating`;
CREATE TABLE `predicted_rating` (
`predicted_rating_value` DOUBLE NOT NULL,
`user_id` INT UNSIGNED NOT NULL, 
`product_id` INT UNSIGNED NOT NULL,
PRIMARY KEY (`user_id`, `product_id` ),
FOREIGN KEY (`user_id` ) REFERENCES affablebean.customer(`id`),
FOREIGN KEY (`product_id` ) REFERENCES affablebean.product(`id`)
 );