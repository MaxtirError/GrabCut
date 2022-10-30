#include "GMM.h"
void GMM::Initialize()
{
	mu = new VectorXd[K]; MatrixXd::Random(K, Dims).array().abs();
	Cov = new MatrixXd[K];
	CovI = new MatrixXd[K];
	alpha = new double[K];
	det = new double[K];
	for (int i = 0; i < K; ++i) {
		mu[i] = VectorXd::Random(Dims).array().abs();
		Cov[i].resize(Dims, Dims);
		Cov[i].setIdentity();
		CovI[i] = Cov[i];
		det[i] = 1;
		alpha[i] = 1.0 / K;
	}
	gamma = new double* [K];
	for (int i = 0; i < K; ++i)
		gamma[i] = new double[n];

	//normalize data
	Bias.resize(Dims);
	Scale.resize(Dims);
	for (int i = 0; i < Dims; ++i) {
		double mx = -1e9, mn = 1e9;
		for (int j = 0; j < n; ++j) {
			mx = std::max(mx, datas[j][i]);
			mn = std::min(mn, datas[j][i]);
		}
		Scale[i] = mx - mn;
		Bias[i] = mn;
		for (int j = 0; j < n; ++j)
			datas[j][i] = (datas[j][i] - mn) / Scale[i];
	}
}

double GMM::G(VectorXd x, const VectorXd &mu, const MatrixXd &Cov, double *pA, MatrixXd *pCovI)
{
	MatrixXd CovI = pCovI == NULL ? Cov.inverse() : *pCovI;
	double A = pA == NULL ? pow(2 * pi, Dims / 2) * sqrt(Cov.determinant()) : *pA;
	VectorXd bias = x - mu;
	VectorXd res = bias.transpose() * CovI * bias / 2.0;
	double p = exp(-res(0)) / A;
	return p;
}
void GMM::Estimate()
//estimate the probablity of every sample
{
	double* sum = new double[n]{0};
	double PA = pow(2 * pi, Dims / 2);
	for (int i = 0; i < K; ++i) 
	{
		double A = PA * sqrt(det[i]);
		for (int j = 0; j < n; ++j) 
		{
			gamma[i][j] = alpha[i] * G(datas[j], mu[i], Cov[i], &A, CovI + i);
			sum[j] += gamma[i][j];
		}
	}
	for (int j = 0; j < n; ++j)
		for (int k = 0; k < K; ++k)
			gamma[k][j] /= sum[j];
	delete[] sum;
}

void GMM::Maximize()
//learning the parameter of GMM model
{
	for (int k = 0; k < K; ++k) 
	{
		mu[k].setZero(); alpha[k] = 0;
		for (int j = 0; j < n; ++j)
		{
			mu[k] += datas[j] * gamma[k][j];
			alpha[k] += gamma[k][j];
		}
		mu[k] /= alpha[k];
		Cov[k].setZero();
		for (int j = 0; j < n; ++j)
		{
			auto bias = datas[j] - mu[k];
			Cov[k] += gamma[k][j] * bias * bias.transpose();
		}
		Cov[k] /= alpha[k];
		alpha[k] /= n;
		CovI[k] = Cov[k].inverse();
		det[k] = Cov[k].determinant();
	}
}

void GMM::Compute()
{
	Initialize();
	for (int i = 0; i < iters_times; ++i)
	{
		Estimate();
		Maximize();
		printf("iter------times:%d\n", i);
		//show_info();
	}
}

std::vector<int> GMM::Classify() 
{
	Estimate();
	std::vector<int>belongs;
	for (int j = 0; j < n; ++j) 
	{
		int be = 0;
		for (int k = 1; k < K; ++k)
			if (gamma[k][j] > gamma[be][j])
				be = k;
		belongs.push_back(be);
	}
	return belongs;
}

void GMM::show_info()
{
	for (int k = 0; k < K; ++k) {
		printf("cluster_id:%d\n", k);
		std::cout << "mu" << mu[k]<<std::endl << "Cov:" << Cov[k]<<std::endl << "alpha:" << alpha[k] << std::endl;
	}
}

void GMM::Fix()
{
	for (int k = 0; k < K; ++k) {
		for (int d = 0; d < Dims; ++d) 
		{
			mu[k][d] *= Scale[d];
			mu[k][d] += Bias[d];
		}
		for (int i = 0; i < Dims; ++i)
			for (int j = 0; j < Dims; ++j)
				Cov[k](i, j) *= Scale[i] * Scale[j];
		CovI[k] = Cov[k].inverse();
		det[k] = Cov[k].determinant();
	}

}