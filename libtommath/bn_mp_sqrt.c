#include <tommath.h>

#ifdef BN_MP_SQRT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://math.libtomcrypt.org
 */

#ifndef NO_FLOATING_POINT
#include <math.h>
#endif

/* this function is less generic than mp_n_root, simpler and faster */
int mp_sqrt(mp_int *arg, mp_int *ret) 
{
  int res;
  mp_int t1,t2;
  int i, j, k;
#ifndef NO_FLOATING_POINT
  double d;
  mp_digit dig;
#endif

  /* must be positive */
  if (arg->sign == MP_NEG) {
    return MP_VAL;
  }

  /* easy out */
  if (mp_iszero(arg) == MP_YES) {
    mp_zero(ret);
    return MP_OKAY;
  }
  
  i = (arg->used / 2) - 1;
  j = 2 * i;
  if ((res = mp_init_size(&t1, i+2)) != MP_OKAY) {
      return res;
  }
  
  if ((res = mp_init(&t2)) != MP_OKAY) {
    goto E2;
  }

  for (k = 0; k < i; ++k) {
      t1.dp[k] = (mp_digit) 0;
  }
      
#ifndef NO_FLOATING_POINT

  /* Estimate the square root using the hardware floating point unit. */

  d = 0.0;
  for (k = arg->used-1; k >= j; --k) {
      d = ldexp(d, DIGIT_BIT) + (double) (arg->dp[k]);
  }
  d = sqrt(d);
  dig = (mp_digit) ldexp(d, -DIGIT_BIT);
  if (dig) {
      t1.used = i+2;
      t1.dp[i+1] = dig;
      d -= ldexp((double) dig, DIGIT_BIT);
  } else {
      t1.used = i+1;
  }
  t1.dp[i] = (mp_digit) d;

#else

  /* Estimate the square root as having 1 in the most significant place. */

  t1.used = i + 2;
  t1.dp[i+1] = (mp_digit) 1;
  t1.dp[i] = (mp_digit) 0;

#endif

  /* t1 > 0  */ 
  if ((res = mp_div(arg,&t1,&t2,NULL)) != MP_OKAY) {
    goto E1;
  }
  if ((res = mp_add(&t1,&t2,&t1)) != MP_OKAY) {
    goto E1;
  }
  if ((res = mp_div_2(&t1,&t1)) != MP_OKAY) {
    goto E1;
  }
  /* And now t1 > sqrt(arg) */
  do { 
    if ((res = mp_div(arg,&t1,&t2,NULL)) != MP_OKAY) {
      goto E1;
    }
    if ((res = mp_add(&t1,&t2,&t1)) != MP_OKAY) {
      goto E1;
    }
    if ((res = mp_div_2(&t1,&t1)) != MP_OKAY) {
      goto E1;
    }
    /* t1 >= sqrt(arg) >= t2 at this point */
  } while (mp_cmp_mag(&t1,&t2) == MP_GT);

  mp_exch(&t1,ret);

E1: mp_clear(&t2);
E2: mp_clear(&t1);
  return res;
}

#endif

/* $Source: /root/tcl/repos-to-convert/tcl/libtommath/bn_mp_sqrt.c,v $ */
/* $Revision: 1.2 $ */
/* $Date: 2005/12/27 17:39:02 $ */
