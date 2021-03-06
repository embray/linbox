/* Copyright (C) LinBox
 *
 *  Author: Zhendong Wan
 *
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
  * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */


/*! @file  tests/test-bitonic-sort.C
 * @ingroup tests
 * @brief no doc.
 * @test NO DOC
 */



#include <algorithm>
#include "linbox/algorithms/bitonic-sort.h"
#include "linbox/util/commentator.h"
#include "linbox/vector/stream.h"
#include "test-common.h"
#include "linbox/vector/blas-vector.h"
//#include "linbox/field/unparametric.h"

using namespace LinBox;

class Comparator {
public:

	void operator()(int& min, int& max) const {

		if (min > max)

			std::swap (min, max);
	}

};

bool testRandom (std::ostream& report, size_t s)
{

	using namespace std;

	commentator().start ("test bitonic sort", "test bitonic sort");

	bool ret = true;

	Comparator comp;

	typedef Givaro::ZRing<int> Ints ;
	Ints	I ;

	BlasVector<Ints> v(I,s), d(I,s);
	BlasVector<Ints>::iterator p1, p2;

	for (p1 = v. begin(), p2 = d.begin(); p1 != v.end(); ++ p1, ++p2)
		*p1 = *p2 = rand();

	report << "Input vector:  ";


	for (p1 = v.begin(); p1 != v.end(); ++ p1)
		report << *p1 << " ";
	report << endl;

	bitonicSort(v.begin(), v.end(), comp);

	report << "Computed sequence by bitonic sorting network:\n";

	for (p1 = v.begin(); p1 != v.end(); ++ p1)
		report << *p1 << " ";
	report << '\n';

	stable_sort(d.begin(), d.end());

	report << "Expected sequence after sorting:\n";

	for (p1 = d.begin(); p1 != d.end(); ++ p1)
		report << *p1 << " ";
	report << '\n';

	for (p1 = v.begin(), p2 = d. begin(); p1 != v.end(); ++ p1, ++ p2)
		if (*p1 != *p2) {
			ret = false;
			break;
		}

	if (!ret)
		commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
		<< "ERROR: Computed Smith form is incorrect" << endl;

	commentator().stop (MSG_STATUS (ret), NULL, "testRandom");

	return ret;
}

int main(int argc, char** argv)
{
	using namespace LinBox;

	bool pass = true;

	static size_t n = 512;

	static Argument args[] = {
		{ 'n', "-n N", "Set size of sequence to N.  N must be a power of 2)",  TYPE_INT,     &n },
		END_OF_ARGUMENTS
	};
	parseArguments (argc, argv, args);

	commentator().start("Sort network test suite", "bitonic sort");
	std::ostream& report = commentator().report();
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (5);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_UNIMPORTANT);

	if (!testRandom(report, n)) pass = false;
	commentator().stop("sort network test suite");

	return pass ? 0 : -1;
}

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:

