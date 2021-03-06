#include "LegendrePolynomial.h"

template<>
InputParameters validParams<LegendrePolynomial>()
{
  InputParameters params = validParams<Function>();
  /* This parameter is used for normalizing the Legendre polynomials, which
     are only orthogonal on [-1, 1]. However, the Cartesian domain over which
     we'd like to use FETs may exist over a different range. So, just scale the
     polynomial appropriately. */
  params.addRequiredParam<std::vector<Real>>("l_geom_norm",
    "Lengths needed for Legendre polynomial normalization (min, max)");
  params.addParam<bool>("dbg", false, "Print debug output");
  return params;
}

LegendrePolynomial::LegendrePolynomial(const InputParameters & parameters) :
    Function(parameters),
    _geom_norm(parameters.get<std::vector<Real>>("l_geom_norm")),
    _dbg(parameters.get<bool>("dbg"))
{
  // actual domain over which we want orthogonal polynomials
  _dz = _geom_norm[1] - _geom_norm[0];
}

LegendrePolynomial::~LegendrePolynomial()
{
}

Real
LegendrePolynomial::value(Real t, const Point & p)
{
  /* Because we need to pass in the order of the Legendre polynomial, the
     parameter list for this method does not suffice. */
  mooseWarning("value() in LegendrePolynomial should not be used");
  return 0.0;
}

Real
LegendrePolynomial::getPolynomialValue(Real t, Real p, int n)
{
  /* The Legendre polynomials are computed with a recursion relation that uses
     the P_{L-1} and P_{L-2} Legendre polynomials. */

  Real z;          // Normalized position
  Real plm2 = 0.0; // L-2 Legendre polynomial value
  Real plm1 = 0.0; // L-1 Legendre polynomial value
  Real plm = 0.0;  // L   Legendre polynomial value

  z = 2.0 * (p - _geom_norm[0]) / _dz - 1.0;

  /* The recursion relation can only be used for order 2 and above. */
  if (n == 0)
    plm = 1.0;
  else if (n == 1)
    plm = z;
  else
  {
    plm2 = 1.0;
    plm1 = z;
    for (int ii = 2; ii <= n; ++ii)
    {
      plm = (z * (2.0 * ii - 1.0) * plm1) / ii - ((ii - 1.0) * plm2) / ii;
      plm2 = plm1;
      plm1 = plm;
    }
  }

  if (_dbg) _console << "Legendre value  = "
    << plm * sqrt((2.0 * n + 1.0) / 2.0) << std::endl;

  return (plm * sqrt((2.0 * n + 1.0) / 2.0));
}
