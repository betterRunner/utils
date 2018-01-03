#ifndef AL_KM_HPP
#define AL_KM_HPP

#include <cstdio>
#include <algorithm>
#include <vector>

/**************************************************************************************************
 * @brief: This is code of algorithm Kuhn－Munkres.
 * 1. The usage of it is to match each element of one graph A to graph B (such as detection and
 *    track in multiple object tracking), which demands:
 *
 * 2. The core idea of it is:
 *    (1) 让每一个detection和track都找到归属（假设n_detection = n_track）
 *    (2) 在(1)的前提下尽量让相近度高的detection和track匹配到一起
 *    (3) 如果(1)中n_detection不等于n_track，应该在少的一方添加元素（假如是track少）,并设置每个detecion与
 *        新元素的相近度为比较低的值(比如0.01，全完相近为1)
 *
 * @ref  : http://www.cnblogs.com/wenruo/p/5264235.html
 *        (代码和原理都是参考这篇文章)
 *
 *************************************************************************************************/

#define SCORE_NEW			 	            1e-5
#define SCORE_MAX										0x7fffffff

namespace al{
using namespace std;

template <typename T = double>
class al_km_t
{
public:

	al_km_t(const vector<vector<T>> & relationScore)
	{
		if(!relationScore.empty())
		{
			vv_relationScore = relationScore;

			int insert_num = vv_relationScore.size() - vv_relationScore[0].size();
      if(insert_num > 0)
			{
				N = vv_relationScore.size();
				for_each(vv_relationScore.begin(), vv_relationScore.end(), [=](vector<T> & ele){
					for(int m=0; m<insert_num; m++)
					{
						ele.push_back(SCORE_NEW);
					}
				});
			}
			else
			{
				insert_num = -insert_num;
				N = vv_relationScore[0].size();
				for(int m=0; m<insert_num; m++)
				{
          std::vector<T> v_new(N, SCORE_NEW);
          vv_relationScore.push_back(v_new);
				}
			}
			v_expect_t = vector<T>(N);
			v_expect_d = vector<T>(N);
			v_isVisit_t = vector<bool>(N);
			v_isVisit_d = vector<bool>(N);
			v_match_t = vector<int>(N);
			v_slack_t = vector<T>(N);

		}
		else
		{
			N = 0;
		}
	}
	~al_km_t(){}

	vector<int> run()
	{
		if(N > 0)
		{
			fill(v_match_t.begin(), v_match_t.end(), -1);
			fill(v_expect_t.begin(), v_expect_t.end(), 0);

			for(int m=0; m<N; m++) // expect value of each d is the maximun relationship score among t at beginning.
			{
				v_expect_d[m] = vv_relationScore[m][0];
				for(int n=0; n<N; n++)
				{
					v_expect_d[m] = max(v_expect_d[m], vv_relationScore[m][n]);
				}
			}

			for(int d=0; d<N; d++)
			{
				fill(v_slack_t.begin(), v_slack_t.end(), SCORE_MAX);
				while(1)
				{ // the strategy is : do a match among all t,
					fill(v_isVisit_d.begin(), v_isVisit_d.end(), false);
					fill(v_isVisit_t.begin(), v_isVisit_t.end(), false);

					if(_is_match(d)) break; // match, then break.

					T min_sub = SCORE_MAX;
					for(int n=0; n<N; n++)
					{
						if(!v_isVisit_t[n]) min_sub = min(min_sub, v_slack_t[n]);
					}
					printf("min_sub is:%.3f\r\n", min_sub);
					for(int n=0; n<N; n++)
					{
						if(!v_isVisit_d[n]) v_expect_d[n] -= min_sub;
						if(v_isVisit_t[n]) v_expect_t[n] += min_sub;
						else v_expect_t[n] -= min_sub;
					}
				}
			}
		}
    return v_match_t;
	}

private:
  bool _is_match(int d)
  {
  	v_isVisit_d[d] = true;
  	for(int t=0; t<N; t++)
		{
      if(v_isVisit_t[t]) continue;

      T gap = v_expect_d[d] + v_expect_t[t] - vv_relationScore[d][t];
      printf("gap of d:%d and t:%d %.3f\r\n", d, t, gap);
      if(gap <= 0)
			{
				printf("d:%d and t:%d are suitable\r\n", d, t);
				v_isVisit_t[t] = true;
				if((v_match_t[t] == -1) || _is_match(v_match_t[t]))
				{
					printf("d:%d and t:%d are match\r\n", d, t);
					v_match_t[t] = d;
					return true;
				}
			}
			else
			{
				v_slack_t[t] = min(v_slack_t[t], gap);
			}
		}

  	return false;
  }

	int N;
	vector<vector<T>> vv_relationScore;
	vector<T> v_expect_t;
	vector<T> v_expect_d;
	vector<bool> v_isVisit_t;
	vector<bool> v_isVisit_d;
	vector<int> v_match_t;
	vector<T> v_slack_t;
};

} // namespace al

#endif

