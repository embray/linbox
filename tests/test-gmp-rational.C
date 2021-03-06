
/* tests/test-gmp-rational.C
 * Copyright (C) 2001, 2002 Bradford Hovinen,
 * Copyright (C) 2002 Dave Saunders
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>,
 *            Dave Saunders <saunders@cis.udel.edu>
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


/*! @file  tests/test-gmp-rational.C
 * @ingroup tests
 * @brief  no doc
 * @test NO DOC
 */

#include "linbox/linbox-config.h"

#include <iostream>
#include <fstream>


#include "linbox/field/gmp-rational.h"

#include "test-common.h"
//#include "test-generic.h"
#include "test-field.h"

using namespace LinBox;

int main (int argc, char **argv)
{
	static size_t n = 1000;
	static int iterations = 1;

	static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test vectors to NxN.", TYPE_INT, &n },
		{ 'i', "-i I", "Perform each test for I iterations.",     TYPE_INT, &iterations },
		END_OF_ARGUMENTS
	};

	parseArguments (argc, argv, args);

	commentator().start("GMP rational field test suite", "GMPRationalField");
	bool pass = true;

	GMPRationalField F;

	// Make sure some more detailed messages get printed
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (4);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_UNIMPORTANT);

	if (!runFieldTests (F,  "GMP Rational",  (unsigned int)iterations, n, false,false)) pass = false;

	commentator().stop("GMP rational field test suite");
	return pass ? 0 : -1;
}

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
