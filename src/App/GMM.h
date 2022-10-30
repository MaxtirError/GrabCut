#pragma once
#include<Eigen/LU>
#include<vector>
#include<iostream>
#define G_NORMAL 0
#define G_UNNORMAL 1
using namespace Eigen;
const double pi = acos(-1.0);
typedef std::vector<VectorXd> GMMData;

class GMM
{
private:
	int K, Dims, iters_times, n;
	VectorXd Bias;
	VectorXd Scale;
	double** gamma;
	GMMData datas;
	void Estimate();
	void Maximize();
	void Initialize();
public:
	VectorXd* mu;
	MatrixXd* Cov;
	MatrixXd* CovI;
	double* alpha;
	double* det;
	GMM() {}
	GMM(GMMData& datas, int K, int Dims, int iters_times = 100) :
		datas(datas), K(K), Dims(Dims), iters_times(iters_times), n(datas.size()) {}
	void Compute();
	double G(VectorXd x, const VectorXd& mu, const MatrixXd& Cov, double* pA = NULL, MatrixXd* pCovI = NULL);
	void show_info();
	std::vector<int> Classify();
	void Fix();
};