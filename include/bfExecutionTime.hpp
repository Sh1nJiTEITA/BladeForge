#ifndef BF_EXECUTION_TIME_H
#define BF_EXECUTION_TIME_H

#include <forward_list>
#include <list>
#include <string>
#include <chrono>
#include <algorithm>

#define BF_TIME_POINT std::chrono::steady_clock::time_point

typedef struct BfExecutionTime {
	static std::forward_list<std::pair<std::string, std::pair<BF_TIME_POINT, BF_TIME_POINT>>> all_time_cuts;
	static std::list<std::pair<std::string, std::pair<BF_TIME_POINT, BF_TIME_POINT>>> now_time_cuts;


	static void BeginTimeCut(std::string name);
	static void EndTimeCut(std::string name);
	static float GetAverageTimeCut(std::string name);
	static float GetMedianTimeCut(std::string name);

} BfET;

inline std::forward_list<std::pair<std::string, std::pair<BF_TIME_POINT, BF_TIME_POINT>>> BfExecutionTime::all_time_cuts{};

inline std::list <std::pair<std::string, std::pair<BF_TIME_POINT, BF_TIME_POINT>>> BfExecutionTime::now_time_cuts{};



inline void BfExecutionTime::BeginTimeCut(std::string name) {
	auto current_time = std::chrono::high_resolution_clock::now();
	
	BfExecutionTime::now_time_cuts.push_back(std::make_pair(name, std::make_pair(current_time, BF_TIME_POINT())));
}

inline void BfExecutionTime::EndTimeCut(std::string name) {

	std::pair<BF_TIME_POINT, BF_TIME_POINT> done_pair;
	for (auto it = BfExecutionTime::now_time_cuts.begin(); it != (BfExecutionTime::now_time_cuts.end());) {
		if (it->first == name) {
			it->second.second = std::chrono::high_resolution_clock::now();
			done_pair = it->second;
			it = BfExecutionTime::now_time_cuts.erase(it);
		}
		else {
			++it;
		}
	}


	BfExecutionTime::all_time_cuts.push_front(std::make_pair(name, done_pair));
}

inline float BfExecutionTime::GetAverageTimeCut(std::string name)
{
	int delta_count = 1;
	long long sum = 0;
	for (const auto& it : BfExecutionTime::all_time_cuts) {
		if (it.first == name) {
			
			auto delta = std::chrono::duration_cast<std::chrono::microseconds>(it.second.second - it.second.first);
			sum += delta.count();
			delta_count++;
		}
	}
	
	if (delta_count == 1)
		return -1.0f;
	return (float)(sum / delta_count);
}

inline float BfExecutionTime::GetMedianTimeCut(std::string name)
{
	std::vector<std::pair<BF_TIME_POINT, BF_TIME_POINT>> local_cuts;
	for (const auto& it : BfExecutionTime::all_time_cuts) {
		if (it.first == name) {
			
			local_cuts.push_back(it.second);

		}
	}
	
	std::sort(local_cuts.begin(), local_cuts.end(), [](const auto& lhs, const auto& rhs) {return lhs.first < rhs.first; });

	if (local_cuts.empty())
		return -1.0;

	std::pair<BF_TIME_POINT, BF_TIME_POINT> needed_pair = local_cuts[(int)(local_cuts.size() / 2)];
	auto delta = std::chrono::duration_cast<std::chrono::microseconds>(needed_pair.second - needed_pair.first);

	return delta.count();
}



#endif