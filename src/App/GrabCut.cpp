#include "GrabCut.h"

void GrabCut::Work()
{
	Initialize();
	Energy = 1e9;
	for (int i = 0; i < 100; ++i)
	{
		GMMLocal();
		GraphCutGlobal();
		std::cout << Energy << std::endl;
	}
}

void GrabCut::Initialize()
{
	std::cout << width << " " << height << std::endl;
	alpha = new bool*[ width];
	kClass = new int* [width];
	id = new int* [width];
	for (int i = 0; i < width; ++i) {
		alpha[i] = new bool [height];
		kClass[i] = new int [height];
		id[i] = new int[height];
		for (int j = 0; j < height; ++j) {
			alpha[i][j] = mask[i][j];
			if (mask[i][j]) {
				id[i][j] = Tu.size();
				Tu.push_back(std::make_pair(i, j));
			}
		}
	}

	GMMData datas[2];
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
			datas[alpha[i][j]].push_back(Data[i][j]);
	for (int i = 0; i < 2; ++i)
	{
		GMM_Tool = new GMM(datas[i], K, 3, 50);
		GMM_Tool->Compute();
		GMM_Tool->Fix();
		Core[i] = new GMM_Core[K];
		for (int k = 0; k < K; ++k)
			Core[i][k].Initialize(GMM_Tool->mu[k], GMM_Tool->alpha[k], GMM_Tool->det[k], GMM_Tool->Cov[k], GMM_Tool->CovI[k]);
		delete GMM_Tool;
	}
	//caculate beta
	beta = 0;
	int cnt = 0;
	for(int i = 0;i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			for (int d = 0; d < 8; ++d)
			{
				int ni = i + dx[d], nj = j + dy[d];
				if (ni < 0 || ni >= width || nj < 0 || nj >= height)
					continue;
				beta += (Data[i][j] - Data[ni][nj]).squaredNorm();
				++cnt;
			}
		}
	beta /= cnt;
	beta = 1.0 / (beta * 2.0);
	std::cout << beta << std::endl;
	G = new Graph_DDD(Tu.size() + 2, Tu.size() * 8);
}

void GrabCut::GMMLocal()
{
	double* D = new double[K];
	GMMData* F[2];
	F[0] = new GMMData[K];
	F[1] = new GMMData[K];
	int cnt[2] = { 0 };
	for (int i = 0;i < width; ++i) 
		for(int j = 0;j < height; ++j)
		{
			bool c = alpha[i][j];
			int km = 0;
			for (int k = 0; k < K; ++k)
			{
				D[k] = Core[c][k].D(Data[i][j]);
				if (D[km] > D[k])
					km = k;
			}
			kClass[i][j] = km;
			F[c][km].push_back(Data[i][j]);
			cnt[c]++;
		}
	delete D;
	for (int i = 0; i < 2; ++i)
		for (int k = 0; k < K; ++k)
			Core[i][k].Generate(F[i][k], cnt[i]);
	delete[] F[0];
	delete[] F[1];
}

void GrabCut::GraphCutGlobal()
{
	G->reset();
	G->add_node(Tu.size());
	for (int i = 0;i < Tu.size(); ++i) 
	{
		int px = Tu[i].first, py = Tu[i].second;
		double d0 = Core[0][kClass[px][py]].D(Data[px][py]);
		double d1 = Core[1][kClass[px][py]].D(Data[px][py]);
		G->add_tweights(i, d0, d1);
		for (int d = 0; d < 8; ++d)
		{
			int nx = px + dx[d], ny = py + dy[d];
			if (nx < 0 || nx >= width || ny < 0 || ny >= height)
				continue;
			double v = gamma * exp(-beta * (Data[px][py] - Data[nx][ny]).squaredNorm());
			if (mask[nx][ny] == false) //alpha = 0
				G->add_tweights(i, 0, v);
			else
				G->add_edge(i, id[nx][ny], v, 0);
		}
	}
	Energy = G->maxflow();
	for (int i = 0; i < Tu.size(); ++i) 
		alpha[Tu[i].first][Tu[i].second] = (G->what_segment(i) == Graph_DDD::SOURCE);
}