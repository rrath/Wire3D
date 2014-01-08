// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Real NoisePerlin<Real>::Noise(Real x, Real y)
{
	Int X = static_cast<Int>(Math<Real>::Floor(x)) & 255;
	Int	Y = static_cast<Int>(Math<Real>::Floor(y)) & 255;
	x -= Math<Real>::Floor(x);
	y -= Math<Real>::Floor(y);
	Real u = Fade(x);
	Real v = Fade(y);
	Int A = s_P[X] + Y;
	Int	AA = s_P[A];
	Int AB = s_P[A+1];
	Int B = s_P[X+1] + Y;
	Int BA = s_P[B];
	Int BB = s_P[B+1];

	return Lerp(v, Lerp(u, Grad(s_P[AA], x, y, 0),
		                   Grad(s_P[BA], x-1, y, 0)),
		           Lerp(u, Grad(s_P[AB], x, y-1, 0),
		                   Grad(s_P[BB], x-1, y-1, 0)));
}

//----------------------------------------------------------------------------
template <class Real>
Real NoisePerlin<Real>::Noise(Real x, Real y, Real z) 
{
	Int X = static_cast<Int>(Math<Real>::Floor(x)) & 255; /* FIND CUBE THAT */
	Int	Y = static_cast<Int>(Math<Real>::Floor(y)) & 255; /* CONTAINS POINT.*/
	Int	Z = static_cast<Int>(Math<Real>::Floor(z)) & 255;
	x -= Math<Real>::Floor(x);                       /* FIND RELATIVE X,Y,Z */
	y -= Math<Real>::Floor(y);                       /* OF POINT IN CUBE.   */
	z -= Math<Real>::Floor(z);
	Real u = Fade(x);                                /* COMPUTE FADE CURVES */
	Real v = Fade(y);                                /* FOR EACH OF X,Y,Z.  */
	Real w = Fade(z);
	Int A = s_P[X] + Y; 
	Int	AA = s_P[A] + Z;
	Int AB = s_P[A+1] + Z;					         /* HASH COORDINATES OF */
	Int B = s_P[X+1] + Y; 
	Int BA = s_P[B] + Z;
	Int BB = s_P[B+1] + Z;					         /* THE 8 CUBE CORNERS, */

	return Lerp(w, Lerp(v, Lerp(u, Grad(s_P[AA], x, y, z),       /* AND ADD */
                                   Grad(s_P[BA], x-1, y, z)),    /* BLENDED */
                           Lerp(u, Grad(s_P[AB], x, y-1, z),     /* RESULTS */
                                   Grad(s_P[BB], x-1, y-1, z))), /* FROM  8 */
                   Lerp(v, Lerp(u, Grad(s_P[AA+1], x, y, z-1 ),  /* CORNERS */
                                   Grad(s_P[BA+1], x-1, y, z-1)),/* OF CUBE */
                           Lerp(u, Grad(s_P[AB+1], x, y-1, z-1),
                                   Grad(s_P[BB+1], x-1, y-1, z-1))));
}

//----------------------------------------------------------------------------
template <class Real>
Real NoisePerlin<Real>::Fade(Real t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

//----------------------------------------------------------------------------
template <class Real>
Real NoisePerlin<Real>::Lerp(Real t, Real a, Real b)
{
	return a + t * (b - a);
}

//----------------------------------------------------------------------------
template <class Real>
Real NoisePerlin<Real>::Grad(Int hash, Real x, Real y, Real z) 
{
	Int h = hash & 15;                    /* CONVERT LO 4 BITS OF HASH CODE */
	Real u = h < 8 ? x : y;               /* INTO 12 GRADIENT DIRECTIONS.   */
	Real v = h < 4 ? y : h==12||h==14 ? x : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}
