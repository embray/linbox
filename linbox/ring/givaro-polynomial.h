/* linbox/ring/givaro-polynomial.h
 * Copyright(C) LinBox
 * Written by
 * Clement Pernet
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

/*! @file ring/givaro-polynomial.h
 * @ingroup ring
 * @brief NO DOC
 */

#ifndef __LINBOX_givaropolynomial_H
#define __LINBOX_givaropolynomial_H

#include <iostream>
#include <givaro/givpoly1.h>
#include <givaro/givpoly1factor.h>
#include <givaro/zring.h>

#include "linbox/integer.h"
//#include "linbox/element/givaro-polynomial.h"


// Namespace in which all LinBox code resides
namespace LinBox
{

	/** Polynomials.
	 * \ingroup ring
	 *
	 * -param Polynomial type, e.g. std::vector<Field::Element>
	 *  @tparam Domain
	 *  @tparam StorageTag
	 */
	template <class Domain, class StorageTag= Givaro::Dense>
	class GivPolynomialRing : public Givaro::Poly1FactorDom< Domain,StorageTag> {
	public:

		//	using Givaro::Poly1FactorDom<Domain,StorageTag>::eval;
		typedef typename Givaro::Poly1FactorDom<Domain,StorageTag> Father_t;
		typedef typename Father_t::Element Element;
		typedef Element Polynomial;

		typedef Father_t Rep;

		// GivPolynomialRing () {}

		GivPolynomialRing (const Domain& D) : Father_t( Domain(D) )
		{}

		GivPolynomialRing (const Domain& D, const Givaro::Indeter& I) :
                Father_t(Domain(D), I)
		{}

		template<class PolyCont>
		PolyCont& factor (PolyCont& factors,
				  std::vector<uint64_t>& exp,
				  const Polynomial& P)
            {

                    // JGD 02.03.2012 : to be refactored
                    // at least without pointers ...
                std::vector<Polynomial> Lf;
                CZfactor(Lf, exp, P); // Cantor-Zassenhaus factorization
                factors.resize((size_t)Lf.size());
                for(size_t i=0;  i<Lf.size(); ++i)
                    factors[(size_t)i] = new Polynomial(Lf[(size_t)i]);

                return factors;
            }


	};


#ifdef __LINBOX_HAVE_NTL
}

#include "linbox/ring/ntl.h"
#include "NTL/ZZXFactoring.h"
namespace LinBox
{
	typedef GivPolynomialRing<Givaro::ZRing<integer>, Givaro::Dense> GivPolIntDense;

	template <>
	template <>
	std::vector<GivPolIntDense::Element* >&
	GivPolIntDense::factor (std::vector<GivPolIntDense::Element* >& factors,
				std::vector<uint64_t>& exp,
				const GivPolIntDense::Element &P)
	{
		NTL::ZZXFac_InitNumPrimes = 1;
		NTL::ZZX f;
		for (size_t i = 0; i < P.size(); ++i){
			NTL::SetCoeff (f, (long)i, NTL::to_ZZ((std::string( P[(size_t)i] )).c_str()) );
		}
		NTL::vec_pair_ZZX_long ntlfactors;
		NTL::ZZ c;
		NTL::factor (c, ntlfactors, f);

		NTL::ZZ t;
		NTL_ZZ NTLIntDom;
		factors.resize((size_t)ntlfactors.length());
		exp.resize((size_t)ntlfactors.length());
		for (int i= 0; i<ntlfactors.length(); ++i) {
			factors[(size_t)i] = new GivPolIntDense::Element( (size_t)( deg(ntlfactors[i].a)+1 ));
			for(int j = 0; j <= deg(ntlfactors[i].a); ++j) {
				NTL::GetCoeff(t,ntlfactors[i].a,j);
				NTLIntDom.convert( factors[(size_t)i]->operator[]((size_t)j), t );
			}
			exp[(size_t)i] =(unsigned long) ntlfactors[i].b;
		}
		return factors;
	}
}

// #include "givaro/zring.h"
 namespace LinBox
 {
// 	typedef GivPolynomialRing<Givaro::ZRing<Integer>, Givaro::Dense> GivPolPIDIntDense;
// 	template <>
// 	template <>
// 	std::vector<GivPolPIDIntDense::Element* >&
// 	GivPolPIDIntDense::factor<std::vector<GivPolPIDIntDense::Element* > > (std::vector<GivPolPIDIntDense::Element* >& factors,
// 			std::vector<unsigned long>& exp,
// 			const GivPolPIDIntDense::Element &P)
// 	{
// 		NTL::ZZXFac_InitNumPrimes = 1;
// 		NTL::ZZX f;
// 		for (size_t i = 0; i < P.size(); ++i){
// 			NTL::SetCoeff (f, (long)i, NTL::to_ZZ((std::string( P[(size_t)i] )).c_str()) );
// 		}
// 		NTL::vec_pair_ZZX_long ntlfactors;
// 		NTL::ZZ c;
// 		NTL::factor (c, ntlfactors, f);

// 		NTL::ZZ t;
// 		NTL_ZZ NTLIntDom;
// 		factors.resize((size_t)ntlfactors.length());
// 		exp.resize((size_t)ntlfactors.length());
// 		for (int i= 0; i<ntlfactors.length(); ++i) {
// 			factors[(size_t)i] = new GivPolPIDIntDense::Element( (size_t)deg(ntlfactors[i].a)+1 );
// 			for(int j = 0; j <= deg(ntlfactors[i].a); ++j) {
// 				NTL::GetCoeff(t,ntlfactors[i].a,j);
// 				NTLIntDom.convert( factors[(size_t)i]->operator[]((size_t)j), t );
// 			}
// 			exp[(size_t)i] = (unsigned long)ntlfactors[i].b;
// 		}
// 		return factors;
// 	}

	typedef GivPolynomialRing< NTL_ZZ , Givaro::Dense> GivPolZZDense;
	template <>
	template <>
	std::vector<GivPolZZDense::Element* >&
	GivPolZZDense::factor<std::vector<GivPolZZDense::Element* > >
	(std::vector<GivPolZZDense::Element* >& factors,
	 std::vector<uint64_t>& exp,
	 const GivPolZZDense::Element &P)
	{
		NTL::ZZXFac_InitNumPrimes = 1;
		NTL::ZZX f;
		for (size_t i = 0; i < P.size(); ++i){
			NTL::SetCoeff (f, (long)i, P[(size_t)i]);
		}
		NTL::vec_pair_ZZX_long ntlfactors;
		NTL::ZZ c;
		NTL::factor (c, ntlfactors, f);

		NTL::ZZ t;
		factors.resize((size_t)ntlfactors.length());
		exp.resize((size_t)ntlfactors.length());
		for (int i= 0; i<ntlfactors.length(); ++i) {
			factors[(size_t)i] = new GivPolZZDense::Element((size_t) deg(ntlfactors[i].a)+1 );
			for(int j = 0; j <= deg(ntlfactors[i].a); ++j) {
				NTL::GetCoeff( factors[(size_t)i]->operator[]((size_t)j),ntlfactors[i].a,j);
			}
			exp[(size_t)i] = (unsigned long)ntlfactors[i].b;
		}
		return factors;
	}

#endif

	typedef GivPolynomialRing<Givaro::Modular<double>, Givaro::Dense> GivPolMdDense;

	template <>
	template <>
	std::vector<GivPolMdDense::Element *>&
	GivPolMdDense::factor (std::vector<GivPolMdDense::Element* > & factors,
			       std::vector<uint64_t>& exp,
			       const GivPolMdDense::Element& P)
	{
		integer charac;
		_domain.characteristic(charac);
		double p = charac;
		typedef Givaro::Modular<double> GivModDouble;
		typedef Givaro::Poly1FactorDom< GivModDouble, Givaro::Dense, GivModDouble::RandIter> PolysDouble;


		PolysDouble PFD(*this, GivModDouble::RandIter(_domain));
		std::vector<PolysDouble::Element> factors2;
		PFD.CZfactor ( factors2, exp, static_cast<PolysDouble::Element>(P),p);

		//std::cerr<<"factorization done"<<std::endl;
		factors.resize((size_t)factors2.size());
		std::vector<GivPolMdDense::Element* >::iterator itf = factors.begin();
		std::vector<PolysDouble::Element >::const_iterator itf2 = factors2.begin();
		for (; itf2 != factors2.end();++itf,++itf2){
			*itf = new GivPolMdDense::Element(*itf2);
			//std::cerr<<"converting factor"<<(*itf)<<std::endl;
			for (size_t i=0; i< (*itf)->size();++i)
				_domain.divin((*itf)->operator[](i),(*itf)->operator[]((*itf)->size()-1));
			_domain.assign((*itf)->operator[]((*itf)->size()-1),1.0);
		}
		return factors;
	}

} // namespace LinBox


#endif // __LINBOX_givaropolynomial_H


// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s

