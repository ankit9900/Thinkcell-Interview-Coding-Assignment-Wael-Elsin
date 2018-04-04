// Copyright 2017-2018 Will (Waël) Elsin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// thinkcell-cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

// BEGIN THINKCELL CODE

#include <assert.h>
#include <map>
#include <limits>

// Wael Note: Should use the std namespace to make the references more
//            readable. Kept out since it wasn't included in the original
//            assignment code

// using namespace std;

void IntervalMapTest(void);

template <class K, class V>
class interval_map {
    friend void IntervalMapTest();

private:
    std::map<K, V> m_map;

public:
    // constructor associates whole range of K with val by inserting (K_min, val)
    // into the map

    // Wael: Seems that there is a discrepency with the original code from think-cell
    //       Only the first element in the map is set, and not the whole range
    //       as is mentioned in the description above!

    interval_map(V const& val)
    {

        // Wael Note: Possible Error in constructor, does not initialize whole range of K to V
        //            as is stated in the assignment specification. Will handle in
        //            the first call to Assign since I have no way of submitting
        //            code outside of the assign member function in the online
        //            submission form
        //
        // After some experimentation, I noticed that it's not advisable to physically set
        // the whole range of the possible keys to the initialValue since it would take too
        // long (atleast in Debug mode), so perhaps the way that the constructor was intentional,
        // thus I will be filling in key-range gaps with the initial value will be handled in
        // the assign. Implementing the algorithm of key-range gap dynamic initialValue filling,
        // which ensures that the map is filled with the initialValue only when needed, will save
        // memory and execution time

        m_map.insert(m_map.begin(), std::make_pair(std::numeric_limits<K>::lowest(), val));
    }

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    // Do not change values outside this interval.
    // Conforming to the C++ Standard Library conventions, the interval
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval,
    // and assign must do nothing.
    void assign(K const& keyBegin, K const& keyEnd, V const& val)
    {
        // Store the initial value so it can be referenced later to
        // to fill in values for gaps in specified key ranges dynamically

        // Ideally, the initialValue should be stored as a member in the class
        // but since I can only submit the body of the assign method
        // implementing this workaround where the first key value can
        // never change after being set by the constructor

        V initialValue = m_map.begin()->second;

        // Define an iterator variable to use as the key index
        std::map<K, V>::iterator keyIdx;

        // Wael Note: We should handle exceptions via try/catch
        //           Not implementing since original assignment code
        //           does not include needed exception header
        //           to print the exception details

        // This is a workaround. See initialValue above
        // Keep the initial value at the time of calling the constructor unchanged
        // This is needed to fill in the values for the key-range gaps
        if (keyBegin == 0) {
            std::cout << "Error: Can't change the first key in the list since we need to know the first value for padding purposes" << std::endl;
            return;
        }

        // Handle the native C++ type overflow case first, the specified key type.
        // This occurs for example when giving a signed "int" value of -1 as a key
        // when the specified type in the constructor is "unsigned int"

        if (keyBegin >= (std::numeric_limits<K>::max() - keyEnd)) {
            std::cout << "Error: Lowerbound Overflow detected. Specified key value is incorrect for the given type" << std::endl;
            return;
        }

        if (keyEnd < (keyBegin - std::numeric_limits<K>::lowest())) {
            std::cout << "Error: Upperbound Overflow detected. Specified key value is incorrect for the given type" << std::endl;
            return;
        }

        // If !( keyBegin < keyEnd ), this designates an empty interval, and assign must do nothing
        if (!(keyBegin < keyEnd)) {
            std::cout << "Error: keyBegin is not less than keyEnd" << std::endl;
            return;
        }

        // Verify that the specified KeyBegin is within the allowed limits
        if (keyBegin < std::numeric_limits<K>::lowest() || keyBegin >= std::numeric_limits<K>::max()) {
            std::cout << "Error: keyBegin is not within allowed limits" << std::endl;
            return;
        }

        // Verify that the specified KeyEnd is within the allowed limits
        if (keyEnd <= std::numeric_limits<K>::lowest() || keyEnd > std::numeric_limits<K>::max()) {
            std::cout << "Error: keyEnd is not within allowed limits" << std::endl;
            return;
        }

        // The basic key-range checks passed

        // Now we verify adjancency checks
        keyIdx = m_map.upper_bound(keyBegin);
        if (keyIdx != m_map.begin()) {
            if ((--keyIdx)->first != keyBegin) {
                // Check the lower bound adjacency
                keyIdx = m_map.lower_bound(keyBegin);

                // Check if keyBegin already exists, otherwise we're ok
                if (keyIdx != m_map.begin()) {
                    // Go to the lower adjacent value
                    keyIdx--;
                    if (keyIdx->second == val) {
                        std::cout << "Error: Lower adjacent value is the same as given value for specified key range" << std::endl;
                        return;
                    }

                    // This will work for existing key ranges and across key-range gaps as well
                    if (keyIdx->first < keyBegin) {
                        if (val == initialValue) {
                            std::cout << "keyBegin value cannot equal the initialValue of '" << initialValue << "'\n";
                            return;
                        }
                    }
                }
            }
        }
        // Check the upper bound adjacency
        keyIdx = m_map.upper_bound(keyEnd);
        if (keyIdx != m_map.end()) {
            // Case of embedded before the end
            if (keyIdx->second == val) {
                std::cout << "Error: Higher adjacent value is the same as given value for specified key range" << std::endl;
                return;
            }
        }
        else {
            // Get the last key in the map
            keyIdx = m_map.lower_bound(std::numeric_limits<K>::max());
            if (keyIdx != m_map.begin()) {
                keyIdx--;
                if (keyIdx->second == val && keyEnd <= keyIdx->first) {
                    // Case of embedded at the end of the map (I.E. Border case)
                    std::cout << "Error: Higher adjacent (border) value is the same as given value for specified key range" << std::endl;
                    return;
                }
            }
            // Two seperate ifs since one doesn't really depend on the other
            if (initialValue == val) {
                // Detects the gap case here where the keyEnd goes into a key-range gap,
                // where all gap values are set to the initialValue implicitly, untill
                // the gap is filled

                std::cout << "Error: Higher adjacent value (initialValue) is the same as given value for specified key range" << std::endl;
                return;
            }
        }

        // All checks passed

        // Now Populate the key-range gap only if all checks passed (valid key range and adjacency)
        // This will save memory & time; as opposed to populating values when
        // a gap is deteced when the boundary conditions are not met
        // It's the time savings that are noticable when the range is big
        m_map.lower_bound(keyBegin);

        if (keyIdx != m_map.begin()) {
            // Detect a gap
            if (keyIdx->first < keyBegin) {

                unsigned long long numOfKeys = (unsigned long long)(keyEnd - keyIdx->first);
                // Wael Note: Amazing what proper testing can uncover!
                //            Below is the fixed code. There was an error initially.
                //            Fixed to handle the immediate key adjacency properly so it
                //            doesn't get reported as a false gap. This was setting the map
                //            range unneccessarily to the initialValue which would then be
                //            overwritten in the map assign code. This is why I missed it.
                //            Uncovered through the testing and the code below is the fixed code
                if (numOfKeys > 1 && keyIdx->first != (keyBegin - 1)) {
                    std::cout << "Gap detected in key range. Autofilling key-range gap with initialValue '" << initialValue << "'\n";

                    K initialKeyInRange = keyIdx->first;

                    for (unsigned long long j = 1; j < numOfKeys; ++j) {

                        m_map.insert_or_assign(++initialKeyInRange, initialValue);
                    }
                }
            }
        }

        // Populate the map
        // Use the largest possible unsigned variable type to accomodate
        // for any native c++ type that the map template is constructed with
        unsigned long long numOfKeys = (unsigned long long)(keyEnd - keyBegin);
        K initialKeyInRange = keyBegin;

        for (unsigned long long i = 1; i <= numOfKeys; ++i) {

            m_map.insert_or_assign(initialKeyInRange++, val);
        }
    }

    // Member function to print the whole member m_map
    void printMap(void)
    {
        std::cout << std::endl
                  << "Printing Map:" << std::endl;
        for (std::map<K, V>::iterator it = m_map.begin(); it != m_map.end(); ++it) {
            std::cout << it->first << "," << it->second << std::endl;
        }

        std::cout << std::endl;
    }

    // look-up of the value associated with key
    V const& operator[](K const& key) const
    {
        return (--m_map.upper_bound(key))->second;
    }

    // Wael Note: Are additional operators really required?
};

// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a function IntervalMapTest() here that tests the
// functionality of the interval_map, for example using a map of unsigned int
// intervals to char.

int main()
{
    IntervalMapTest();
}

void IntervalMapTest(void)
{
    //Positive Testcases
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "// POSITIVE TESTCASES" << std::endl;
    std::cout << "//" << std::endl
              << std::endl;

    // 1.	Initialize the Map
    std::cout << "1.	Initialize Map" << std::endl
              << std::endl;
    std::cout << "IntervalMap('A')" << std::endl;
    // Note: Create tests and repeat them with different K,V data types
    interval_map<unsigned int, char> myPosIntervalMap('A');
    myPosIntervalMap.printMap();

    // 2.	Change the first element
    std::cout << "2.	Change the first element" << std::endl
              << std::endl;
    std::cout << "Assign(1, 2, 'B')" << std::endl;
    myPosIntervalMap.assign(1, 2, 'B');
    myPosIntervalMap.printMap();

    // 3.	Assign a valid key/value range including the first element
    std::cout << "3.	Assign a valid key/value range including the first element" << std::endl
              << std::endl;
    std::cout << "Assign(1, 3, 'C')" << std::endl;
    myPosIntervalMap.assign(1, 3, 'C');
    myPosIntervalMap.printMap();

    // 4.	Append a valid key/value range to the map with no gap
    std::cout << "4.	Append a valid key/value range to the map with no gap" << std::endl
              << std::endl;
    std::cout << "Assign(3, 6, 'D')" << std::endl;
    myPosIntervalMap.assign(3, 6, 'D');
    myPosIntervalMap.printMap();

    // 5.	Overwrite entire existing key/value range with the same value
    std::cout << "5.	Overwrite entire existing key/value range with the same value" << std::endl
              << std::endl;
    std::cout << "Assign(3, 6, 'D')" << std::endl;
    myPosIntervalMap.assign(3, 6, 'D');
    myPosIntervalMap.printMap();

    // 6.	Append a valid key/value range to the map with a gap
    std::cout << "6.	Append a valid key/value range to the map with a gap" << std::endl
              << std::endl;
    std::cout << "Assign(9, 12, 'E')" << std::endl;
    myPosIntervalMap.assign(9, 12, 'E');
    myPosIntervalMap.printMap();

    // 7.	Insert a valid key/value range into the middle of the map
    std::cout << "7.	Insert a valid key/value range into the middle of the map" << std::endl
              << std::endl;
    std::cout << "Assign(5, 8, 'F')" << std::endl;
    myPosIntervalMap.assign(5, 8, 'F');
    myPosIntervalMap.printMap();

    // 8.	Insert a valid key/value range to overlap the end of the map
    std::cout << "8.	Insert a valid key/value range to overlap the end of the map" << std::endl
              << std::endl;
    std::cout << "Assign(10, 13, 'G')" << std::endl;
    myPosIntervalMap.assign(10, 13, 'G');
    myPosIntervalMap.printMap();

    // 9.	Extend an existing key/value range with the same value
    std::cout << "9.	Extend an existing key/value range with the same value" << std::endl
              << std::endl;
    std::cout << "Assign(3, 8, 'D')" << std::endl;
    myPosIntervalMap.assign(3, 8, 'D');
    myPosIntervalMap.printMap();

    //Negative Testcases
    std::cout << std::endl;
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "// NEGATIVE TESTCASES" << std::endl;
    std::cout << "//" << std::endl
              << std::endl;

    // 1.	Initialize the Map
    std::cout << "Initialize Map" << std::endl;
    std::cout << "1.	Initialize Map" << std::endl;
    std::cout << "IntervalMap('A')" << std::endl;
    // Note: Create tests and repeat them with different K,V data types
    interval_map<unsigned int, char> myNegIntervalMap('A');
    myNegIntervalMap.printMap();

    // 2.	Try to assign a keyBegin for the first key with a valid keyEnd
    std::cout << "2.	Try to assign a keyBegin for the first key with a valid keyEnd" << std::endl
              << std::endl;
    std::cout << "Assign(0, 5, 'A')" << std::endl;
    myNegIntervalMap.assign(0, 5, 'A');
    myNegIntervalMap.printMap();

    // 3.	Try to assign a keyBegin and keyEnd equal to first valid key
    std::cout << "3.	Try to assign a keyBegin and keyEnd equal to first valid key" << std::endl
              << std::endl;
    std::cout << "Assign(0, 0, 'B')" << std::endl;
    myNegIntervalMap.assign(0, 0, 'B');
    myNegIntervalMap.printMap();

    // 4.	Try to assign a keyBegin below the lowest valid key with keyEnd at first value
    std::cout << "4.	Try to assign a keyBegin below the lowest valid key with keyEnd at first value" << std::endl
              << std::endl;
    std::cout << "Assign(-1, 1, 'B')" << std::endl;
    myNegIntervalMap.assign(-1, 1, 'B');
    myNegIntervalMap.printMap();

    // 5.	Try to assign a keyBegin below the lowest valid key with keyEnd valid
    std::cout << "5.	Try to assign a keyBegin below the lowest valid key with keyEnd valid" << std::endl
              << std::endl;
    std::cout << "Assign(-1, 3, 'B')" << std::endl;
    myNegIntervalMap.assign(-1, 3, 'B');
    myNegIntervalMap.printMap();

    // 6.	Try to assign a keyBegin and keyEnd below the lowest valid key
    std::cout << "6.	Try to assign a keyBegin and keyEnd below the lowest valid key" << std::endl
              << std::endl;
    std::cout << "Assign(-2, -1, 'B')" << std::endl;
    myNegIntervalMap.assign(-2, -1, 'B');
    myNegIntervalMap.printMap();

    // 7.	Try to assign a keyBegin and keyEnd equal to first allowed key
    std::cout << "7.	Try to assign a keyBegin and keyEnd equal to first allowed key" << std::endl
              << std::endl;
    std::cout << "Assign(1, 1, 'B')" << std::endl;
    myNegIntervalMap.assign(1, 1, 'B');
    myNegIntervalMap.printMap();

    // 8.	Try to assign a keyBegin and keyEnd below the lowest allowed key
    std::cout << "8.	Try to assign a keyBegin and keyEnd below the lowest allowed key" << std::endl
              << std::endl;
    std::cout << "Assign(-1, 0, 'B')" << std::endl;
    myNegIntervalMap.assign(-1, 0, 'B');
    myNegIntervalMap.printMap();

    // 9.	Try to assign keyBegin within allowed key range with keyEnd after the max allowed
    std::cout << "9.	Try to assign keyBegin within allowed key range with keyEnd after the max allowed" << std::endl
              << std::endl;
    std::cout << "Assign(1, std::numeric_limits<unsigned int>::max() + 1, B)" << std::endl;
    myNegIntervalMap.assign(1, std::numeric_limits<unsigned int>::max() + 1, 'B');
    myNegIntervalMap.printMap();

    // 10.	Validate adjacent key values cannot be the same for lowest border
    std::cout << "10.	Validate adjacent key values cannot be the same for lowest border" << std::endl
              << std::endl;
    std::cout << "Assign(1, 2, 'A')" << std::endl;
    myNegIntervalMap.assign(1, 2, 'A');
    myNegIntervalMap.printMap();

    // 11.	Validate adjacent key values cannot be the same for adjacent border no gap
    std::cout << "11.	Validate adjacent key values cannot be the same for adjacent border no gap" << std::endl
              << std::endl;
    std::cout << "Assign(1, 3, 'B')" << std::endl;
    myNegIntervalMap.assign(1, 3, 'B');
    myNegIntervalMap.printMap();

    std::cout << "Assign(3, 5, 'B')" << std::endl;
    myNegIntervalMap.assign(3, 5, 'B');
    myNegIntervalMap.printMap();

    // 12.	Validate adjacent key values cannot be the same for adjacent border with gap
    std::cout << "12.	Validate adjacent key values cannot be the same for adjacent border with gap" << std::endl
              << std::endl;
    std::cout << "Assign(5, 7, 'A')" << std::endl;
    myNegIntervalMap.assign(5, 7, 'A');
    myNegIntervalMap.printMap();

    // 13.	Validate adjacent key values cannot be the same for lower gap border
    std::cout << "13.	Validate adjacent key values cannot be the same for lower gap border" << std::endl
              << std::endl;
    std::cout << "Assign(1, 2, 'A')" << std::endl;
    myNegIntervalMap.assign(5, 7, 'A');
    myNegIntervalMap.printMap();

    // 14.	Validate adjacent key values cannot be the same for upper gap border
    std::cout << "14.	Validate adjacent key values cannot be the same for upper gap border" << std::endl
              << std::endl;
    std::cout << "Assign(2, 3, 'A')" << std::endl;
    myNegIntervalMap.assign(2, 3, 'A');
    myNegIntervalMap.printMap();

    // 15.	Validate adjacent key values cannot be the same for upper embedded border before end
    std::cout << "15.	Validate adjacent key values cannot be the same for upper embedded border before end" << std::endl
              << std::endl;
    std::cout << "Assign(1, 5, 'B')" << std::endl;
    myNegIntervalMap.assign(1, 5, 'B');
    myNegIntervalMap.printMap();

    std::cout << "Assign(1, 3, 'B')" << std::endl;
    myNegIntervalMap.assign(1, 3, 'B');
    myNegIntervalMap.printMap();

    // 16.	Validate adjacent key values cannot be the same for upper embedded border at end
    std::cout << "16.	Validate adjacent key values cannot be the same for upper embedded border at end" << std::endl
              << std::endl;
    std::cout << "Assign(1, 4, 'B')" << std::endl;
    myNegIntervalMap.assign(1, 4, 'B');
    myNegIntervalMap.printMap();
}

// END THINKCELL CODE

