#include <iostream>
#include <math.h>
#include <random>
#include <string>
#include <mutex>
#include <thread>
using namespace std;


int frac(int n)
{
	int result = 1;
	for (int i = 1; i <= n; i++)
		result *= i;
	return result;
}

double bino_k_outof_n(int n, int k, double p)
{
	return frac(n) / frac(k) / frac(n-k) * pow(p, k) * pow(1-p, n-k);
}

double calc_p_rare(double alpha, int beta, int p_dlvr)
{
	int window = round(alpha * beta * p_dlvr / (1000 - p_dlvr));
	// calculate the prob of losing no less than beta packets in window
	double cum_p = 0;
	for (int loss = beta; loss <= window; loss++)
	{
		cum_p += bino_k_outof_n(window, loss, 1 - 0.001 * p_dlvr);
	}
	return cum_p;

}


class Simulator
{
private:
    int p_rare;
    int p_success;
    double alpha;
    double s_awd;
    double s_init;
    double s_dec;
    double upper;

    bool simulate_one(int rounds)
    {
        double score = s_init;
        for (int i = 0; i < rounds; i++)
        {

            int rd = rand() % 1000;
            if (rd < p_rare)
            {
                score -= s_dec;
                if (score < 0)
                    return false;
            }
            else if (rd > 1 - p_success)
            {
                score += s_awd;
                score = min(score, upper);
            }
        }
        return true;
    }

public:
    Simulator(double one_over_alp, int beta, int dlvr)
    {
        p_success = dlvr; // 99.5%
        p_rare = 1000 - p_success;
        alpha = 1.0 / one_over_alp; // Bad nodes can perform 10x omissions that a good node without being caught

        s_awd = 1;

        s_dec = alpha * p_success / p_rare * s_awd;
        upper = beta * s_dec; // Bad nodes can perform at most 2 omissions in a short period without being caught
//        s_init = upper / 2;
        s_init = upper;
        // cout << "alpha=" << alpha << ", beta=" << beta << endl;
        // cout << "s_dec=" << s_dec << ", init=" << s_init << endl;
    }
    double simulate_all(int nodes)
    {
        int n_succ = 0;
        for (int i = 0; i < nodes; i++)
        {
            if (this->simulate_one(10000))
                n_succ += 1;
        }
        return n_succ;
    }
};

static std::mutex print_mtx;

void th_given_dlvr(int dlvr)
{
    double one_over_alphas[] = {6.0 / 5.0, 4.0 / 3.0, 3.0 / 2.0, 2, 3, 5};
    int betas[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

    for (int j = 0; j < sizeof(one_over_alphas) / sizeof(double); j++)
    {
        string str = "p_dlvr=" + std::to_string(dlvr) + ", 1/alpha=" + std::to_string(one_over_alphas[j]) + ", [";
        int prev = 0;
        int success;
        for (int k = 0; k < sizeof(betas) / sizeof(int); k++)
        {
			if (prev >= 9900)
			{
				success = 100000;
			}
			else{
				Simulator s(one_over_alphas[j], betas[k], dlvr);
				success = s.simulate_all(100000);
            }
            prev = success;
            str += std::to_string(success) + ", ";
            fprintf(stderr, "%d %f %d: %d\n", dlvr, one_over_alphas[j], betas[k], success);
        }
        str += "]";
        print_mtx.lock();
        cout << str << endl;
        cout.flush();
        print_mtx.unlock();
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    if (argc > 1)
    {
//	cout << calc_p_rare(1.0 / atof(argv[1]), atoi(argv[2]), 990) << endl;
        Simulator s(atof(argv[1]), atoi(argv[2]), 990);
        cout << s.simulate_all(10000) << endl;
    }
    else
    {
        int dlvrs[] = {995, 990, 900, 800, 500};
        //int dlvrs[] = {990};
        std::thread* th[sizeof(dlvrs) / sizeof(int)];
        for (int i = 0; i < sizeof(dlvrs) / sizeof(int); i++)
        {
            th[i] = new thread(th_given_dlvr, dlvrs[i]);
        }
        for (int i = 0; i < sizeof(dlvrs) / sizeof(int); i++)
        {
            th[i]->join();
            delete th[i];
        }
    }

    return 0;
}
