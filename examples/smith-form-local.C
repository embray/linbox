#include <linbox/linbox-config.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <linbox/util/timer.h>

#include <linbox/ring/modular.h>
#include <linbox/matrix/sparse-matrix.h>
#include <linbox/algorithms/smith-form-sparseelim-local.h>

#include <linbox/util/timer.h>

#include <linbox/ring/local2_32.h>
#include <linbox/ring/pir-modular-int32.h>
#include <linbox/ring/givaro-poly.h>
#include <linbox/ring/givaro-poly-local.h>
#include <linbox/ring/givaro-poly-quotient.h>
#include <linbox/randiter/givaro-poly.h>
#include <linbox/algorithms/smith-form-local.h>
#include <linbox/algorithms/smith-form-iliopoulos2.h>
#include <linbox/algorithms/smith-form-adaptive.h>

using namespace LinBox;
using namespace std;

typedef Givaro::Modular<double> Field;
typedef typename Field::Element Element;
typedef Givaro::Poly1Dom<Field,Givaro::Dense> PolyDom;
typedef GivaroPoly<Field> PolyRing;
typedef GivaroPolyQuotient<PolyDom> QuotRing;
typedef GivaroPolyRandIter<PolyRing> PolyRandIter;
typedef typename PolyRing::Element PolyElement;
typedef MatrixDomain<PolyRing> MatDom;
typedef typename MatDom::OwnMatrix Mat;
typedef MatrixDomain<QuotRing> QuotMatDom;
typedef typename QuotMatDom::OwnMatrix QMat;

typedef GivaroPolyLocal<PolyDom> LocalRing;
typedef MatrixDomain<LocalRing> LocalMatDom;
typedef LocalMatDom::OwnMatrix LMat;

typedef IliopoulosDomain<PolyRing> IliopoulosDom;
typedef SmithFormLocal<QuotRing> LocalDom;
typedef DenseVector<PolyRing> FactorVector;

int VERBOSE = 0;

void local(Mat &M, PolyElement &f, QuotRing &R) {
	size_t n = M.coldim();
	QMat A(R, n, n);
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			PolyElement tmp;
			M.getEntry(tmp, i, j);
			A.setEntry(i, j, tmp);
		}
	}
	
	LocalDom sfl;
	list<PolyElement> l;
	
	Timer timer;
	timer.start();
	sfl(l, A, R);
	timer.stop();
	cout << "Local Time: " << timer << endl;
	
	if (VERBOSE) {
		list<PolyElement>::const_iterator iterator;
		for (iterator = l.begin(); iterator != l.end(); ++iterator) {
			R.write(cout, *iterator) << endl;
		}
		cout << endl;
	}
}

void local2(Mat &M, LocalRing &R) {
	size_t n = M.coldim();
	LMat A(R, n, n);
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			PolyElement tmp;
			M.getEntry(tmp, i, j);
			
			LocalRing::Element ltmp;
			R.init(ltmp, tmp);
			
			A.setEntry(i, j, ltmp);
		}
	}
	
	SmithFormLocal<LocalRing> sfl;
	list<LocalRing::Element> l;
	
	Timer timer;
	timer.start();
	sfl(l, A, R);
	timer.stop();
	cout << "Local2 Time: " << timer << endl;
	
	if (VERBOSE) {
		list<LocalRing::Element>::const_iterator iterator;
		for (iterator = l.begin(); iterator != l.end(); ++iterator) {
			R.write(cout, *iterator) << endl;
		}
		cout << endl;
	}
}

void ilio(Mat &M, PolyElement &f, MatDom &MD, PolyRing &R) {
	IliopoulosDom sfi(R);
	
	size_t n = M.coldim();
	Mat A(MD.field(), n, n);
	MD.copy(A, M);
	
	FactorVector l;
	l.resize(n);
	
	Timer timer;
	timer.start();
	sfi.smithForm(l, A, f);
	timer.stop();
	cout << "Iliopolous Time: " << timer << endl;
	
	if (VERBOSE) {
		for (size_t i = 0; i < l.size(); i++) {
			R.write(cout, l[i]) << endl;
		}
		cout << endl;
	}
}

void generateM(
	Mat &M,
	MatDom &MD,
	PolyRing &R,
	PolyElement &f,
	PolyElement &g,
	size_t n,
	size_t e
) {
	integer max;
	R.convert(max, f);
	max *= 10;
	
	Mat L(R, n, n);
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < i; j++) {
			PolyElement e;
			R.init(e, rand() % max);
			R.modin(e, f);
			L.setEntry(i, j, e);
		}
	}
	for (size_t i = 0; i < n; i++) {
		L.setEntry(i, i, R.one);
	}
	
	Mat T(R, n, n);
	for (size_t i = 0; i < n; i++) {
		for (size_t j = i+1; j < n; j++) {
			PolyElement e;
			R.init(e, rand() % max);
			R.modin(e, f);
			T.setEntry(i, j, e);
		}
	}
	for (size_t i = 0; i < n; i++) {
		T.setEntry(i, i, R.one);
	}
	
	Mat D(R, n, n);
	for (size_t i = 0; i < n; i++) {
		size_t ei = rand() % e;
		PolyElement di;
		R.assign(di, R.one);
		
		for (size_t j = 0; j < ei; j++) {
			R.mulin(di, g);
		}
		
		D.setEntry(i, i, di);
		if (VERBOSE > 1) {
			R.write(cout << i << ", " << i << ": ", di) << endl;
		}
	}
	
	MD.mul(M, L, D);
	MD.mulin(M, T);
	
	if (VERBOSE > 1) {
		cout << endl;
		for (size_t i = 0; i < n; i++) {
			for (size_t j = 0; j < n; j++) {
				PolyElement e;
				M.getEntry(e, i, j);
				R.write(cout << i << ", " << j << ": ", e) << endl;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	size_t p = 3;
	size_t n = 50;
	size_t e = 6;
	int gn = 13;

	static Argument args[] = {
		{ 'p', "-p P", "Set the field GF(p)", TYPE_INT, &p},
		{ 'n', "-n N", "Set the matrix dimensions", TYPE_INT, &n},
		{ 'e', "-e E", "Set the max exponent", TYPE_INT, &e},
		{ 'g', "-g G", "Set irreducible such that f(p) = G", TYPE_INT, &gn},
		{ 'v', "-v V", "Set verbosity", TYPE_INT, &VERBOSE},
		END_OF_ARGUMENTS
	};
	
	parseArguments(argc, argv, args);
	
	Field F(p);
	PolyRing R(F, "x");
	PolyRandIter PRI(R, 0, 10);
	MatDom MD(R);
	
	PolyElement g, f;
	R.init(g, gn);
	R.write(cout << "irred: ", g) << endl;
	
	R.pow(f, g, e);
	R.write(cout << "quotient: ", f) << endl;
	
	QuotRing QR(R, f);
	LocalRing LR(R, g, e);
	
	Mat M(R, n, n);
	generateM(M, MD, R, f, g, n, e);
	
	cout << endl;
	
	ilio(M, f, MD, R);
	
	//local(M, f, QR);
	
	local2(M, LR);
}
