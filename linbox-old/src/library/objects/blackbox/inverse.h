/* -*- mode: c; style: linux -*- */

/* linbox/src/library/objects/blackbox/inverse.h
 * Copyright (C) 2001 Bradford Hovinen
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

#ifndef __INVERSE_H
#define __INVERSE_H

#include "LinBox/blackbox_archetype.h"
#include "LinBox/vector_traits.h"
#include "LinBox/diagonal.h"

// Namespace in which all LinBox library code resides
namespace LinBox
{

	/** Blackbox Inverse.  This represents the inverse of a nonsingular
	 * matrix.
	 *
	 * The matrix itself is not stored in memory.  Rather, its apply
	 * methods use a vector of {@link Fields field} elements, which are 
	 * used to "multiply" the matrix to a vector.
	 * 
	 * This class has three template parameters.  The first is the field in 
	 * which the arithmetic is to be done.  The second is the type of 
	 * \Ref{LinBox} vector to which to apply the matrix.  The 
	 * third is chosen be default to be the \Ref{LinBox} vector trait
	 * of the vector.  This class is then specialized for dense and sparse 
	 * vectors.
	 *
	 * @param Field \Ref{LinBox} field
	 * @param Vector \Ref{LinBox} dense or sparse vector of field elements
	 * @param Trait  Marker whether to use dense or sparse LinBox vector 
	 *               implementation.  This is chosen by a default parameter 
	 *               and partial template specialization.  */
	template <class Field, class Vector, class Trait = vector_traits<Vector>::vector_category>
	class Inverse : public Blackbox_archetype<Vector>
	{
		typedef vector<Field::Element> Polynomial;

	    public:

		typedef Blackbox_archetype<Vector> Blackbox;

		/** Constructor from field and dense vector of field elements.
		 * @param __BB   Black box of which to get the inverse
		 */
		Inverse (const Field &F, const Blackbox *BB)
		    : _F (F), _BB (BB->clone ())
		{
			Polynomial _mp1;
			int i;

			minpoly (_mp1, _BB);

			_minpoly.resize (_mp1.size () - 1);

			for (i = 1; i < _mp1.size (); i++) {
				_F.div (_minpoly[i-1], _mp1[i], _mp1[0]);
				_F.negin (_minpoly[i-1]);
			}
		}

		/** Copy constructor, so that we don't have to recompute the
		 * minimal polynomial every time this black box is used inside
		 * another black box
		 */
		Inverse (const Inverse &BB)
		    : _F (BB._F), _BB (BB._BB->clone ()), _minpoly (BB._minpoly)
		{}

		/** Virtual constructor.
		 * Required because constructors cannot be virtual.
		 * Make a copy of the Blackbox_archetype object.
		 * Required by abstract base class.
		 * @return pointer to new blackbox object
		 */
	        Blackbox *clone () const
	        {
			return new Inverse (*this);
		}

		/** Application of BlackBox matrix.
		 * y= A*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  y reference to vector into which to store the result
		 * @param  x constant reference to vector to contain input
		 */
	        Vector& apply (Vector &y, const Vector& x) const
	        {
			Vector tmp;
			int n = _minpoly.size () - 1;
			int i;

			tmp.resize (_BB->coldim ());

			// I would somewhat like to assume here that y is already
			// of the right size. Maybe it does not matter.
			y.resize (coldim ());

			for (i = 0; i < coldim (); i++)
				_F.mul (y[i], x[i], _minpoly[n]);

			for (i = n - 1; i >= 0; i--) {
				_BB->apply (tmp, y);
				// FIXME: How do we do axpy on vectors?
				axpy (y, x, _minpoly[i], tmp);
			}

			return y;
	        }

		/** Application of BlackBox matrix transpose.
		 * y= transpose(A)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		Vector& applyTranspose (Vector &y, const Vector& x) const
		{
			// FIXME: We probably need a different minimal polynomial here...
			Vector tmp;
			int n = _minpoly.size () - 1;
			int i;

			tmp.resize (_BB->coldim ());

			// I would somewhat like to assume here that y is alread
			// of the right size. Maybe it does not matter.
			y.resize (coldim ());

			for (i = 0; i < coldim (); i++)
				_F.mul (y[i], x[i], _minpoly[n]);

			for (i = n - 1; i >= 0; i--) {
				_BB->applyTranspose (tmp, y);
				// FIXME: How do we do axpy on vectors?
				axpy (y, x, _minpoly[i], tmp);
			}

			return y;
		}

		/** Retreive row dimensions of BlackBox matrix.
		 * This may be needed for applying preconditioners.
		 * Required by abstract base class.
		 * @return integer number of rows of black box matrix.
		 */
		size_t rowdim (void) const
		{
			return _BB->rowdim ();
		}
    
		/** Retreive column dimensions of BlackBox matrix.
		 * Required by abstract base class.
		 * @return integer number of columns of black box matrix.
		 */
		size_t coldim(void) const
		{
			return _BB->coldim ();
		}

	    private:

		Blackbox        *_BB;
		const Field     &_F;
		Polynomial       _minpoly;

	}; // template <Field, Vector> class Inverse

} // namespace LinBox

#endif // __INVERSE_H
