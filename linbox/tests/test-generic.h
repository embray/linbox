/* -*- mode: c; style: linux -*- */

/* linbox/tests/test-generic.h
 * Copyright (C) 2001, 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __TEST_GENERIC_H
#define __TEST_GENERIC_H

#include <iostream>
#include <fstream>
#include <vector>

#include "linbox/field/vector-domain.h"
#include "linbox/blackbox/archetype.h"
#include "linbox/integer.h"

#include "test-common.h"

/* Generic test 1: Application of transpose of a matrix
 *
 * Take the given black box and compute u^T A v via <A^T u, v> and <u, Av> for
 * randomly chosen u and v; check whether the results are equal. In theory, this
 * should guarantee that tranpose is working correctly if apply and dot product
 * are also working correctly. Apply and dot product should, of course, be
 * independently checked.
 *
 * F - Field over which to perform computations
 * A - Black box of which to construct the transpose
 * report - Stream to which to output detailed report of failures, if any
 * iterations - Number of random vectors to which to apply matrix
 *
 * Return true on success and false on failure
 */

template <class Field>
static bool
testTranpose (Field                                                               &F,
	      LinBox::Blackbox_archetype <std::vector <typename Field::element> > &A,
	      ostream                                                             &report,
	      int                                                                  iterations) 
{
	typedef vector <typename Field::element> Vector;

	cout << "Testing tranpose apply...";
	cout.flush ();
	report << "Testing tranpose apply:" << endl;

	bool ret = true;

	int i, j;

	Vector u(A.rowdim ()), v(A.coldim ()), w(A.coldim ());
	LinBox::VectorDomain <Field, Vector, Vector> VD (F);
	typename Field::RandIter r (F);
	typename Field::element r1, r2;

	for (i = 0; i < iterations; i++) {
		report << "  Iteration " << i << ": " << endl;

		for (j = 0; j < A.coldim (); j++) {
			r.random (u[j]);
			r.random (v[j]);
		}

		report << "    Input vector u:  ";
		printVector<Field> (F, report, u);

		report << "    Input vector v:  ";
		printVector<Field> (F, report, v);

		A.apply (w, v);

		report << "    Result of apply:  ";
		printVector<Field> (F, report, w);

		VD.dotprod (r1, u, w);

		A.applyTranspose (w, u);

		report << "    Result of tranpose apply:  ";
		printVector<Field> (F, report, w);

		VD.dotprod (r2, w, v);

		report << "    <u, Av>:  ";
		F.write (report, r1);
		report << endl;

		report << "    <A^T u, v>:  ";
		F.write (report, r2);
		report << endl;

		if (!F.areEqual (r1, r2)) {
			ret = false;
			report << "    ERROR: Vectors are not equal" << endl;
		}
	}

	if (ret) {
		cout << "passed" << endl;
		report << "Test passed" << endl << endl;
	} else {
		cout << "FAILED" << endl;
		report << "Test FAILED" << endl << endl;
	}

	cout.flush ();

	return ret;
}

#endif // __TEST_GENERIC_H
