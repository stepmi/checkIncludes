#pragma once

#include <algorithm>
#include <vector>

namespace tools
{

	template <typename T> bool find(const std::vector<T> &a_rContainer, const T &a_rValue)
	{
		return std::find(a_rContainer.begin(), a_rContainer.end(), a_rValue) != a_rContainer.end();
	}

	template <typename T> bool find(const T &a_rContainer, const typename T::key_type &a_rValue)
	{
		return a_rContainer.find(a_rValue) != a_rContainer.end();
	}

}