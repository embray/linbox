/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/field/modular.h
 * Copyright (C) 1999-2001 William J Turner,
 *               2001 Bradford Hovinen
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 * 2002-04-10 Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * LargeModular is now replace by a class Modular parameterized on the element
 * type. So, the old LargeModular is equivalent to Modular<integer>. All other
 * interface details are exactly the same.
 *
 * Renamed from large-modular.h to modular.h
 * ------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __FIELD_MODULAR_H
#define __FIELD_MODULAR_H

#include <iostream>
#include <climits>

#include "linbox/integer.h"
#include "linbox/field/vector-domain.h"
#include "linbox/field/field-interface.h"
#include "linbox/util/field-axpy.h"
#include "linbox/vector/vector-traits.h"

// Namespace in which all LinBox code resides
namespace LinBox 
{ 

	/** Field of elements modulo some modulus
	 *
	 * This parameterized field can be used to construct any prime
	 * field. Typical use would be Modular<integer> for integers modulo a
	 * large prime, Modular<long, long long> for integers modulo a wordsize
	 * prime, and Modular<long> for integers modulo a half-wordsize prime.
	 *
	 * @param element Element type, e.g. long or integer
	 * @param Intermediate Type to use for intermediate computations. This
	 *                     should be a data type that can support integers
	 *                     twice the length of the maximal modulus used
	 *
	 * The primality of the modulus will not be checked, so it is the
	 * programmer's responsibility to supply a prime modulus.  This class
	 * implements a field of unparamterized integers modulo a prime integer.
	 * Field has (non-static) member to contain modulus of field.
	 */
	template <class _Element>
	class ModularBase
	{
	    public:

		/** Element type
		 */
		typedef _Element Element;

		/** Random iterator generator type.
		 * It must meet the common object interface of random element generators
		 * as given in the the archetype RandIterArchetype.
		 */
	        class RandIter;

		/** @name Object Management
		 */
		//@{
 
		/** Default constructor.
		 */
		ModularBase (void) {}

		/** Constructor from an element type.
		 * Sets the modulus of the field throug the static member of the 
		 * element type.
		 * @param value constant reference to integer prime modulus
		 */
		ModularBase (unsigned long value) : _modulus (value) {}

		/** Constructor from an integer.
		 * Sets the modulus of the field throug the static member of the 
		 * element type.
		 * @param value constant reference to integer prime modulus
		 */
		ModularBase (const integer &value) : _modulus (value) {}

		/** Copy constructor.
		 * Constructs Modular object by copying the field.
		 * This is required to allow field objects to be passed by value
		 * into functions.
		 * @param  F Modular object.
		 */
		ModularBase (const ModularBase<Element> &F) : _modulus (F._modulus) {}
 
		/** Assignment operator.
		 * Required by abstract base class.
		 * @return reference to Modular object for self
		 * @param F constant reference to Modular object
		 */
		ModularBase &operator= (const ModularBase<Element> &F)
			{ return *this; }

		/** Conversion of field base element to a template class T.
		 * This function assumes the output field base element x has already been
		 * constructed, but that it is not already initialized.
		 * @return reference to template class T.
		 * @param x template class T to contain output (reference returned).
		 * @param y constant field base element.
		 */
		integer &convert (integer &x, const Element &y) const
			{ return x = y; }
 
		/** Assignment of one field base element to another.
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return reference to x
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 */
		Element &assign (Element &x, const Element &y) const { return x = y; }

		/** Cardinality.
		 * Return integer representing cardinality of the domain.
		 * Returns a non-negative integer for all domains with finite
		 * cardinality, and returns -1 to signify a domain of infinite
		 * cardinality.
		 * @return integer representing cardinality of the domain
		 */
		integer &cardinality (integer &c) const
			{ return c = _modulus; }
 
		/** Characteristic.
		 * Return integer representing characteristic of the domain.
		 * Returns a positive integer to all domains with finite characteristic,
		 * and returns 0 to signify a domain of infinite characteristic.
		 * @return integer representing characteristic of the domain.
		 */
		integer &characteristic (integer &c) const
			{ return c = _modulus; }

		//@} Object Management

		/** @name Arithmetic Operations
		 * x <- y op z; x <- op y
		 * These operations require all elements, including x, to be initialized
		 * before the operation is called.  Uninitialized field base elements will
		 * give undefined results.
		 */
		//@{

		/** Equality of two elements.
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return boolean true if equal, false if not.
		 * @param  x field base element
		 * @param  y field base element
		 */
		bool areEqual (const Element &x, const Element &y) const
			{ return x == y; }

		/** Zero equality.
		 * Test if field base element is equal to zero.
		 * This function assumes the field base element has already been
		 * constructed and initialized.
		 * @return boolean true if equals zero, false if not.
		 * @param  x field base element.
		 */
		bool isZero (const Element &x) const
			{ return x == 0; }
 
		/** One equality.
		 * Test if field base element is equal to one.
		 * This function assumes the field base element has already been
		 * constructed and initialized.
		 * @return boolean true if equals one, false if not.
		 * @param  x field base element.
		 */
		bool isOne (const Element &x) const
			{ return x == 1; }

		//@} Arithmetic Operations

		/** @name Input/Output Operations */
		//@{

		/** Print field.
		 * @return output stream to which field is written.
		 * @param  os  output stream to which field is written.
		 */
		ostream &write (ostream &os) const 
			{ return os << "integers mod " << _modulus; }

		/** Read field.
		 * @return input stream from which field is read.
		 * @param  is  input stream from which field is read.
		 */
		istream &read (istream &is) { return is >> _modulus; }

		/** Print field base element.
		 * This function assumes the field base element has already been
		 * constructed and initialized.
		 * @return output stream to which field base element is written.
		 * @param  os  output stream to which field base element is written.
		 * @param  x   field base element.
		 */
		ostream &write (ostream &os, const Element &x) const
			{ return os << x; }
 
		/** Read field base element.
		 * This function assumes the field base element has already been
		 * constructed and initialized.
		 * @return input stream from which field base element is read.
		 * @param  is  input stream from which field base element is read.
		 * @param  x   field base element.
		 */
		istream &read (istream &is, Element &x) const
		{
			is >> x;

			x %= _modulus;
			if (x < 0) x += _modulus;

			return is; 
		}

		//@}

	    protected:

		/// Private (non-static) element for modulus
		Element _modulus;

	}; // class ModularBase

	/** Field of elements modulo some modulus
	 *
	 * This parameterized field can be used to construct any prime
	 * field. Typical use would be Modular<integer> for integers modulo a
	 * large prime, Modular<long, long long> for integers modulo a wordsize
	 * prime, and Modular<long> for integers modulo a half-wordsize prime.
	 *
	 * @param element Element type, e.g. long or integer
	 * @param Intermediate Type to use for intermediate computations. This
	 *                     should be a data type that can support integers
	 *                     twice the length of the maximal modulus used
	 *
	 * The primality of the modulus will not be checked, so it is the
	 * programmer's responsibility to supply a prime modulus.  This class
	 * implements a field of unparamterized integers modulo a prime integer.
	 * Field has (non-static) member to contain modulus of field.
	 */
	template <class _Element>
	class Modular : public ModularBase<_Element>, public FieldInterface
	{
	    public:
		typedef _Element Element;
		typedef typename ModularBase<_Element>::RandIter RandIter;

		/** @name Object Management
		 */
		//@{
 
		/** Default constructor.
		 */
		Modular () {}

		/** Constructor from an element type
		 * Sets the modulus of the field throug the static member of the 
		 * element type.
		 * @param value constant reference to integer prime modulus
		 */
		Modular (unsigned long value) : ModularBase<_Element> (value) {}

		/** Constructor from an integer
		 * Sets the modulus of the field throug the static member of the 
		 * element type.
		 * @param value constant reference to integer prime modulus
		 */
		Modular (const integer &value) : ModularBase<_Element> (value) {}

		/** Initialization of field base element from an integer.
		 * Behaves like C++ allocator construct.
		 * This function assumes the output field base element x has already been
		 * constructed, but that it is not already initialized.
		 * This is not a specialization of the template function because
		 * such a specialization is not allowed inside the class declaration.
		 * @return reference to field base element.
		 * @param x field base element to contain output (reference returned).
		 * @param y integer.
		 */
		Element &init (Element &x, const integer &y = 0) const
		{ 
			x = y % _modulus;
			if (x < 0) x += _modulus;
			return x;
		}

		//@}  
		/** @name Arithmetic Operations
		 * x <- y op z; x <- op y
		 * These operations require all elements, including x, to be initialized
		 * before the operation is called.  Uninitialized field base elements will
		 * give undefined results.
		 */
		//@{

		/** Addition.
		 * x = y + z
		 * This function assumes all the field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 * @param  z field base element.
		 */
		Element &add (Element &x, const Element &y, const Element &z) const
		{
			x = y + z;
			if (x >= _modulus) x -= _modulus;
			return x;
		}
 
		/** Subtraction.
		 * x = y - z
		 * This function assumes all the field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 * @param  z field base element.
		 */
		Element &sub (Element &x, const Element &y, const Element &z) const
		{ 
			x = y - z;
			if (x < 0) x += _modulus;
			return x;
		}
 
		/** Multiplication.
		 * x = y * z
		 * This function assumes all the field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 * @param  z field base element.
		 */
		Element &mul (Element &x, const Element &y, const Element &z) const
			{ return x = (y * z) % _modulus; }
 
		/** Division.
		 * x = y / z
		 * This function assumes all the field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 * @param  z field base element.
		 */
		Element &div (Element &x, const Element &y, const Element &z) const
		{ 
			Element temp;
			inv (temp, z);
			return mul (x, y, temp);
		}
 
		/** Additive Inverse (Negation).
		 * x = - y
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 */
		Element &neg (Element &x, const Element &y) const
			{ if (y == 0) return x = y; else return x = _modulus - y; }
 
		/** Multiplicative Inverse.
		 * x = 1 / y
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 */
		Element &inv (Element &x, const Element &y) const
		{
			// The extended Euclidean algoritm
			Element x_int, y_int, q, tx, ty, temp;
			x_int = _modulus; 
			y_int = y;
			tx = 0; 
			ty = 1;

			while (y_int != 0) {
				// always: gcd (modulus,residue) = gcd (x_int,y_int)
				//         sx*modulus + tx*residue = x_int
				//         sy*modulus + ty*residue = y_int
				q = x_int / y_int; // integer quotient
				temp = y_int;  y_int  = x_int  - q*y_int;  x_int  = temp;
				temp = ty; ty = tx - q*ty; tx = temp;
			}

			// now x_int = gcd (modulus,residue)
			x = tx;
			if (x < 0) x += _modulus;

			return x;
		}

		/** Natural AXPY.
		 * r  = a * x + y
		 * This function assumes all field elements have already been 
		 * constructed and initialized.
		 * @return reference to r.
		 * @param  r field element (reference returned).
		 * @param  a field element.
		 * @param  x field element.
		 * @param  y field element.
		 */
		Element &axpy (Element &r, 
			      const Element &a, 
			      const Element &x, 
			      const Element &y) const
		{ 
			r = (a * x + y) % _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

		//@} Arithmetic Operations
 
		/** @name Inplace Arithmetic Operations
		 * x <- x op y; x <- op x
		 */
		//@{

		/** Inplace Addition.
		 * x += y
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 */
		Element &addin (Element &x, const Element &y) const
		{ 
			x += y;
			if (x >= _modulus) x -= _modulus;
			return x;
		}
 
		/** Inplace Subtraction.
		 * x -= y
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 */
		Element &subin (Element &x, 
				const Element &y) const
		{
			x -= y;
			if (x < 0) x += _modulus;
			return x;
		}
 
		/** Inplace Multiplication.
		 * x *= y
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 */
		Element &mulin (Element &x, 
				const Element &y) const
		{
			x *= y;
			x %= _modulus;
			return x;
		}
 
		/** Inplace Division.
		 * x /= y
		 * This function assumes both field base elements have already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 * @param  y field base element.
		 */
		Element &divin (Element &x, 
				const Element &y) const
		{
			Element temp;
			inv (temp, y);
			return mulin (x, temp);
		}
 
		/** Inplace Additive Inverse (Inplace Negation).
		 * x = - x
		 * This function assumes the field base element has already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 */
		Element &negin (Element &x) const
		{
			x = _modulus - x;
			return x;
		}
 
		/** Inplace Multiplicative Inverse.
		 * x = 1 / x
		 * This function assumes the field base elementhas already been
		 * constructed and initialized.
		 * @return reference to x.
		 * @param  x field base element (reference returned).
		 */
		Element &invin (Element &x) const
			{ return inv (x, x); }

		/** Inplace AXPY.
		 * r  += a * x
		 * This function assumes all field elements have already been 
		 * constructed and initialized.
		 * Purely virtual
		 * @return reference to r.
		 * @param  r field element (reference returned).
		 * @param  a field element.
		 * @param  x field element.
		 */
		Element &axpyin (Element &r, const Element &a, const Element &x) const
		{ 
			r = (r + a * x) % _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

		//@} Inplace Arithmetic Operations

	    private:

		friend class FieldAXPY<Modular<_Element> >;

	}; // class Modular

	/* Specialization of class Modular for short element type */

	class Modular<short> : public ModularBase<short>
	{
	    public:

		typedef short Element;

		Modular () : k (0) {}
		Modular (unsigned long value)
			: ModularBase<short> (value), k (((unsigned long long) -1LL) / ((_modulus - 1) * (_modulus - 1))) {}
		Modular (const integer &value)
			: ModularBase<short> ((long) value), k (((unsigned long long) -1LL) / ((_modulus - 1) * (_modulus - 1))) {}

		Element &init (Element &x, const integer &y = 0) const
		{
			x = (unsigned long) y % (unsigned long) _modulus;
			if (x < 0) x += _modulus;
			return x;
		}

		Element &add (Element &x, const Element &y, const Element &z) const
		{
			x = y + z;
			if ((unsigned short) x >= (unsigned short) _modulus) x -= _modulus;
			return x;
		}
 
		Element &sub (Element &x, const Element &y, const Element &z) const
		{ 
			x = y - z;
			if (x < 0) x += _modulus;
			return x;
		}
 
		Element &mul (Element &x, const Element &y, const Element &z) const
			{ return x = ((long) y * (long) z) % (long) _modulus; }
 
		Element &div (Element &x, const Element &y, const Element &z) const
		{ 
			Element temp;
			inv (temp, z);
			return mul (x, y, temp);
		}
 
		Element &neg (Element &x, const Element &y) const
			{ if (y == 0) return x = y; else return x = _modulus - y; }
 
		Element &inv (Element &x, const Element &y) const
		{
			// The extended Euclidean algoritm
			unsigned long x_int, y_int, q, tx, ty, temp;
			x_int = _modulus; 
			y_int = y;
			tx = 0; 
			ty = 1;

			while (y_int != 0) {
				// always: gcd (modulus,residue) = gcd (x_int,y_int)
				//         sx*modulus + tx*residue = x_int
				//         sy*modulus + ty*residue = y_int
				q = x_int / y_int; // integer quotient
				temp = y_int;  y_int  = x_int  - q * y_int;
				x_int  = temp;
				temp = ty; ty = tx - q * ty;
				tx = temp;
			}

			// now x_int = gcd (modulus,residue)
			x = tx;
			if (x < 0) x += _modulus;

			return x;
		}

		Element &axpy (Element &r, 
			       const Element &a, 
			       const Element &x, 
			       const Element &y) const
		{
			r = ((unsigned long) a * (unsigned long) x + (unsigned long) y) % (unsigned long) _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

		Element &addin (Element &x, const Element &y) const
		{ 
			x += y;
			if (x >= _modulus) x -= _modulus;
			return x;
		}
 
		Element &subin (Element &x, 
				const Element &y) const
		{
			x -= y;
			if (x < 0) x += _modulus;
			return x;
		}
 
		Element &mulin (Element &x, 
				const Element &y) const
		{
			x = ((unsigned long) x * (unsigned long) y) % (unsigned long) _modulus;
			return x;
		}
 
		Element &divin (Element &x, 
				const Element &y) const
		{
			Element temp;
			inv (temp, y);
			return mulin (x, temp);
		}
 
		Element &negin (Element &x) const
		{
			x = _modulus - x;
			return x;
		}
 
		Element &invin (Element &x) const
			{ return inv (x, x); }

		Element &axpyin (Element &r, const Element &a, const Element &x) const
		{ 
			r = ((unsigned long) r + (unsigned long) a * (unsigned long) x) % (unsigned long) _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

	    private:

		friend class FieldAXPY<Modular<short> >;

		// Number of times one can perform an axpy into a long long
		// before modding out is mandatory.
		unsigned int k;

	}; // class Modular<short>

	/* Specialization of class Modular for int element type */

	class Modular<int> : public ModularBase<int>
	{
	    public:

		typedef int Element;

		Modular () {}
		Modular (unsigned long value)  : ModularBase<int> (value) {}
		Modular (const integer &value) : ModularBase<int> (value) {}

		Element &init (Element &x, const integer &y = 0) const
		{
			x = y % _modulus;
			if (x < 0) x += _modulus;
			return x;
		}

		Element &add (Element &x, const Element &y, const Element &z) const
		{
			x = y + z;
			if ((unsigned int) x >= (unsigned int) _modulus) x -= _modulus;
			return x;
		}
 
		Element &sub (Element &x, const Element &y, const Element &z) const
		{
			x = y - z;
			if (x < 0) x += _modulus;
			return x;
		}
 
		Element &mul (Element &x, const Element &y, const Element &z) const
			{ return x = ((unsigned long long) y * (unsigned long long) z) % (unsigned long long) _modulus; }
 
		Element &div (Element &x, const Element &y, const Element &z) const
		{ 
			Element temp;
			inv (temp, z);
			return mul (x, y, temp);
		}
 
		Element &neg (Element &x, const Element &y) const
			{ if (y == 0) return x = y; else return x = _modulus - y; }
 
		Element &inv (Element &x, const Element &y) const
		{
			// The extended Euclidean algoritm
			unsigned long long x_int, y_int, q, tx, ty, temp;
			x_int = _modulus; 
			y_int = y;
			tx = 0; 
			ty = 1;

			while (y_int != 0) {
				// always: gcd (modulus,residue) = gcd (x_int,y_int)
				//         sx*modulus + tx*residue = x_int
				//         sy*modulus + ty*residue = y_int
				q = x_int / y_int; // integer quotient
				temp = y_int;  y_int  = x_int  - q * y_int;
				x_int  = temp;
				temp = ty; ty = tx - q * ty;
				tx = temp;
			}

			// now x_int = gcd (modulus,residue)
			x = tx;
			if (x < 0) x += _modulus;

			return x;
		}

		Element &axpy (Element &r, 
			       const Element &a, 
			       const Element &x, 
			       const Element &y) const
		{
			r = ((unsigned long long) a * (unsigned long long) x + (unsigned long long) y) % (unsigned long long) _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

		Element &addin (Element &x, const Element &y) const
		{ 
			x += y;
			if ((unsigned int) x >= (unsigned int) _modulus) x -= _modulus;
			return x;
		}
 
		Element &subin (Element &x, 
				const Element &y) const
		{
			x -= y;
			if (x < 0) x += _modulus;
			return x;
		}
 
		Element &mulin (Element &x, 
				const Element &y) const
		{
			x = ((unsigned long long) x * (unsigned long long) y) % (unsigned long long) _modulus;
			return x;
		}
 
		Element &divin (Element &x, 
				const Element &y) const
		{
			Element temp;
			inv (temp, y);
			return mulin (x, temp);
		}
 
		Element &negin (Element &x) const
		{
			x = _modulus - x;
			return x;
		}
 
		Element &invin (Element &x) const
			{ return inv (x, x); }

		Element &axpyin (Element &r, const Element &a, const Element &x) const
		{ 
			r = ((unsigned long long) r + (unsigned long long) a * (unsigned long long) x) % (unsigned long long) _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

	    private:

		friend class FieldAXPY<Modular<int> >;
		friend class VectorDomain<Modular<int> >;

	}; // class Modular<int>

	/* Specialization of class Modular for long element type */

	class Modular<long> : public ModularBase<long>
	{
	    public:

		typedef long Element;

		Modular () {}
		Modular (unsigned long value)  : ModularBase<long> (value) {}
		Modular (const integer &value) : ModularBase<long> (value) {}

		Element &init (Element &x, const integer &y = 0) const
		{
			x = y % _modulus;
			if (x < 0) x += _modulus;
			return x;
		}

		Element &add (Element &x, const Element &y, const Element &z) const
		{
			x = y + z;
			if ((unsigned long) x >= (unsigned long) _modulus) x -= _modulus;
			return x;
		}
 
		Element &sub (Element &x, const Element &y, const Element &z) const
		{
			x = y - z;
			if (x < 0) x += _modulus;
			return x;
		}
 
		Element &mul (Element &x, const Element &y, const Element &z) const
			{ return x = ((unsigned long long) y * (unsigned long long) z) % (unsigned long long) _modulus; }
 
		Element &div (Element &x, const Element &y, const Element &z) const
		{ 
			Element temp;
			inv (temp, z);
			return mul (x, y, temp);
		}
 
		Element &neg (Element &x, const Element &y) const
			{ if (y == 0) return x = y; else return x = _modulus - y; }
 
		Element &inv (Element &x, const Element &y) const
		{
			// The extended Euclidean algoritm
			unsigned long long x_int, y_int, q, tx, ty, temp;
			x_int = _modulus; 
			y_int = y;
			tx = 0; 
			ty = 1;

			while (y_int != 0) {
				// always: gcd (modulus,residue) = gcd (x_int,y_int)
				//         sx*modulus + tx*residue = x_int
				//         sy*modulus + ty*residue = y_int
				q = x_int / y_int; // integer quotient
				temp = y_int;  y_int  = x_int  - q * y_int;
				x_int  = temp;
				temp = ty; ty = tx - q * ty;
				tx = temp;
			}

			// now x_int = gcd (modulus,residue)
			x = tx;
			if (x < 0) x += _modulus;

			return x;
		}

		Element &axpy (Element &r, 
			       const Element &a, 
			       const Element &x, 
			       const Element &y) const
		{
			r = ((unsigned long long) a * (unsigned long long) x + (unsigned long long) y) % (unsigned long long) _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

		Element &addin (Element &x, const Element &y) const
		{ 
			x += y;
			if ((unsigned long) x >= (unsigned long) _modulus) x -= _modulus;
			return x;
		}
 
		Element &subin (Element &x, 
				const Element &y) const
		{
			x -= y;
			if (x < 0) x += _modulus;
			return x;
		}
 
		Element &mulin (Element &x, 
				const Element &y) const
		{
			x = ((unsigned long long) x * (unsigned long long) y) % (unsigned long long) _modulus;
			return x;
		}
 
		Element &divin (Element &x, 
				const Element &y) const
		{
			Element temp;
			inv (temp, y);
			return mulin (x, temp);
		}
 
		Element &negin (Element &x) const
		{
			x = _modulus - x;
			return x;
		}
 
		Element &invin (Element &x) const
			{ return inv (x, x); }

		Element &axpyin (Element &r, const Element &a, const Element &x) const
		{ 
			r = ((unsigned long long) r + (unsigned long long) a * (unsigned long long) x) % (unsigned long long) _modulus;
			if (r < 0) r += _modulus;
			return r;
		}

	    private:

		friend class FieldAXPY<Modular<long> >;
		friend class VectorDomain<Modular<long> >;

	}; // class Modular<long>

	/* Specialization of FieldAXPY for parameterized modular field */

	template <class _Element>
	class FieldAXPY<Modular<_Element> >
	{
	    public:

		typedef _Element Element;
		typedef Modular<_Element> Field;

		FieldAXPY (const Field &F) : _F (F) { _y = 0; }
		FieldAXPY (const FieldAXPY<Modular<Element> > &faxpy) : _F (faxpy._F), _y (faxpy._y) {}

		FieldAXPY<Modular <Element> > &operator = (const FieldAXPY &faxpy) 
			{ _F = faxpy._F; _y = faxpy._y; return *this; }

		inline void accumulate (const Element &a, const Element &x)
			{ _y += a * x; }

		inline Element &get (Element &y) { _y %= _F._modulus; y = _y; return y; }

		inline FieldAXPY &assign (const Element y)
			{ _y = y; return *this; }

	    private:

		Field _F;
		Element _y;
	};

	/* Specialization of FieldAXPY for short modular field */

	template <>
	class FieldAXPY<Modular<short> >
	{
	    public:

		typedef short Element;
		typedef Modular<short> Field;

		FieldAXPY (const Field &F) : _F (F), i (F.k) { _y = 0; }
		FieldAXPY (const FieldAXPY &faxpy) : _F (faxpy._F), _y (0), i (faxpy._F.k) {}

		FieldAXPY<Modular<short> > &operator = (const FieldAXPY &faxpy) 
			{ _F = faxpy._F; _y = faxpy._y; return *this; }

		inline void accumulate (const Element &a, const Element &x)
		{
			long long t = (long long) a * (long long) x;

			if (!i--) {
				_y = (unsigned long) _y % (unsigned long) _F._modulus + t;
				i = _F.k;
			} else
				_y += t;
		}

		inline Element &get (Element &y) {
			(unsigned long) _y %= (unsigned long) _F._modulus;
			if (_y < 0) _y += _F._modulus;
			y = (short) _y;
			i = _F.k;
			return y;
		}

		inline FieldAXPY &assign (const Element y)
			{ _y = y; i = _F.k; return *this; }

	    private:

		Field _F;
		unsigned long long _y;
		int i;
	};

	/* Specialization of FieldAXPY for short modular field */

	template <>
	class FieldAXPY<Modular<int> >
	{
	    public:

		typedef int Element;
		typedef Modular<int> Field;

		FieldAXPY (const Field &F) : _F (F) { _y = 0; }
		FieldAXPY (const FieldAXPY &faxpy) : _F (faxpy._F), _y (0) {}

		FieldAXPY<Modular<int> > &operator = (const FieldAXPY &faxpy) 
			{ _F = faxpy._F; _y = faxpy._y; return *this; }

		inline void accumulate (const Element &a, const Element &x)
		{
			long long t = (long long) a * (long long) x;

			if (_y >= (unsigned long long) -t)
				_y = _y % (unsigned long long) _F._modulus + t;
			else
				_y += t;
		}

		inline Element &get (Element &y) {
			_y %= (unsigned long long) _F._modulus;
			if (_y < 0) _y += _F._modulus;
			y = (int) _y;
			return y;
		}

		inline FieldAXPY &assign (const Element y)
			{ _y = y; return *this; }

	    private:

		Field _F;
		unsigned long long _y;
	};

	/* Specialization of FieldAXPY for short modular field */

	template <>
	class FieldAXPY<Modular<long> >
	{
	    public:

		typedef long Element;
		typedef Modular<long> Field;

		FieldAXPY (const Field &F) : _F (F) { _y = 0; }
		FieldAXPY (const FieldAXPY &faxpy) : _F (faxpy._F), _y (0) {}

		FieldAXPY<Modular<long> > &operator = (const FieldAXPY &faxpy) 
			{ _F = faxpy._F; _y = faxpy._y; return *this; }

		inline void accumulate (const Element &a, const Element &x)
		{
			long long t = (long long) a * (long long) x;

			if (_y >= (unsigned long long) -t)
				_y = _y % (unsigned long long) _F._modulus + t;
			else
				_y += t;
		}

		inline Element &get (Element &y) {
			_y %= (unsigned long long) _F._modulus;
			if (_y < 0) _y += _F._modulus;
			y = (long) _y;
			return y;
		}

		inline FieldAXPY &assign (const Element y)
			{ _y = y; return *this; }

	    private:

		Field _F;
		unsigned long long _y;
	};

} // namespace LinBox

#include "linbox/randiter/modular.h"

#endif // __FIELD_MODULAR_H
