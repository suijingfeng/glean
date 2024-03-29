// BEGIN_COPYRIGHT -*- glean -*-
// 
// Copyright (C) 2000  Allen Akin   All Rights Reserved.
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the
// Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
// KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ALLEN AKIN BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 
// END_COPYRIGHT




// tbinding.h:  Test functions in the window-system binding

#ifndef __tbinding_h__
#define __tbinding_h__

#include "tbasic.h"

namespace GLEAN {

class DrawingSurfaceConfig;		// Forward reference.
class Window;

#define drawingSize 128

class MakeCurrentResult: public BaseResult {
public:
	bool pass;
	// Short descriptions of the rendering contexts:
	vector<const char*> descriptions;
	// Complete record of rendering contexts made "current" during
	// the test:
	vector<int> testSequence;

	void putresults(ostream& s) const {
		s << pass << '\n';
	}
	
	bool getresults(istream& s) {
		s >> pass;
		return s.good();
	}
};

class MakeCurrentTest: public BaseTest<MakeCurrentResult> {
public:

	MakeCurrentTest(const char* aName, const char* aFilter, const char* aDescription):
		BaseTest<MakeCurrentResult>(aName, aFilter, aDescription) {
		fWidth  = drawingSize;
                fHeight = drawingSize;
                testOne = false;
		std::cout << " MakeCurrentTest constructor 1" << std::endl;
	}

	MakeCurrentTest(const char* aName, const char* aFilter, Test** thePrereqs, const char* aDescription):
		BaseTest<MakeCurrentResult>(aName, aFilter, thePrereqs, aDescription) {
                fWidth  = drawingSize;
                fHeight = drawingSize;
                testOne = false;
		std::cout << " MakeCurrentTest constructor 2" << std::endl;
	}

	MakeCurrentTest(const char* aName, const char* aFilter,
	     const char* anExtensionList,
	     const char* aDescription):
		BaseTest<MakeCurrentResult>(aName, aFilter, anExtensionList, aDescription) {
                fWidth  = drawingSize;
                fHeight = drawingSize;
                testOne = false;
		std::cout << " MakeCurrentTest constructor 3" << std::endl;
	}

	virtual ~MakeCurrentTest() {}

	virtual void runOne(MakeCurrentResult& r, Window& w);
	virtual void compareOne(MakeCurrentResult& oldR, MakeCurrentResult& newR);
	virtual void logOne(MakeCurrentResult& r);

}; // class MakeCurrentTest

} // namespace GLEAN

#endif // __tbinding_h__
