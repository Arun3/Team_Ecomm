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




#include <iostream>
#include <algorithm>
#include <fstream>
#include <random> //C++11
//#include <cstdlib>
#include <math.h>
#include <chrono>
#include <ctime>
using namespace std;



int main() {

	//SETTING PARAMETERS
	double epsilon = 50;//learning rate
	double lambda = 0.01;//0.01; //regularization parameter
	double momentum = 0.8; //momentum

	int epoch = 0; //current epoch number
	int maxepoch = 50;  //number of epochs


	//LOADING DATA
	//C++ File IO:
	//help from www.cplusplus.com/doc/tutorial/files/
	//and from
	//stackoverflow.com/questions/14516915/read-numeric-data-from-a-text-file-in-c
	//1st: create an ifstream object with the filename as the argument
	ifstream training_data_file("pmftraining.txt");
	ifstream validation_data_file("pmfvalidation.txt");


	//load movie data (triplet vector): user_id, movie_id, rating
	//from file into 2 vectors:
	//train_vec (training data) and probe_vec (validation data).
	//Their columns are called user_id, movie_id, and rating.

	//count the number of triplets in each, store these counts into
	//pairs_tr and pairs_pr.
	//hard-coding this for now
	int pairs_tr = 900000;
	int pairs_pr = 100209;
	int * training_data_uid = new int[pairs_tr];
	int * training_data_mid = new int[pairs_tr];
	int * training_data_rating = new int[pairs_tr];
	int * intermed_training_data_uid = new int[pairs_tr];
	int * intermed_training_data_mid = new int[pairs_tr];
	int * intermed_training_data_rating = new int[pairs_tr];
	int * shuffler = new int[pairs_tr];
	int * validation_data_uid = new int[pairs_pr];
	int * validation_data_mid = new int[pairs_pr];
	int * validation_data_rating = new int[pairs_pr];
	double err_train[maxepoch];
	double err_valid[maxepoch];




	int training_datum;
	int validation_datum;

	int i = 0;
	while ((training_data_file >> training_datum) && (i<(900*1000*3)))
	{
		//setting user id and movie id to be 0-based with the - 1's
		if((i%3) == 0) training_data_uid[i/3] = training_datum - 1;
		else if((i%3) == 1) training_data_mid[i/3] = training_datum - 1;
		else training_data_rating[i/3] = training_datum;
		i++;
	}

	i = 0;
	while ((validation_data_file >> validation_datum) && (i<(100209*3)))
	{
		//setting user id and movie id to be 0-based with the - 1's
		if((i%3) == 0) validation_data_uid[i/3] = validation_datum - 1;
		else if((i%3) == 1) validation_data_mid[i/3] = validation_datum - 1;
		else validation_data_rating[i/3] = validation_datum;
		i++;
	}

	//get the mean of the 3rd column of the training data
	//and stores it into a variable
	//called mean_rating.
	double mean_rating = 0;
	for(i=0; i<(pairs_tr); i++) {
		mean_rating += training_data_rating[i];
	}

	mean_rating = mean_rating/pairs_tr;


	//SETTING MORE PARAMETERS
	int numbatches = 9; //number of batches
	int num_m = 3952; //number of movies
	int num_p = 6040; //number of users
	int num_feat = 10; //number of features: rank 10 decomposition

	//set N to the number of training triplets per batch (default 10^5);
	int N = 10*10*10*10*10;
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
			double pred_out[N];
			for(i=0;i<N;i++) {
				double sum = 0;
				for(int j=0; j<num_feat; j++) {
					sum += w1_M1[num_feat*aa_m[i] + j]*w1_P1[num_feat*aa_p[i] + j];
				}
				pred_out[i] = sum;
			}



			//Alpha = calculate (pred_out - rating) and square each element of this difference.
			//
			double alpha[N];
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
			double beta[N];

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
		double pred_out[N];
		for(i=0;i<N;i++) {
			double sum = 0;
			for(int j=0; j<num_feat; j++) {
				sum += w1_M1[num_feat*aa_m[i] + j]*w1_P1[num_feat*aa_p[i] + j];
			}
			pred_out[i] = sum;
		}



		//Alpha = calculate (pred_out - rating) and square each element of this difference.
		//
		double alpha[N];
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


		//Compute predictions on the validation set

		//set NN=pairs_pr
		//above unncessary as int NN = pairs_pr;
		//set aa_p = (cast double) first column of the probe vec
		//already validation_data_uid

		//set aa_m = "" second ""
		//already validation_data_mid

		//set rating "" third ""
		//already validation_data_rating

		//calculate pred_out by
		//using the values from aa_m, grab rows from w1_M1.
		//also do the same thing with aa_p and w1_P1.
		//multiply corresponding elements from the results of these two actions.
		//With the result of the last action, add all the elements of each
		//row together to form a single vector called val_pred_out of length NN.
		//add to that vector the mean_rating.

		double val_pred_out[pairs_pr];
		for(i=0;i<pairs_pr;i++) {
			double sum = 0;
			for(int j=0; j<num_feat; j++) {
				sum += w1_M1[num_feat*validation_data_mid[i] + j] * w1_P1[num_feat*validation_data_uid[i] + j];
			}
			val_pred_out[i] = sum + mean_rating;
		}

		//clip predictions to 5 by finding all of val_pred_out greater than 5,
		//and setting those to 5.
		//find all less than 1, and set to 1. Now we have ratings from 1 to 5.

		for (i=0;i<pairs_pr;i++) {
			if(val_pred_out[i] < 1) val_pred_out[i] = 1;
			else if(val_pred_out[i] > 5) val_pred_out[i] = 5;
			else { //prediction is OK
			}
		}

		//element-wise subtract val_pred_out and validation_data_rating, then square element-wise.
		//sum all the squared elements together.
		double total_squared_error = 0;
		for (i=0;i<pairs_pr;i++) {
			double to_square = (val_pred_out[i] - validation_data_rating[i]);
			total_squared_error += to_square*to_square;
		}

		//divide the sum by pairs_pr, then take the square root,
		//store the result in err_valid(epoch).
		err_valid[epoch] = sqrt(total_squared_error/pairs_pr);




		//print out the epoch (epoch), the batch # (batch),
		//the training RMSE (err_train(epoch)),
		//and the test RMSE (err_valid(epoch)).
		//cout << "\n" << "epoch #: " << epoch;
		//cout << " training RMSE: " << err_train[epoch] << " test RMSE: " << err_valid[epoch] << "\n";

		//if (epoch % 10) == 0, then checkpoint (save) the w1_M1 and w1_P1
		//in a file called pmf_weight.

	}//end epoch loop
	cout << "final training RMSE: " << err_train[maxepoch-1] << " final test RMSE: " << err_valid[maxepoch-1] << "\n";

	//cleanup
	delete[] training_data_uid, training_data_mid, training_data_rating;
	delete[] intermed_training_data_uid, intermed_training_data_mid, intermed_training_data_rating;
	delete[] shuffler;
	delete[] validation_data_uid, validation_data_mid, validation_data_rating;
	delete[] IO;


	delete[] w1_M1, w1_P1, w1_M1_inc, w1_P1_inc;
	delete[] Ix_m, Ix_p, dw1_M1, dw1_P1;
	delete[] aa_p, aa_m, rating;

	return 0;


}
