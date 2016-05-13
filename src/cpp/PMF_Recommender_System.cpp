// Description : Probabilistic Matrix Factorization based on
// original code at http://www.cs.toronto.edu/~rsalakhu/code_BPMF/pmf.m
// main page of original programs: http://www.cs.toronto.edu/~rsalakhu/BPMF.html

//% Original MATLAB Code provided by Ruslan Salakhutdinov
// Translation to C++ by Joseph Caprino
//%
//% Permission is granted for anyone to copy, use, modify, or distribute this
//% program and accompanying programs and documents for any purpose, provided
//% this copyright notice is retained and prominently displayed, along with
//% a note saying that the original programs are available from our
//% web page.
//% The programs and documents are distributed without any warranty, express or
//% implied.  As the programs were written for research purposes only, they have
//% not been tested to the degree that would be advisable in any important
//% application.  All use of these programs is entirely at the user's own risk.

//NOTE THAT THIS SOFTWARE NEEDS the following libraries:
//MySQL Connector for C++: libmysqlcppcon-dev (last used was version 1.1.3-5)

//COMPILING/LIBRARY INFORMATION:
// compiled in an Ubuntu-based system that had libmysqlcppcon-dev package installed
// version 1.1.3-5
//compile help from here:
//http://stackoverflow.com/questions/15995319/c-mysql-connector-undefined-reference-to-get-driver-instance-already-tri/18390778#18390778
//currently compiling with the following command:
//g++ -o PMF_Recommender_System -I/usr/include/cppconn -mavx2 -O3 -std=c++11
//-L/usr/lib/mysqlcppconn PMF_Recommender_System.cpp -lmysqlcppconn

//parts of this code from
//https://dev.mysql.com/doc/connector-cpp/en/connector-cpp-examples-complete-example-1.html
//and connector-cpp-examples-complete-example-2.html
//which is licensed under GPL V2

//BE SURE TO SET THE PROPER sql_connection and sql_schema throughout the code where you find these variables, 
//according to your system and current use (i.e. testing or deployment)


#include <iostream>
#include <algorithm>
#include <fstream>
#include <random> //C++11
//#include <cstdlib>
#include <math.h>
#include <chrono>
#include <ctime>
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


using namespace std;



int main() {

	//SETTING PARAMETERS
	double epsilon = 50;//learning rate
	double lambda = 0.01;//0.01; //regularization parameter
	double momentum = 0.8; //momentum

	int epoch = 0; //current epoch number
	int maxepoch = 50;  //number of epochs


	//LOADING DATA


	//load product data (triplet vector): user_id, product_id, rating
	//from sql database into 2 arrays:

	//get product and customer counts
	try {
		sql::mysql::MySQL_Driver *sql_driver;
		sql::Connection *sql_connection;
		sql::Statement *sql_statement;
		sql::ResultSet *sql_results;

		//creating connection
		sql_driver = sql::mysql::get_mysql_driver_instance();
		sql_connection = sql_driver->connect("tcp://127.0.0.1:3306", "root", "nbuser");
		//connecting to the database
		sql_connection->setSchema("affablebean");

		sql_statement = sql_connection->createStatement();



		sql_results = sql_statement->executeQuery("SELECT COUNT(*) AS product_count FROM product");
		sql_results->next();
		//cout << "SQL count: " << sql_results->getInt("product_count") << endl;
		int product_count = sql_results->getInt("product_count");
		//cout << "int count: " << product_count << endl;
		sql_results = sql_statement->executeQuery("SELECT COUNT(*) AS customer_count FROM customer");
		sql_results->next();
		int customer_count = sql_results->getInt("customer_count");
		int * product_id_map = new int[product_count];
		int * customer_id_map = new int[customer_count];

		//get all rows sorted (ORDER BY id)
		//build map from 0 to (product_count - 1) with the values in this set of rows
		//ordered by product.id. Do this with an array int[] product_id_map[product_count]
		//and store it like next(); product_id_map[i] = getInt("id");
		//on output, we can reverse this map for recommendations
		sql_results = sql_statement->executeQuery("SELECT id AS product_id FROM product ORDER BY id");
		int i = 0;
		while( sql_results->next() ) {
			//			cout << "product id: " << sql_results->getInt("product_id") << endl;
			product_id_map[i] = sql_results->getInt("product_id");
			//			cout << "product id map: " << product_id_map[i] << endl;
			i++;
		}

		//do the same with customer ids from customer.id.
		sql_results = sql_statement->executeQuery("SELECT id AS customer_id FROM customer ORDER BY id");
		i = 0;
		while( sql_results->next() ) {
			//			cout << "customer id: " << sql_results->getInt("customer_id") << endl;
			customer_id_map[i] = sql_results->getInt("customer_id");
			//			cout << "customer id map: " << customer_id_map[i] << endl;
			i++;
		}


		//count the number of triplets. Store in pairs_tr.

		//read in values from ratings database
		//use these array maps product_id_map and customer_id_map to reconstruct the triplets
		//inside the program
		sql_connection->setSchema("rating_recommending");
		sql_statement = sql_connection->createStatement();
		//		sql_statement->execute("INSERT INTO rating VALUES (1.5,1,2),(1.5,2,3),(1.5,3,4),(1.5,4,5),(1.5,5,6)");
		sql_results = sql_statement->executeQuery("SELECT COUNT(*) AS rating_count FROM rating");
		sql_results->next();
		int rating_count = sql_results->getInt("rating_count");
		int * ratings_customer_id = new int[rating_count];
		int * ratings_product_id = new int[rating_count];
		double * ratings_rating_value = new double[rating_count];

		sql_results = sql_statement->executeQuery("SELECT * FROM rating");
		int rating_row = 0;
		while( sql_results->next() ) {
			//			cout << "ROW:" << endl;
			//			cout << "customer id: " << sql_results->getInt("user_id") << endl;
			int customer_id = sql_results->getInt("user_id");
			//finding the corresponding index value for the product id and the customer id
			//Later, this can be changed to a bidirectional map or use binary search
			//to improve complexity
			int customer_map_index = 0;
			while(customer_id != customer_id_map[customer_map_index] ) customer_map_index++;
			//			cout << "product id: " << sql_results->getInt("product_id") << endl;
			int product_id = sql_results->getInt("product_id");
			int product_map_index = 0;
			while(product_id != product_id_map[product_map_index] ) product_map_index++;
			//			cout << "rating value: " << sql_results->getInt("rating_value") << endl;
			double rating_value = sql_results->getInt("rating_value");

			//storing a given triplet in the same index of these vectors
			//			cout << "Mapped:" << endl;
			//			cout << "customer: " << customer_map_index << endl;
			//			cout << "product: " << product_map_index << endl;
			//			cout << "rating: " << rating_value << endl;
			ratings_customer_id[rating_row] = customer_map_index;
			ratings_product_id[rating_row] = product_map_index;
			ratings_rating_value[rating_row] = rating_value;
			rating_row++;
		}

		//		//LOADING DATA
		//		//C++ File IO:
		//		//help from www.cplusplus.com/doc/tutorial/files/
		//		//and from
		//		//stackoverflow.com/questions/14516915/read-numeric-data-from-a-text-file-in-c
		//		//1st: create an ifstream object with the filename as the argument
		//		//	ifstream training_data_file("pmftraining.txt");
		ifstream validation_data_file("pmfvalidation.txt");


		//hard-coding this for now
		int pairs_tr = rating_count;
		cout << "rating count: " << rating_count << endl;
		//		int pairs_pr = 100209;
		int * training_data_uid = ratings_customer_id;
		int * training_data_mid = ratings_product_id;
		double * training_data_rating = ratings_rating_value;
		int * intermed_training_data_uid = new int[pairs_tr];
		int * intermed_training_data_mid = new int[pairs_tr];
		double * intermed_training_data_rating = new double[pairs_tr];
		int * shuffler = new int[pairs_tr];
		//		int * validation_data_uid = new int[pairs_pr];
		//		int * validation_data_mid = new int[pairs_pr];
		//		int * validation_data_rating = new int[pairs_pr];
		double err_train[maxepoch];
		//		double err_valid[maxepoch];




		//		int training_datum;
		//				int validation_datum;

		//		i = 0;
		//		//	while ((training_data_file >> training_datum) && (i<(900*1000*3)))
		//		//	{
		//		//		//setting user id and movie id to be 0-based with the - 1's
		//		//		if((i%3) == 0) training_data_uid[i/3] = training_datum - 1;
		//		//		else if((i%3) == 1) training_data_mid[i/3] = training_datum - 1;
		//		//		else training_data_rating[i/3] = training_datum;
		//		//		i++;
		//		//	}
		//		//
		//				i = 0;
		//				while ((validation_data_file >> validation_datum) && (i<(100209*3)))
		//				{
		//					//setting user id and movie id to be 0-based with the - 1's
		//					if((i%3) == 0) validation_data_uid[i/3] = validation_datum - 1;
		//					else if((i%3) == 1) validation_data_mid[i/3] = validation_datum - 1;
		//					else validation_data_rating[i/3] = validation_datum;
		//					i++;
		//				}

		//get the mean of the 3rd column of the training data
		//and stores it into a variable
		//called mean_rating.
		double mean_rating = 0;
		for(i=0; i<(pairs_tr); i++) {
			mean_rating += training_data_rating[i];
		}

		mean_rating = mean_rating/pairs_tr;


		//SETTING MORE PARAMETERS
		int numbatches = 2; //number of batches, set to 1 since N is set to pairs_tr, default 9
		int num_m = product_count; //number of movies
		int num_p = customer_count; //number of users
		int num_feat = 2; //number of features: rank 10 decomposition

		//Personalized Predicted Ratings Matrix
		double * personalized_predicted_ratings = new double[product_count*customer_count];

		//set N to the number of training triplets per batch (default 10^5);
		//KNOWN BUG (if it's even that serious, it may not be):
		//setting pairs_tr / numbatches will cause any training values beyond
		// the (floor(pairs_tr / numbatches) * numbatches)'th training value
		//to be excluded from a given epoch after shuffling.
		int N = pairs_tr / numbatches; //changed to size of the input dataset, originally 10*10*10*10*10;
		double * IO = new double[N*num_feat];

		//w1_M1: matrix of movie feature vectors filled with random
		//values sampled like this: (0.1*ndistribution(mean=0,variance=1)).
		//the dimensions of this random number matrix are:
		//num_m, num_feat
		//w1_P1: ditto for this USER feature vector matrix
		//(same sampling), but the dimensions are: num_p, num_feat
		//	int w1_P1[num_p][num_feat];
		//also make these matrices:
		//w1_M1_inc of dimensions num_m, num_feat (fill with zeroes)
		//w1_P1_inc of dimensions num_p, num_feat (fill with zeroes)
		double * w1_M1 = new double[num_m*num_feat];
		double * w1_P1 = new double[num_p*num_feat];
		double * w1_M1_inc = new double[num_m*num_feat];
		double * w1_P1_inc = new double[num_p*num_feat];
		double * Ix_m = new double[N*num_feat];
		double * Ix_p = new double[N*num_feat];
		double * dw1_M1 = new double[num_m*num_feat];
		double * dw1_P1 = new double[num_p*num_feat];
		int * aa_p = new int[N];
		int * aa_m = new int[N];
		double * rating = new double[N];
		double * pred_out = new double[N];
		double * alpha = new double[N];
		double * beta = new double[N];

		//normal distribution from
		//www.cplusplus.com/reference/random/normal_distribution/
		//note that seed issue for multicore is discussed here: http://stackoverflow.com/questions/15461140/stddefault-random-engine-generato-values-betwen-0-0-and-1-0
		//creating the random number engine generator according to //www.cplusplus.com/reference/random/normal_distribution/
		default_random_engine rand_eng;
		normal_distribution<double> ndistribution(0.0, 1.0); //defaults to mean=0, stdev=1.0 variance=stdev^2=1.0
		for(i=0; i<(num_m*num_feat); i++)
		{
			w1_M1[i] = 0.1 * ndistribution(rand_eng);
			w1_M1_inc[i] = 0.0;
		}

		for(i=0; i<(num_p*num_feat); i++)
		{
			w1_P1[i] = 0.1 * ndistribution(rand_eng);
			w1_P1_inc[i] = 0.0;
		}



		for(i=0; i<pairs_tr; i++) {
			shuffler[i] = i;
		}


		//START AN EPOCH (epoch = 1 to maxepoch inclusive)
		for(epoch=0;epoch<maxepoch;epoch++) {

			//randomly shuffling the shuffler array
			//http://www.cplusplus.com/reference/algorithm/random_shuffle/
			//help from http://stackoverflow.com/questions/22105867/seeding-default-random-engine
			shuffle(&shuffler[0], &shuffler[pairs_tr], rand_eng);
			//random_shuffle(&shuffler[0], &shuffler[pairs_tr]);

			//shuffle the training vector triples so that the triples
			//are now in a different row, but each triple's order is preserved
			for(i=0; i<pairs_tr; i++) {
				intermed_training_data_uid[i] = training_data_uid[shuffler[i]];
				intermed_training_data_mid[i] = training_data_mid[shuffler[i]];
				intermed_training_data_rating[i] = training_data_rating[shuffler[i]];
			}
			for(i=0; i<pairs_tr; i++) {
				training_data_uid[i] = intermed_training_data_uid[i];
				training_data_mid[i] = intermed_training_data_mid[i];
				training_data_rating[i] = intermed_training_data_rating[i];
			}


			//START A BATCH (batch = 1 to number of batches inclusive)
			for(int batch = 0; batch<numbatches; batch++) {
				//cout << "batch: " << batch << " epoch: " << epoch << "\n";

				//aa_p = (double cast) a vector taken from train_vec
				//of the next N values forming a given batch, first column (user id).
				//aa_m = ditto for second column (movie id).
				//rating = ditto for third column (rating).
				for(i=0;i<N;i++) {
					aa_p[i] = training_data_uid[batch*N + i];
					aa_m[i] = training_data_mid[batch*N + i];
					rating[i] = training_data_rating[batch*N + i];
				}



				//rating vector: subtract off the mean rating of all the values computed
				//at the beginning
				for(i=0;i<N;i++) {
					rating[i] = rating[i] - mean_rating;
				}


				//COMPUTE PREDICTIONS
				//using the values from aa_m (the current batch of movie id from the
				//training data), use aa_m values to grab these rows from w1_M1.
				//also do the same thing with aa_p and w1_P1.
				//Take these two sets of rows and element-wise product them.
				//Next, sum all their columns together to form a single column
				//of batch length N. Store in a vector called pred_out.
				//double interm_prod[N*num_feat];
				//double * pred_out = new double[N];
				for(i=0;i<N;i++) {
					double sum = 0;
					for(int j=0; j<num_feat; j++) {
						sum += w1_M1[num_feat*aa_m[i] + j]*w1_P1[num_feat*aa_p[i] + j];
					}
					pred_out[i] = sum;
				}



				//Alpha = calculate (pred_out - rating) and square each element of this difference.
				//
				//double * alpha = new double[N];
				for(i=0;i<N;i++) {
					alpha[i] = (pred_out[i] - rating[i])*(pred_out[i] - rating[i]);
				}
				//As before, use aa_m values to grab rows from w1_M1. Square each entry.
				//Ditto with aa_p and w1_P1.
				//Next, sum, in an element-wise fashion,
				//the result of the above two matrices that had their elements
				//squared. This will result in a matrix of the same dimensions.
				//Take this matrix and sum together all the num_feat elements
				//in each row, so it is reduced to a
				//column vector of length N.
				//Multiply this column vector by 0.5*lambda.
				//Add the result to the Alpha vector.
				for(i=0;i<N;i++) {
					double sum = 0;
					for(int j=0;j<num_feat;j++) {
						sum += w1_M1[num_feat*aa_m[i] + j] * w1_M1[num_feat*aa_m[i] + j]
																   + w1_P1[num_feat*aa_p[i] + j] * w1_P1[num_feat*aa_p[i] + j];
					}
					alpha[i] = alpha[i] + sum*0.5*lambda;
				}
				//Reduce-sum the resulting vector and store in f.
				double f = 0;
				for(i=0;i<N;i++) {
					f = alpha[i] + f;
				}


				//COMPUTE GRADIENTS
				//compute 2*(pred_out - rating) vector of length N.
				//double * beta = new double[N];

				for(i=0;i<N;i++) {
					beta[i] = 2*(pred_out[i] - rating[i]);
				}

				//repeat this vector num_feat times so that we have a matrix
				//of this 2*(pred_out - rating) vector repeated num_feat columns.
				//store in a matrix called "IO".
				for(i=0;i<N;i++) {
					for(int j=0;j<num_feat;j++) {
						IO[i*num_feat + j] = beta[i];
					}

				}


				//Take IO and multiply it element-wise with this quantity:
				//using aa_p values to grab rows from w1_P1. Add the resulting matrix
				//element-wise to (lambda*(using aa_m values to grab rows from w1_M1)).
				//Store the result of all of this into Ix_m.
				//Ditto for Ix_p, except the quantity (aa_m with w1_M1) is swapped with
				//(aa_p with w1_P1).
				for(i=0;i<N;i++) {
					for(int j=0;j<num_feat;j++) {
						Ix_m[i*num_feat + j] = IO[i*num_feat + j] * w1_P1[num_feat*aa_p[i] + j] + lambda*w1_M1[num_feat*aa_m[i] + j];
						Ix_p[i*num_feat + j] = IO[i*num_feat + j] * w1_M1[num_feat*aa_m[i] + j] + lambda*w1_P1[num_feat*aa_p[i] + j];
					}
				}

				//create two matrices of zeroes
				//dw1_M1 of size (num_m, num_feat)
				//dw1_P1 of size (num_p, num_feat)
				for(i=0;i<num_m;i++) {
					for(int j=0;j<num_feat;j++) {
						dw1_M1[i*num_feat + j] = 0;
					}
				}
				for(i=0;i<num_p;i++) {
					for(int j=0;j<num_feat;j++) {
						dw1_P1[i*num_feat + j] = 0;
					}
				}

				//for ii=1 to N:
				//Use the element in row ii of aa_m to get the row in
				//dw1_M1. Add this row element-wise to Ix_m at row ii.
				//do the same with aa_p, dw1_P1, and Ix_p.
				for(int ii=0;ii<N;ii++) {
					for(int j=0;j<num_feat;j++){
						dw1_M1[num_feat*aa_m[ii] + j] += Ix_m[num_feat*ii + j];
						dw1_P1[num_feat*aa_p[ii] + j] += Ix_p[num_feat*ii + j];
					}

				}
				//end ii loop






				//UPDATE MOVIE AND USER FEATURES
				//take the w1_M1_inc matrix and multiply each element by momentum.
				//	double * w1_M1_inc = new double[num_m*num_feat];
				//  double * w1_P1_inc = new double[num_p*num_feat];
				for(i=0;i<num_m;i++) {
					for(int j=0;j<num_feat;j++) {
						w1_M1_inc[i*num_feat + j] *= momentum;
					}
				}

				//divide epsilon by N, multiply each element of dw1_M1 by this result.
				double epsilon_over_N = epsilon / N;
				//	double * dw1_M1 = new double[num_m*num_feat];
				//  double * dw1_P1 = new double[num_p*num_feat];
				for(i=0;i<num_m;i++) {
					for(int j=0;j<num_feat;j++) {
						w1_M1_inc[i*num_feat + j] += epsilon_over_N * dw1_M1[i*num_feat + j];
					}
				}


				//calculate w1_M1 = w1_M1 - w1_M1_inc element-wise
				for(i=0;i<num_m;i++) {
					for(int j=0;j<num_feat;j++) {
						w1_M1[i*num_feat + j] -= w1_M1_inc[i*num_feat + j];
					}
				}


				//ditto for all of the above for P1
				for(i=0;i<num_p;i++) {
					for(int j=0;j<num_feat;j++) {
						w1_P1_inc[i*num_feat + j] *= momentum;
					}
				}

				for(i=0;i<num_p;i++) {
					for(int j=0;j<num_feat;j++) {
						w1_P1_inc[i*num_feat + j] += epsilon_over_N * dw1_P1[i*num_feat + j];
					}
				}

				for(i=0;i<num_p;i++) {
					for(int j=0;j<num_feat;j++) {
						w1_P1[i*num_feat + j] -= w1_P1_inc[i*num_feat + j];
					}
				}
				//end ditto for all of the above for P1


			} //end batch loop


			//COMPUTE PREDICTIONS AFTER PARAMETER UPDATES
			//do the same as the "COMPUTE PREDICTIONS" section, except
			//instead of storing into f, store into f_s.


			//NOTE: THIS CODE IS IDENTICAL TO PREVIOUS "COMPUTE PREDICTIONS" SECTION UNTIL
			//F_S
			//COMPUTE PREDICTIONS
			//using the values from aa_m (the current batch of movie id from the
			//training data), use aa_m values to grab these rows from w1_M1.
			//also do the same thing with aa_p and w1_P1.
			//Take these two sets of rows and element-wise product them.
			//Next, sum all their columns together to form a single column
			//of batch length N. Store in a vector called pred_out.
			//double interm_prod[N*num_feat];
			//double * pred_out = new double[N];
			for(i=0;i<N;i++) {
				double sum = 0;
				for(int j=0; j<num_feat; j++) {
					sum += w1_M1[num_feat*aa_m[i] + j]*w1_P1[num_feat*aa_p[i] + j];
				}
				pred_out[i] = sum;
			}



			//Alpha = calculate (pred_out - rating) and square each element of this difference.
			//
			//double * alpha = new double[N];
			for(i=0;i<N;i++) {
				alpha[i] = (pred_out[i] - rating[i])*(pred_out[i] - rating[i]);
			}
			//As before, use aa_m values to grab rows from w1_M1. Square each entry.
			//Ditto with aa_p and w1_P1.
			//Next, sum, in an element-wise fashion,
			//the result of the above two matrices that had their elements
			//squared. This will result in a matrix of the same dimensions.
			//Take this matrix and sum together all the num_feat elements
			//in each row, so it is reduced to a
			//column vector of length N.
			//Multiply this column vector by 0.5*lambda.
			//Add the result to the Alpha vector.
			for(i=0;i<N;i++) {
				double sum = 0;
				for(int j=0;j<num_feat;j++) {
					sum += w1_M1[num_feat*aa_m[i] + j] * w1_M1[num_feat*aa_m[i] + j]
															   + w1_P1[num_feat*aa_p[i] + j] * w1_P1[num_feat*aa_p[i] + j];
				}
				alpha[i] = alpha[i] + sum*0.5*lambda;
			}
			//END THIS CODE IS IDENTICAL TO PREVIOUS "COMPUTE PREDICTIONS" SECTION

			//Reduce-sum the resulting vector and store in f.
			double f_s = 0;
			for(i=0;i<N;i++) {
				f_s = alpha[i] + f_s;
			}


			//next, compute err_train(epoch) = sqrt(f_s/N)
			err_train[epoch] = sqrt(f_s/N);


			//			//Compute predictions on the validation set
			//			//
			//			//set NN=pairs_pr
			//			//above unncessary as int NN = pairs_pr;
			//			//set aa_p = (cast double) first column of the probe vec
			//			//already validation_data_uid
			//
			//			//set aa_m = "" second ""
			//			//already validation_data_mid
			//
			//			//set rating "" third ""
			//			//already validation_data_rating
			//
			//			//calculate pred_out by
			//			//using the values from aa_m, grab rows from w1_M1.
			//			//also do the same thing with aa_p and w1_P1.
			//			//multiply corresponding elements from the results of these two actions.
			//			//With the result of the last action, add all the elements of each
			//			//row together to form a single vector called val_pred_out of length NN.
			//			//add to that vector the mean_rating.
			//
			//			double val_pred_out[pairs_pr];
			//			for(i=0;i<pairs_pr;i++) {
			//				double sum = 0;
			//				for(int j=0; j<num_feat; j++) {
			//					sum += w1_M1[num_feat*validation_data_mid[i] + j] * w1_P1[num_feat*validation_data_uid[i] + j];
			//				}
			//				val_pred_out[i] = sum + mean_rating;
			//			}
			//
			//			//clip predictions to 5 by finding all of val_pred_out greater than 5,
			//			//and setting those to 5.
			//			//find all less than 1, and set to 1. Now we have ratings from 1 to 5.
			//
			//			for (i=0;i<pairs_pr;i++) {
			//				if(val_pred_out[i] < 1) val_pred_out[i] = 1;
			//				else if(val_pred_out[i] > 5) val_pred_out[i] = 5;
			//				else { //prediction is OK
			//				}
			//			}
			//
			//			//element-wise subtract val_pred_out and validation_data_rating, then square element-wise.
			//			//sum all the squared elements together.
			//			double total_squared_error = 0;
			//			for (i=0;i<pairs_pr;i++) {
			//				double to_square = (val_pred_out[i] - validation_data_rating[i]);
			//				total_squared_error += to_square*to_square;
			//			}
			//
			//			//divide the sum by pairs_pr, then take the square root,
			//			//store the result in err_valid(epoch).
			//			err_valid[epoch] = sqrt(total_squared_error/pairs_pr);




			//print out the epoch (epoch), the batch # (batch),
			//the training RMSE (err_train(epoch)),
			//and the test RMSE (err_valid(epoch)).
			cout << "\n" << "epoch #: " << epoch;
			cout << " training RMSE: " << err_train[epoch] << endl; //" test RMSE: " << err_valid[epoch] << endl;

			//if (epoch % 10) == 0, then checkpoint (save) the w1_M1 and w1_P1
			//in a file called pmf_weight.

		}//end epoch loop
		cout << "final training RMSE: " << err_train[maxepoch-1] << endl; //" final test RMSE: " << err_valid[maxepoch-1] << "\n";

		//CALCULATING THE USER-PERSONALIZED RECOMMENDATION PREDICTIONS MATRIX
		cout << "calculating personalized recommendations matrix" << endl;


		//		w1_M1 double[num_m*num_feat];
		//		w1_P1 double[num_p*num_feat];
		//customer dimension is i, product dimension is j
		for(int i=0; i<customer_count; i++) {
			for(int j=0; j<product_count; j++) {
				double sum = 0;
				for(int k=0; k<num_feat; k++) {
					sum += w1_M1[num_feat*j + k] * w1_P1[num_feat*i + k];
				}
				//storing the rating of one product, j, for a customer, i
				personalized_predicted_ratings[i*product_count + j] = sum;
			}
		}

		//INSERTING USER-PERSONALIZED RECOMMENDATION PREDICTIONS INTO THE DATABASE:
		cout << "inserting user-personalized recommendation predictions for each product";
		cout << " into the database" << endl;
		sql_connection->setSchema("rating_recommending");

		sql_statement = sql_connection->createStatement();

		for(int i=0; i<customer_count; i++) {
			for(int j=0; j<product_count; j++) {
				//help from here: https://dev.mysql.com/doc/connector-cpp/en/connector-cpp-examples-query.html
				string insert_query_string = "INSERT INTO predicted_rating(user_id, product_id, ";
				insert_query_string += "predicted_rating_value) VALUES (";
				insert_query_string += to_string( customer_id_map[i] );
				insert_query_string += ",";
				insert_query_string += to_string( product_id_map[j] );
				insert_query_string += ",";
				insert_query_string += to_string( (personalized_predicted_ratings[i*product_count + j] + mean_rating) ); //predicted_rating );
				//help from here: http://stackoverflow.com/questions/4205181/insert-into-a-mysql-table-or-update-if-exists
				insert_query_string += ") ON DUPLICATE KEY UPDATE predicted_rating_value=";
				insert_query_string += to_string( (personalized_predicted_ratings[i*product_count + j] + mean_rating) ); //predicted_rating );
//				cout << "insert_query_string: " << insert_query_string << endl;

				sql_statement->execute(insert_query_string);
			}
		}
		cout << "Completed insertion/update of predicted product ratings" << endl;

		//cleanup
		//delete[] training_data_uid, training_data_mid, training_data_rating;
		delete[] ratings_customer_id, ratings_product_id, ratings_rating_value;
		delete[] intermed_training_data_uid, intermed_training_data_mid, intermed_training_data_rating;
		delete[] shuffler;
		delete[] personalized_predicted_ratings;
		//		delete[] validation_data_uid, validation_data_mid, validation_data_rating;
		delete[] IO;


		delete[] w1_M1, w1_P1, w1_M1_inc, w1_P1_inc;
		delete[] Ix_m, Ix_p, dw1_M1, dw1_P1;
		delete[] aa_p, aa_m, rating, alpha, beta, pred_out;
	} catch (sql::SQLException &exception) {
		cout << "# ERR: " << exception.what();
		cout << " (MySQL error code: " << exception.getErrorCode();
		cout << ", SQLState: " << exception.getSQLState() << " )" << endl;
	}

	cout << "Recommendation Program Complete!" << endl;
	return 0;


}
