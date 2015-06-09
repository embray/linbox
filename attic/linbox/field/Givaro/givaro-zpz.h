/* linbox/field/givaro-zpz.h
 * Copyright (C) 2002 Pascal Giorgi
 *
 * Written by Pascal Giorgi <pascal.giorgi@ens-lyon.fr>
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

/* WARNING this wrapper works only with an improved version of Givaro.
 * This version of givaro won't be available for public yet.
 * But it is available on my web page.
 * You can send me a mail to get it or for others details.
 */

#ifndef __LINBOX_field_givaro_zpz_H
#define __LINBOX_field_givaro_zpz_H


#include "linbox/linbox-config.h"
#include "linbox/integer.h"
#include "linbox/field/field-interface.h"
#include "linbox/util/debug.h"
#include "linbox/vector/vector-domain.h"
#include "linbox/util/write-mm.h"
//-------------------------------------
// Files of Givaro library
#include <givaro/modular.h>
#include <givaro/givranditer.h>
#include "linbox/field/field-traits.h"

//--------------------------------------

// Namespace in which all LinBox code resides
namespace LinBox
{

	/*  This wrapper allows to use three sorts of givaro fields :
	 *  Elements represent by a 32 bits integer
	 *  Elements represent by a 16 bits integer
	 *  Elements represent in Zech log representation by a 16 bits integer
	 *
	 *  To use these fields with the wrapper below, just replace the template
	 *  parameter by the appropriate Tag.
	 *  "int16_t"  for 16 bits integer
	 *  "int32_t"  for 32 bits integer
	 *  "Givaro::Log16"  for Zech log representation in 16 bits
	 */
	template<class Field>
	class DotProductDomain;
	template<class Field>
	class FieldAXPY;

	template <class Ring>
	struct ClassifyRing;

	template <class TAG>
	class GivaroZpz;

	template<class Tag>
	struct ClassifyRing<GivaroZpz<Tag> > {
		typedef RingCategories::ModularTag categoryTag;
	};

	/** \brief wrapper of Givaro's Modular.
	  \ingroup field

	 *  Most methods are inherited from Givaro::Modular< int16_t>, Givaro::Modular< int32_t>
	 *  and Givaro::Modular<log16> classes of Givaro.
	 *  These classes allow to construct only finite fields with a prime modulus.
	 */
	template <class TAG>
	class GivaroZpz : public Givaro::Modular<TAG>, public FieldInterface {

	private:

		/*		friend class DotProductDomain<GivaroZpz<TAG> > ;
				friend class FieldAXPY<GivaroZpz<TAG> >; */

	public:

		//typedef integer Integer;

		/** Element type.
		 *  This type is inherited from the Givaro class Givaro::Modular<TAG>
		 */
		typedef typename Givaro::Modular<TAG>::Rep Element;
		// Element zero,one,mOne;
		typedef Givaro::Modular<TAG> Father_t ;
		typedef GivaroZpz<TAG>      Self_t ;
		using  Father_t::one ;
		using  Father_t::zero ;
		using  Father_t::mOne ;

		/** RandIter type
		 *  This type is inherited from the Givaro class Givaro::Modular<TAG>
		 */
		typedef Givaro::GIV_randIter< Givaro::Modular<TAG>, integer > RandIter;

		/** Constructor from an integer
		 *  this constructor uses the Givaro::Modular<TAG> constructor.
		 */
		GivaroZpz (const integer &p) :
		 Givaro::Modular<TAG> (static_cast<typename Givaro::Modular<TAG>::Residu_t> (p))
		{}


		/** Constructor from an integer (takes degree of extension as 2nd parameter, must be 1).
		 *  This constructor uses the Givaro::Modular<TAG> constructor.
		 */
		GivaroZpz (const integer &p, const integer& k) :
		 Givaro::Modular<TAG> (static_cast<typename Givaro::Modular<TAG>::Residu_t> (p))
		{

			if (k!=1)
				throw PreconditionFailed(LB_FILE_LOC,"exponent must be 1");
		}

		/** Copy constructor.
		 * This copy constructor uses the Givaro::Modular<TAG> copy constructor.
		 */
		GivaroZpz (const GivaroZpz<TAG>& F) :
		 Givaro::Modular<TAG> (F)
		{}

		/** Characteristic.
		 * Return integer representing characteristic of the domain.
		 * @return integer representing characteristic of the domain.
		 */
		integer &characteristic (integer &c) const
		{
			return c = integer ( Givaro::Modular<TAG>::size ());
		}
		unsigned long &characteristic (unsigned long &c) const
		{
			return c = (unsigned long) ( Givaro::Modular<TAG>::size ());
		}

		long characteristic() const
		{
			return static_cast<int>( Givaro::Modular<TAG>::size());
		}

		/** Cardinality.
		 * Return integer representing cardinality of the domain.
		 * @return integer representing cardinality of the domain
		 */
		integer &cardinality (integer &c) const
		{
			return c = integer ( Givaro::Modular<TAG>::size ());
		}

		integer cardinality () const
		{
			return integer ( Givaro::Modular<TAG>::size ());
		}

		/** Conversion of field base element to an integer.
		 * This function assumes the output field base element x has already been
		 * constructed, but that it is not already initialized.
		 * @return reference to an integer.
		 * @param x integer to contain output (reference returned).
		 * @param y constant field base element.
		 */
		integer &convert (integer &x, const Element &y) const
		{
			return x = integer (y);
		}

		double &convert (double& x, const Element& y) const
		{
			return x = static_cast<double>(y);
		}

		template<class Type>
		Type &convert (Type& x, const Element& y) const
		{
			return x = static_cast<Type>(y);
		}

		/** Initialization of field base element from an integer.
		 * Behaves like C++ allocator construct.
		 * This function assumes the output field base element x has already been
		 * constructed, but that it is not already initialized.
		 * @return reference to field base element.
		 * @param x field base element to contain output (reference returned).
		 * @param y integer.
		 */
		//!@bug using Father_t::init ?
		Element &init (Element &x , const integer &y = 0) const
		{
			//
			//	AU 28/03/07 no cast to long allows to use Givaro::Modular<integer>
			//
			//Givaro::Modular<TAG>::init (x, (long) (y% integer(this->_p)));
			Givaro::Modular<TAG>::init (x, (y% integer(this->_p)));
			return x;
		}

		Element &init (Element &x , const long &y ) const
		{
			return Givaro::Modular<TAG>::init (x, y ) ;

		}

		Element &init (Element &x , const int &y ) const
		{
			return Givaro::Modular<TAG>::init (x, y ) ;

		}

		Element &init (Element &x , const unsigned&y ) const
		{
			return Givaro::Modular<TAG>::init (x, y ) ;

		}

		Element &init (Element &x , const unsigned long &y ) const
		{
			return Givaro::Modular<TAG>::init (x, y ) ;

		}

		Element &init (Element &x , const double &y ) const
		{
			double z = fmod(y, (double) this->_p);
			if (z < 0) z += (double) this->_p;
			z += 0.5;
			return x = static_cast<Element>(z); //rounds towards 0
		}

		static uint64_t getMaxModulus();

		using Father_t::write;
		std::ostream &write (std::ostream &os, std::string F) const
		{
			os << "GivaroZpz<" << eltype( Element() ) << " > "; // class name
			if (F != "") {
				integer p = cardinality();
				os << F << "( " << p << " )"; // show constuctor args
			}
			return os;
		}



	}; // class GivaroZpz<TAG>


	template <> uint64_t GivaroZpz< int32_t>::getMaxModulus() { return 46339; } // 2^15.5-1
	template <> uint64_t GivaroZpz< int64_t>::getMaxModulus() { return 3037000499ULL; } // 2^15.5-1
	template <> uint64_t GivaroZpz< uint32_t>::getMaxModulus() { return 65535; } // 2^16-1
	template <> uint64_t GivaroZpz< int16_t>::getMaxModulus() { return 255; }   // 2^8-1
	template <> uint64_t GivaroZpz< Givaro::Log16>::getMaxModulus() { return 32767; } // 2^15 - 1

	/** Specialisation of the convert function for the zech log representation
	 *	of givaro-zpz (GivaroZpz< Givaro::Log16>.
	 *  this function translates the internal representation to the real
	 *	value of the element.
	 *	This can have no sense but can be usefull
	 *  NB : the init function for this specialisation does the same thing.
	 *  the function transaltes the values to her internal representation.
	 */
	template <> integer& GivaroZpz< Givaro::Log16>::convert(integer& x, const Element& y) const
	{
		if (y>=this->_p) return x = 0;
		int tmp = _tab_rep2value[y];
		return x = integer (tmp);
	}

	template <> double& GivaroZpz< Givaro::Log16>::convert(double& x, const Element& y) const
	{
		if (y>=this->_p) return x = 0.0;
		int tmp = _tab_rep2value[y];
		return x = (double) tmp;
	}

	template <> GivaroZpz< Givaro::Log16>::Element& GivaroZpz< Givaro::Log16>::init(GivaroZpz< Givaro::Log16>::Element& x, const double& y) const
	{
		double z = fmod(y, (double) this->_p);
		if (z < 0) z += this->_p;
		z += 0.5;
		return x = _tab_value2rep[static_cast<long>(z)]; //rounds towards 0
	}

	template <> GivaroZpz< Givaro::Log16>::Element& GivaroZpz< Givaro::Log16>::init(GivaroZpz< Givaro::Log16>::Element& x, const integer& y) const
	{
		int tmp =(int) (y % (integer)this->_p);
		if (tmp < 0 ) tmp += this->_p;
		return x = _tab_value2rep[tmp];
	}

	/* Specialization of FieldAXPY for GivaroZpz< int32_t> Field */

	template <>
	class FieldAXPY<GivaroZpz< int32_t> > {
	public:

		typedef GivaroZpz< int32_t>::Element Element;
		typedef uint64_t Abnormal;
		typedef GivaroZpz< int32_t> Field;

		FieldAXPY (const Field &F) :
			_field (&F) , Corr(uint64_t(-1) % (uint64_t)F.characteristic() +1)
		{ _y = 0; }
		FieldAXPY (const FieldAXPY &faxpy) :
			_field (faxpy._field), _y (0) , Corr(faxpy.Corr)
		{}

		FieldAXPY<GivaroZpz< int32_t> > &operator = (const FieldAXPY &faxpy)
		{ _field = faxpy._field; _y = faxpy._y; Corr = faxpy.Corr; return *this; }

		inline uint64_t& mulacc (const Element &a, const Element &x)
		{
			uint64_t t = (uint64_t) a * (uint64_t) x;
			_y += t;
			if (_y < t)
				return _y += Corr;
			else
				return _y;
		}

		inline uint64_t& accumulate (const Element &t)
		{
			_y += (uint64_t) t;
			if (_y < (uint64_t)t)
				return _y += Corr;
			else
				return _y;
		}

		inline Element &get (Element &y) {
			_y %= (uint64_t) field().characteristic();
			if ((int64_t) _y < 0) _y += (uint64_t) field().characteristic();
			y = (Element) (uint32_t) _y; //! @bug or feature
			return y;
		}

		inline FieldAXPY &assign (const Element y)
		{
		       	_y = (uint64_t)y;
		       	return *this;
	       	}

		inline void reset()
		{
			_y = 0;
		}

		inline const Field & field() const { return *_field; }

	private:

		const Field *_field;
		uint64_t _y;
		uint64_t Corr;
	};


	/* Specialization of FieldAXPY for GivaroZpz< int32_t> Field */

	template <>
	class FieldAXPY<GivaroZpz< int16_t> > {
	public:

		typedef GivaroZpz< int16_t>::Element Element;
		typedef uint32_t Abnormal;
		typedef GivaroZpz< int16_t> Field;

		FieldAXPY (const Field &F) :
			_field (&F) , Corr(uint32_t(-1) % (uint32_t)F.characteristic() +1)
		{
			_y = 0;
		}
		FieldAXPY (const FieldAXPY &faxpy) :
			_field (faxpy._field), _y (0) , Corr(faxpy.Corr)
		{}

		FieldAXPY<GivaroZpz< int16_t> > &operator = (const FieldAXPY &faxpy)
		{
			_field = faxpy._field;
			_y = faxpy._y;
			Corr = faxpy.Corr;
			return *this;
		}

		inline uint32_t& mulacc (const Element &a, const Element &x)
		{
			uint32_t t = (uint32_t) a * (uint32_t) x;
			_y += t;

			if (_y < t)
				return _y += Corr;
			else
				return _y;
		}

		inline uint32_t& accumulate (const Element &t)
		{
			_y += (uint32_t) t;

			if (_y < (uint32_t)t)
				return _y += Corr;
			else
				return _y;
		}

		inline Element &get (Element &y)
		{
			_y %= (uint32_t) field().characteristic();
			if ((int32_t) _y < 0)
				_y += (uint32_t) field().characteristic();
			y = (Element) (uint16_t) _y; //! @bug or feature ?
			return y;
		}

		inline FieldAXPY &assign (const Element y)
		{
			_y = (uint32_t)y;
			return *this;
		}

		inline void reset()
		{
			_y = 0;
		}

		inline const Field & field() const { return *_field; }
	private:

		const Field * _field;
		uint32_t _y;
		uint32_t Corr;
	};

	// Specialization of DotProductDomain for GivaroZpz< int32_t> field

	template <>
	class DotProductDomain<GivaroZpz< int32_t> > :  private virtual VectorDomainBase<GivaroZpz< int32_t> > {

	public:

		typedef GivaroZpz< int32_t>::Element Element;
		DotProductDomain(){}
		DotProductDomain (const GivaroZpz< int32_t> &F) :
			VectorDomainBase<GivaroZpz< int32_t> > (F) ,
			Corr(uint64_t(-1) % (uint64_t)F.characteristic() +1),
			Max(uint64_t(-1))
		{}

		using VectorDomainBase<GivaroZpz< int32_t> >::field;
		using VectorDomainBase<GivaroZpz< int32_t> >::_field;
	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;

	private:
		uint64_t Corr;
		uint64_t Max;
	};

	// Specialization of DotProductDomain for GivaroZpz< int16_t> field

	template <>
	class DotProductDomain<GivaroZpz< int16_t> > :  private virtual VectorDomainBase<GivaroZpz< int16_t> > {

	public:

		typedef GivaroZpz< int16_t>::Element Element;

		DotProductDomain(){}

		DotProductDomain (const GivaroZpz< int16_t> &F) :
			VectorDomainBase<GivaroZpz< int16_t> > (F) ,
			Corr(uint32_t(-1) % (uint32_t)F.characteristic() +1),
			Max(uint32_t(-1))
		{}

		using VectorDomainBase<GivaroZpz< int16_t> >::field;
		using VectorDomainBase<GivaroZpz< int16_t> >::_field;
	protected:
		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

		template <class Vector1, class Vector2>
		inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;

	private:
		uint32_t Corr;
		uint32_t Max;
	};

} // namespace LinBox

#include "linbox/field/Givaro/givaro-zpz.inl"

#endif // __LINBOX_field_givaro_zpz_H


// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
