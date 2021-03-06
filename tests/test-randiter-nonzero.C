
/* tests/test-randiter-nonzero.C
 * Copyright (C) 2001, 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *.
 */


/*! @file  tests/test-randiter-nonzero.C
 * @ingroup tests
 * @brief  no doc
 * @test no doc.
 */



#include "linbox/linbox-config.h"
#include <givaro/givranditer.h>

#include <iostream>
#include <fstream>


#include "linbox/util/commentator.h"
#include "linbox/ring/modular.h"

#include "test-common.h"

using namespace std;
using namespace LinBox;

/* Test 1: Nonzero random elements
 *
 * Construct a sequence of random elements from the nonzero random iterator and
 * check that they are all nonzero
 *
 * F - Field over which to perform computations
 * iterations - Number of iterations to perform
 *
 * Return true on success and false on failure
 */

template <class Field>
static bool testNonzeroRandom (Field &F, unsigned int iterations)
{
	int i;

	commentator().start ("Testing nonzero random elements", "testNonzeroRandom", iterations);

	bool ret = true;

	typename Field::Element x;
	typename Field::RandIter r (F);

	//NonzeroRandIter <Field, typename Field::RandIter> rp (F, r);
	Givaro::GeneralRingNonZeroRandIter <Field> rp (r);

	for (i = 0; i <(int) iterations; i++) {
		commentator().startIteration ((unsigned int)i);

		rp.random (x);

		ostream &report = commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);
		report << "Element: ";
		F.write (report, x);
		report << endl;

		if (F.isZero (x)) {
			ret = false;
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Element is zero" << endl;
		}

		commentator().stop ("done");
		commentator().progress ();
	}

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testNonzeroRandom");

	return ret;
}

int main (int argc, char **argv)
{
	bool pass = true;

	static integer q = 101;
	static unsigned int iterations = 1000;

	static Argument args[] = {
		{ 'q', "-q Q", "Operate over the \"field\" GF(Q) [1].", TYPE_INTEGER, &q },
		{ 'i', "-i I", "Perform each test for I iterations.", TYPE_INT,     &iterations },
		END_OF_ARGUMENTS
	};

	parseArguments (argc, argv, args);
	Givaro::Modular<uint32_t> F (q);

	srand ((unsigned)time (NULL));

	commentator().start("Nonzero random iterator test suite", "Givaro::GeneralRingNonZeroRandIter");

	commentator().setBriefReportParameters (Commentator::OUTPUT_CONSOLE, false, false, false);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (2);

	if (!testNonzeroRandom (F, iterations)) pass = false;

	commentator().stop("Nonzero random iterator test suite");
	return pass ? 0 : -1;
}

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:

