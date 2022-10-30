#pragma once
#include<QImage>
#include "GMM.h"
#include "maxflow.h"
using namespace maxflow;
struct GMM_Core
{
	Vector3d mu;
	double pi;
	double det;
	Matrix3d Cov;
	Matrix3d CovI;
	double D(Vector3d z)
	{
		z -= mu;
		return -log(pi) + log(det) / 2.0 + (z.transpose() * CovI * z / 2.0)(0);
	}
	void Initialize(const Vector3d &_mu, const double &_pi, const double &_det, const MatrixXd &_Cov, const MatrixXd &_CovI)
	{
		mu = _mu;
		pi = _pi;
		det = _det;
		Cov = _Cov;
		CovI = _CovI;
	}
	void Generate(const GMMData &F, const int &cnt)
	{
		int N = F.size();
		pi = 1.0 * N / cnt;
		mu.setZero();
		for (int i = 0; i < N; ++i)
			mu += F[i];
		mu /= N;
		for(int x = 0;x < 3; ++x)
			for (int y = 0; y <= x; ++y)
			{
				double exy = 0;
				for (int i = 0; i < N; ++i)
					exy += F[i][x] * F[i][y];
				Cov(x, y) = Cov(y, x) = exy / N - mu[x] * mu[y];
			}
		CovI = Cov.inverse();
		det = Cov.determinant();
	}
};

class GrabCut 
{
public:
	GrabCut() { }
	GrabCut(int width, int height, Vector3d**Data, bool **mask, int K = 5) 
		: width(width), height(height), mask(mask), Data(Data), K(K) {}
	void Work();
	bool** alpha;
private:
	const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	const double gamma = 100;
	void GMMLocal();
	void GraphCutGlobal();
	void Initialize();
	Vector3d **Data;
	bool** mask;
	int** kClass;
	int** id;
	int width, height;
	GMM* GMM_Tool;
	int K;
	double beta;
	GMM_Core *Core[2];
	std::vector<std::pair<int, int>>Tu;
	Graph_DDD* G;
	double Energy;
};