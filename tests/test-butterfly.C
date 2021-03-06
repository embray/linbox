
/* tests/test-butterfly.C
 * Copyright (C) 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * --------------------------------------------------------
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


/*! @file  tests/test-butterfly.C
 * @ingroup tests
 * @brief  no doc
 * @test NO DOC
 */



#include "linbox/linbox-config.h"

#include <iostream>
#include <fstream>


#include "linbox/ring/modular.h"
#include "linbox/vector/vector-domain.h"
#include <givaro/givranditer.h>
#include "linbox/util/commentator.h"
#include "linbox/vector/stream.h"
#include "linbox/blackbox/compose.h"
#include "linbox/blackbox/diagonal.h"
//#include "linbox/matrix/sparse-matrix.h"
#include "linbox/blackbox/submatrix.h"
#include "linbox/solutions/det.h"
#include "linbox/blackbox/butterfly.h"

#include "test-blackbox.h"

using namespace LinBox;
using namespace std;

/* Test 1: setButterfly/BooleanSwitch
 *
 * Return true on success and false on failure
 */

template <class Field, class Vector>
static bool testSetButterfly (const Field &F, VectorStream<Vector> &stream, size_t k)
{
	commentator().start ("Testing setButterfly", "testSetButterfly", stream.size ());

	bool ret = true;

	// unsigned long real_k;

	Vector v_p;
	BlasVector<Field> w (F,stream.dim ()), v1 (F,stream.dim ());
	VectorDomain<Field> VD (F);

	while (stream) {
		commentator().startIteration ((unsigned int)stream.j ());

		stream >> v_p;
		BlasVector<Field> v (F,stream.n ());
		VD.copy (v, v_p);

		unsigned long real_k = v_p.first.size ();

		ostream &report = commentator().report (Commentator::LEVEL_UNIMPORTANT, INTERNAL_DESCRIPTION);
		report << "Input vector: ";
		VD.write (report, v) << endl;

		std::vector<bool> z (stream.n ());

		for (typename Vector::first_type::iterator iter = v_p.first.begin (); iter != v_p.first.end (); ++iter)
			z[*iter] = true;

		vector<bool> z_vec = setButterfly (z);

		LinBox::BooleanSwitchFactory factory (z_vec);
		Butterfly<Field, BooleanSwitch> P (F, stream.dim (), factory);

		P.apply (w, v);

		report << "Result of apply: ";
		VD.write (report, w) << endl;

		P.applyTranspose (v1, w);

		report << "Result of applyTranspose: ";
		VD.write (report, v1) << endl;

		bool iter_passed = true;

		for (size_t i = 0; i < real_k; ++i)
			if (F.isZero (w[i]))
				ret = iter_passed = false;

		if (!iter_passed)
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Initial block contains zeros" << endl;

		iter_passed = true;

		for (size_t i = real_k; i < v.size (); ++i)
			if (!F.isZero (w[i]))
				ret = iter_passed = false;

		if (!iter_passed)
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Nonzero entries outside initial block" << endl;

		if (!VD.areEqual (v, v1)) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: P^T != P^-1" << endl;
			ret = false;
		}

		commentator().stop ("done");
		commentator().progress ();
	}

	stream.reset ();

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testSetButterfly");

	return ret;
}

/* Test 2: Cekstv switch
 *
 * Return true on success and false on failure
 */

template <class Field>
static bool testCekstvSwitch (const Field &F, unsigned int iterations, size_t n, size_t r)
{
	commentator().start ("Testing cekstv switch", "testCekstvSwitch", iterations);

	bool ret = true;

	unsigned int failures = 0;

	typename Vector<Field>::SparsePar d1;
    typename Field::RandIter gen(F);
	RandomSparseStream<Field, typename Vector<Field>::SparsePar> stream (F, gen, (double) r / (double) n, n, iterations);
	VectorDomain<Field> VD (F);

	// unsigned long real_r;
	typename Field::Element det_Ap;


	while (stream) {
		commentator().startIteration ((unsigned int)stream.pos ());

		stream >> d1;

		BlasVector<Field> d (F,n);
		VD.copy (d, d1);

		ostream &report = commentator().report (Commentator::LEVEL_UNIMPORTANT, INTERNAL_DESCRIPTION);
		report << "Input vector: ";
		VD.write (report, d) << endl;

		unsigned long real_r = d1.first.size ();

		report << "Real rank: " << real_r << endl;

		typename Field::RandIter rdtr (F);
		typename CekstvSwitch<Field>::Factory factory (rdtr);
		Butterfly<Field, CekstvSwitch<Field> > P (F, n, factory);
		Butterfly<Field, CekstvSwitch<Field> > Q (F, n, factory);
		typedef Butterfly<Field, CekstvSwitch<Field> > Blackbox1;

		Diagonal<Field> D (d);
		// typedef Diagonal<Field> Blackbox2;

		Compose<Blackbox1>  DQ (&P, &Q);
		typedef Compose<Blackbox1, Blackbox1> Blackbox3;
		Compose<Blackbox1, Blackbox3> A (P, DQ);
		typedef Compose<Blackbox1, Blackbox3> Blackbox4;
		//Compose<BlasVector<Field> > A (&P, &DQ);

		Submatrix<Blackbox4> Ap (&A, 0, 0, real_r, real_r);

		det (det_Ap, Ap,  Method::Wiedemann());

		report << "Deteriminant of r x r leading principal minor: ";
		F.write (report, det_Ap) << endl;

		if (F.isZero (det_Ap)) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_WARNING)
				<< "WARNING: Determinant of r x r leading principal minor is zero" << endl;
			++failures;
		}

		commentator().stop ("done");
		commentator().progress ();
	}

	commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
		<< "Total failures: " << failures << endl;

	// FIXME: I need a theoretical error bound
	if (failures > iterations / 5) {
		commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
			<< "ERROR: Too many failures. This is likely a bug." << endl;
		ret = false;
	}

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testCekstvSwitch");

	return ret;
}

/* Test 3: Random linearity
 *
 * Construct a random dense matrix and a submatrix thereof. Call testLinearity
 * in test-generic.h to test that the submatrix is a linear operator
 *
 * F - Field over which to perform computations
 * n - Dimension to which to make matrices
 * iterations - Number of iterations to run
 * N - Number of random vectors to which to apply
 *
 * Return true on success and false on failure
 */

template <class Field>
static bool testRandomLinearity (const Field                                 &F,
				 VectorStream<BlasVector<Field> > &v1_stream,
				 VectorStream<BlasVector<Field> > &v2_stream)
{
	commentator().start ("Testing random linearity", "testRandomLinearity", v1_stream.size ());

	typename Field::RandIter r (F);
	typename CekstvSwitch<Field>::Factory factory (r);
	Butterfly<Field, CekstvSwitch<Field> > A (F, v1_stream.dim (), factory);

	bool ret = testLinearity (A, v1_stream, v2_stream);

	v1_stream.reset ();
	v2_stream.reset ();

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testRandomLinearity");

	return ret;
}

/* Test 4: Random transpose
 *
 * Construct a random dense matrix and a submatrix thereof. Call testLinearity
 * in test-generic.h to test that the submatrix is a linear operator
 *
 * F - Field over which to perform computations
 * n - Dimension to which to make matrices
 * iterations - Number of iterations to run
 * N - Number of random vectors to which to apply
 *
 * Return true on success and false on failure
 */

template <class Field>
static bool testRandomTranspose (const Field                                 &F,
				 VectorStream<BlasVector<Field> > &v1_stream,
				 VectorStream<BlasVector<Field> > &v2_stream)
{
	commentator().start ("Testing random transpose", "testRandomTranspose", v1_stream.size ());

	typename Field::RandIter r (F);
	typename CekstvSwitch<Field>::Factory factory (r);
	Butterfly<Field, CekstvSwitch<Field> > A (F, v1_stream.dim (), factory);

	bool ret = testTranspose (F, A, v1_stream, v2_stream);

	v1_stream.reset ();
	v2_stream.reset ();

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testRandomTranspose");

	return ret;
}

int main (int argc, char **argv)
{
	bool pass = true;

	static int32_t n = 100;
	static integer q = 65521U;
	static unsigned int iterations = 1; // was 10
	static unsigned int k = 10;

	static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test matrices to NxN.",      TYPE_INT,     &n },
		{ 'q', "-q Q", "Operate over the \"field\" GF(Q) [1].", TYPE_INTEGER, &q },
		{ 'i', "-i I", "Perform each test for I iterations.",           TYPE_INT,     &iterations },
		{ 'k', "-k K", "K nonzero elements in random vectors.",        TYPE_INT,     &k },
		END_OF_ARGUMENTS
	};

	typedef Givaro::Modular<uint32_t, uint64_t> Field;

	parseArguments (argc, argv, args);
	srand ((unsigned int) time (NULL));

	commentator().setMaxDepth (-1);
	commentator().setMaxDetailLevel (-1);

	commentator().start("Butterfly preconditioner test suite", "butterfly preconditioner");

	Field F (q);

	// SetButterfly
        Field::RandIter Gen(F);
        typedef Givaro::GeneralRingNonZeroRandIter<Field> NZRand;
        NZRand NZGen(Gen);
	RandomSparseStream<Field, Vector<Field>::Sparse, NZRand>
            stream (F, NZGen, (double) k / (double) n, n, iterations);
	if (!testSetButterfly  (F, stream, k)) pass = false;

	// Cekstv
	if (!testCekstvSwitch  (F, iterations, n, k)) pass = false;

	// Blackbox
	Butterfly<Field> P(F, n);
	if (!testBlackboxNoRW(P)) pass = false;

	commentator().stop("butterfly preconditioner test suite");
	return pass ? 0 : -1;
}

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
