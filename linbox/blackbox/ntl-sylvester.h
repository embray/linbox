/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    ntl-sylvester.h
 *    Copyright (C) 2003 Austin Lobo, B. David Saunders
 *
 *    Template for sylvester matrix specification for ntl Arithmetic,
 *    for polynomials in one variable.
 *    Linbox version 2003
 *  ========LICENCE========
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
 *-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifndef __LINBOX_ntl_sylvester_H
#define __LINBOX_ntl_sylvester_H

#include <iostream>
#include <fstream>
#include <vector>
#include <NTL/ZZ_pX.h>
#include <NTL/ZZ_p.h>

#include "linbox/blackbox/blackbox-interface.h"
#include "linbox/vector/vector-traits.h"

namespace LinBox
{
	/*! This is a representation of the Sylvester matrix of two polynomials.
	 * \ingroup blackbox
	 */
	template <class _Field>
	class Sylvester : public  BlackboxInterface {
	public:
		typedef _Field Field;
		typedef typename Field::Element element;      // Currently restricted to ZZ_p
		~Sylvester();                                 // Destructor
		Sylvester( const Field &F );                                  // Default Constructor
		Sylvester( const Field F,
			   const std::vector<element> &vpx,
			   const std::vector<element> &vpy ); // Constructor given 2 polys and Field
		Sylvester(
			   const BlasVector<Field> &vpx,
			   const BlasVector<Field> &vpy );


		void   print( std::ostream& os = std::cout ) const; // Print to stream or stdout
		void   print( char *outFileName ) const;            // Print to file or stdout
		void   printcp( char *outFileName) const;           // Print to file in sparse format

		inline size_t rowdim() const { return rowDim; }
		inline size_t coldim() const { return colDim; }
		inline size_t sysdim() const { return sysDim; }
		const Field& field() const { return K; }

		template<typename _Tp1>
		struct rebind {
			typedef Sylvester<_Tp1> other;
		};

		template <class OutVector, class InVector>
		OutVector& apply( OutVector &v_out, const InVector& v_in ) const;

		template <class OutVector, class InVector>
		OutVector& applyTranspose( OutVector &v_out, const InVector& v_in ) const;

		//Sylvester(char *dataFileName ); // read from a file -- not implemented yet

		std::ostream& write(std::ostream & os)
		{
			os << "NOT YET" ;
			return os ;
		}
	protected:

		size_t        rowDim,
			      colDim,
			      sysDim;

		NTL::ZZ_pX    pxdata,                // "Upper" Polynomial
		qxdata;                // "Lower" Polynomial in Sylvester matrix

		std::vector<NTL::ZZ_p>    pdata,     // Input vector of polynomial coeff
		qdata;     // Input vector of coeffs for second poly

		size_t pxdeg() const { return pdata.size() - 1; }
		size_t qxdeg() const { return qdata.size() - 1; }


		const Field       &   K;

	};// End, Sylvester
}

#include "linbox/blackbox/ntl-sylvester.inl"

#endif //__LINBOX_ntl_sylvester_H


// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:

