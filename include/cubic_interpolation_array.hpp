#pragma once

#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>

extern "C" {
#include "cubic_interpolation_array.h"
}

// C++ cubic array class
// this is directly ported from the old code
template <class t1, class t2>
class AutoRhythm_CppCubicArray {
private:
	std::map<t1, t2> p_inputValues;
public:
	// add a value pair as two variables
	void AddValue(t1 position, t2 value) {
		p_inputValues[position] = value;
	}

	// add a value pair as an std::pair
	void AddValue(std::pair<t1, t2> pair) {
		p_inputValues.insert(pair);
	}

	// add a list of std::pairs
	void AddValues(std::initializer_list<std::pair<t1, t2>> pairs) {
		p_inputValues.insert(pairs.begin(), pairs.end());
	}

	// add a vector of std::pairs
	void AddValues(std::vector<std::pair<t1, t2>> pairs) {
		for (int i = 0; i < pairs.size(); i++) {
			p_inputValues.insert(pairs[i]);
		}
	}

	// add a c style array of std::pairs
	void AddValues(std::pair<t1, t2> pairs[], int arraylen) {
		// i severely doubt this one will be used
		for (int i = 0; i < arraylen; i++) {
			p_inputValues.insert(pairs[i]);
		}
	}

	// get interpolated value from position
	t2 GetValue(t1 position) {
		// create vector of keys from input values map so that they can be iterated over
		std::vector<t1> keys;
		for (const auto& kv : p_inputValues)
			keys.push_back(kv.first);

		// find range of values
		int lowerIndex = 0, lowerIndex2 = 0, upperIndex = 0, upperIndex2 = 0;
		for (int i = 0; i < keys.size(); i++) {
			upperIndex = i;
			// upper limit
			if (position <= keys[i] || keys[i] == p_inputValues.rbegin()->first)
				break;
			// increase lower value
			if (position >= keys[i])
				lowerIndex = i;
		}

		// get second indices and clamp those values
		upperIndex2 = std::min(upperIndex + 1, (int)keys.size() - 2); // i need this int cast here because C++
		lowerIndex2 = std::max(lowerIndex - 1, 0);

		// if this is true the given position has a value in the p_inputValues
		if (lowerIndex == upperIndex)
			return p_inputValues[position]; // so we just return that

		// otherwise we have to interpolate(/extrapolate) a value
		float val[4] = {
			p_inputValues[keys.at(lowerIndex2)],p_inputValues[keys.at(lowerIndex)],
			p_inputValues[keys.at(upperIndex)],p_inputValues[keys.at(upperIndex2)]
		};
		return (t2)cubic_interpolation(val, (float)(position - keys.at(lowerIndex)) / (float)(keys.at(upperIndex) - keys.at(lowerIndex)));
	}
};