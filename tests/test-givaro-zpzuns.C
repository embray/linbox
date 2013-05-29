
/* tests/test-givaro-zpz.C
 * Copyright (C) 2002 Pascal Giorgi
 *
 * Written by Pascal Giorgi  <pascal.giorgi@ens-lyon.fr>
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


/*! @file  tests/test-givaro-zpzuns.C
 * @ingroup tests
 * @brief  no doc
 * @test NO DOC
 */



#include "linbox/linbox-config.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "linbox/field/givaro.h"

#include "test-common.h"
#include "test-field.h"

#ifndef TEST_ARCHETYPES
#define TEST_ARCHETYPES 1
#endif

using namespace LinBox;

int main (int argc, char **argv)
{
        static integer q = 10733;
	static size_t n = 10000;
	static unsigned int iterations = 10;
	static int e;
	static unsigned int trials = 10000;
	static unsigned int categories = 1000;
	static unsigned int hist_level = 10;


        static Argument args[] = {
                { 'q', "-q Q", "Operate over the \"field\" GF(Q) [1].", TYPE_INTEGER, &q },
                { 'e', "-e E", "Use GF(q^e) for the extension field [1].", TYPE_INT,     &e },
		{ 'n', "-n N", "Set dimension of test vectors to NxN.", TYPE_INT,     &n },
		{ 'i', "-i I", "Perform each test for I iterations.", TYPE_INT,     &iterations },
		{ 't', "-t T", "Number of trials for the random iterator test.", TYPE_INT, &trials },
		{ 'c', "-c C", "Number of categories for the random iterator test.", TYPE_INT, &categories },
		{ 'H', "-H H", "History level for random iterator test.", TYPE_INT, &hist_level },
		END_OF_ARGUMENTS
        };

        parseArguments (argc, argv, args);

	//cout << endl << "GivaroZpz< Givaro::Unsigned32> field test suite" << endl;
	//cout.flush ();
	bool pass = true;

	GivaroZpz< Givaro::Unsigned32> F1 (2);
	GivaroZpz< Givaro::Unsigned32> F2 (q);
	GivaroZpz< Givaro::Unsigned32> F3 (3);
	GivaroZpz< Givaro::Unsigned32> F4 (32749);
	GivaroZpz< Givaro::Unsigned32> F5 (65521);

	LinBox::commentator().start("Givaro-zpzuns test suite", "GivZpzu");
	// Make sure some more detailed messages get printed
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (4);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_UNIMPORTANT);

	if (!runFieldTests (F1, "2",     iterations, n, false)) pass = false;
	if (!runFieldTests (F2, "10733", iterations, n, false)) pass = false;
	if (!runFieldTests (F3, "3",     iterations, n, false)) pass = false;
	if (!runFieldTests (F4, "32749", iterations, n, false)) pass = false;
	if (!runFieldTests (F5, "65521", iterations, n, false)) pass = false;

	if (!testRandomIterator (F1,  "GivaroZpz< Givaro::Unsigned32>(2)", trials, categories, hist_level)) pass = false;
	if (!testRandomIterator (F2,  "GivaroZpz< Givaro::Unsigned32>(10733)", trials, categories, hist_level)) pass = false;
	if (!testRandomIterator (F3,  "GivaroZpz< Givaro::Unsigned32>(3)", trials, categories, hist_level)) pass = false;
	if (!testRandomIterator (F4,  "GivaroZpz< Givaro::Unsigned32>(32749)", trials, categories, hist_level)) pass = false;
	if (!testRandomIterator (F5,  "GivaroZpz< Givaro::Unsigned32>(65521)", trials, categories, hist_level)) pass = false;

#if TEST_ARCHETYPES

	GivaroZpz< Givaro::Std16> * K1g = new GivaroZpz< Givaro::Std16> (101);
	FieldArchetype K1(K1g);
	if (!testField<FieldArchetype> (K1, "Testing archetype with envelope of GivaroZpz< Givaro::Std16> field"))
		pass = false;
	delete K1g;
#endif

#if TEST_ARCHETYPES
	GivaroZpz< Givaro::Unsigned32> * K2g = new GivaroZpz< Givaro::Unsigned32> (101);
	FieldArchetype K2(K2g);
	if (!testField<FieldArchetype> (K2, "Testing archetype with envelope of GivaroZpz< Givaro::Unsigned32> field"))
		pass = false;
	delete K2g;
#endif

#if TEST_ARCHETYPES
	GivaroZpz< Givaro::Log16> * K3g = new GivaroZpz< Givaro::Log16>(101);
	FieldArchetype K3(K3g);

	if (!testField<FieldArchetype> (K3, "Testing archetype with envelope of GivaroZpz< Givaro::Log16> field"))
		pass = false;
	delete K3g;
#endif

#if TEST_ARCHETYPES
	GivaroGfq * K4g = new GivaroGfq(101,1);
	FieldArchetype K4(K4g);

	if (!testField<FieldArchetype> (K4, "Testing archetype with envelope of GivaroGfq prime field"))
		pass = false;
	delete K4g;
#endif


	LinBox::commentator().stop(MSG_STATUS (pass), "GivaroZpzuns test suite");
	return pass ? 0 : -1;
}

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
