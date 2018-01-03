#ifndef AL_KMEANS_HPP_INCLUDED
#define AL_KMEANS_HPP_INCLUDED


#include <math.h>
#include <complex>
#include <algorithm>
#include <iostream>
#include <vector>

/**
 * @note  1. set EN_LogMsg to 1 log message, set 0 otherwise.
 */
#define EN_LogMsg                         1u

#if EN_LogMsg > 0u
  #define  __logMsg(...) printf(__VA_ARGS__)
#else
  #define  __logMsg(...) (void *)0
#endif

namespace al_kmeans{

  static void find_minMaxValue(std::vector<double> &v_ce, std::vector<int> &v_other);
  static int find_max_minDistance(std::vector<double> &v_ce, std::vector<int> &v_other, uint32_t k);
  static void run_kmeans_loop(const std::vector<int> &v_other, std::vector<double> &v_ce, std::vector<std::vector<int>> &vv_group);
  static int save_vectorMsg(const std::vector<int> &v_msg);

  int run_kmeans(const std::vector<int> &v_data, uint32_t k)
  {
    if(k <= 1) return -1;
    if(v_data.size() < k) return -2;

    std::vector<int> v_copy;
    v_copy.insert(v_copy.end(), v_data.begin(), v_data.end());

    /* 1.get the init k center elements. */
    std::vector<double> ce_vector;
    ce_vector.clear();
    /* (1) select the max and min value as two of k center elements. */
    find_minMaxValue(ce_vector, v_copy);
    /* (2) if k > 2, find the element with max min_distance among selected center elements. */
    if(k > 2)
    {
      find_max_minDistance(ce_vector, v_copy, k-2);
    }

    __logMsg("\r\n");
    for(auto iter=ce_vector.begin(); iter!=ce_vector.end(); iter++)
    {
      double val = (*iter);
      __logMsg("center %d: %f\r\n", std::distance(ce_vector.begin(), iter), val);
    }
    __logMsg("\r\n");

    /* 2. run k means loop. */
    std::vector<std::vector<int>> group_of_element(k);
    run_kmeans_loop(v_copy, ce_vector, group_of_element);
    v_copy.clear();

    __logMsg("finish\r\n");
  }

  static void find_minMaxValue(std::vector<double> &v_ce, std::vector<int> &v_other)
  {
#if 0
    double min = 10000, max = -10000;
    int min_inx = 0, max_inx = 0;

    for(auto iter = v_other.begin(); iter != v_other.end(); iter++)
    {
      int val = (*iter);
      if(val < min)
      {
        min_inx = ;
        min = val;
      }
      else if(val > max)
      {
        max_inx = m;
        max = val;
      }
    }

    v_ce.push_back(min);
    v_ce.push_back(max);
    if(min_inx < max_inx)
    {
      v_other.erase(v_other.begin() +max_inx);
      v_other.erase(v_other.begin() +min_inx);
    }
    else
    {
      v_other.erase(v_other.begin() +min_inx);
      v_other.erase(v_other.begin() +max_inx);
    }
#else
  auto iter_min = std::min_element(std::begin(v_other), std::end(v_other));
  v_ce.push_back(*iter_min);
  v_other.erase(iter_min);
  auto iter_max = std::max_element(std::begin(v_other), std::end(v_other));
  v_ce.push_back(*iter_max);
  v_other.erase(iter_max);
#endif
  }

  static int find_max_minDistance(std::vector<double> &v_ce, std::vector<int> &v_other, uint32_t k)
  {
    if((v_other.size() == 0) && (k < v_other.size())) return -1;

    while(k--)
    {
      double max = -10000;
      int max_inx = 0;
      for(int m=0; m<v_other.size(); m++)
      {
        int min_distance = 10000;
        for(int n=0; n<v_ce.size(); n++)
        {
          int tmp = std::abs(v_other[m] - v_ce[n]);
          min_distance = (min_distance > tmp)? tmp: min_distance;
        }
        if(max < min_distance)
        {
          max_inx = m;
          max = min_distance;
        }
      }
      v_ce.push_back(v_other[max_inx]);
      v_other.erase(v_other.begin() +max_inx);
    }
  }

  static void run_kmeans_loop(const std::vector<int> &v_other, std::vector<double> &v_ce, std::vector<std::vector<int>> &vv_group)
  {
    static double error_sum = 0, error_last = 0;

    do{
      error_last = error_sum;
      error_sum = 0;
      for(int m=0; m<vv_group.size(); m++)
      {
        vv_group[m].clear();
      }
      /* 1. push all elements into their nearest group vector. */
      for(int m=0; m<v_other.size(); m++)
      {
        int ele_val = v_other[m];
        int min_distance = 10000, min_inx = 0;
        for(int n=0; n<v_ce.size(); n++)
        {
          int distance = std::abs(ele_val - v_ce[n]);
          if(min_distance > distance)
          {
            min_inx = n;
            min_distance = distance;
          }
        }
        vv_group[min_inx].push_back(v_other[m]);
      }
      /* 2. calculate the new center element and the error with the old center element. */
      #if 0
      for(int m=0; m<vv_group.size(); m++)
      {
        double ce_new = 0;
        for(int n=0; n<vv_group[m].size(); n++)
        {
          ce_new += vv_group[m][n];
        }
        ce_new /= vv_group[m].size();
        error_sum += std::abs(ce_new - v_ce[m]);
        v_ce[m] = ce_new;
      }
      #else
      for(auto iter=vv_group.begin(); iter!=vv_group.end(); iter++)
      {
        double ce_new = 0;
        int index = std::distance(vv_group.begin(), iter);

        std::for_each((*iter).begin(), (*iter).end(),
        [&ce_new](int v){
          ce_new += v;
        }
        );
        ce_new /= (*iter).size();
        error_sum += std::abs(ce_new - v_ce[index]);
        v_ce[index] = ce_new;
      }
      #endif

      __logMsg("\r\n");
      for(auto iter=v_ce.begin(); iter!=v_ce.end(); iter++)
      {
        double val = (*iter);
        __logMsg("center %d: %f\r\n", std::distance(v_ce.begin(), iter), val);
      }
      __logMsg("\r\n");

    } while((error_sum - error_last) *100 > error_sum); // if current error <= last error, quit the loop.

//    for(int m=0; m<vv_group.size(); m++)
    {
      std::for_each(vv_group.begin(), vv_group.end(), [](std::vector<int> v){save_vectorMsg(v);});

    }
  }

  static int save_vectorMsg(const std::vector<int> &v_msg)
  {
     std::ofstream out("mst.txt", std::ios::app);
     if(out.is_open())
      {
       out<<"group size:"<<v_msg.size()<<std::endl;
       for(auto iter=v_msg.begin(); iter!=v_msg.end(); iter++)
       {
         int val = (*iter);
         out<<val<<" ";
       }
       out<<std::endl;
       out.close();
     }
  }

}



#endif // AL_KMEANS_HPP_INCLUDED
