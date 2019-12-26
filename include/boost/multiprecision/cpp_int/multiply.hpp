///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//
// Comparison operators for cpp_int_backend:
//
#ifndef BOOST_MP_CPP_INT_MUL_HPP
#define BOOST_MP_CPP_INT_MUL_HPP

namespace boost { namespace multiprecision { namespace backends {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#endif

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && !is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a,
    const limb_type&                                                            val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   if (!val)
   {
      result = static_cast<limb_type>(0);
      return;
   }
   if ((void*)&a != (void*)&result)
      result.resize(a.size(), a.size());
   double_limb_type                                                                                  carry = 0;
   typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_pointer       p     = result.limbs();
   typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_pointer       pe    = result.limbs() + result.size();
   typename cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>::const_limb_pointer pa    = a.limbs();
   while (p != pe)
   {
      carry += static_cast<double_limb_type>(*pa) * static_cast<double_limb_type>(val);
#ifdef __MSVC_RUNTIME_CHECKS
      *p = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
      *p = static_cast<limb_type>(carry);
#endif
      carry >>= cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
      ++p, ++pa;
   }
   if (carry)
   {
      unsigned i = result.size();
      result.resize(i + 1, i + 1);
      if (result.size() > i)
         result.limbs()[i] = static_cast<limb_type>(carry);
   }
   result.sign(a.sign());
   if (!cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::variable)
      result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2, unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3, cpp_int_check_type Checked3, class Allocator3>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<   (is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value  == false)
                                                     && (is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value  == false)
                                                     && (is_trivial_cpp_int<cpp_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3> >::value  == false)>::type
eval_multiply_a_by_b(
          cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a,
    const cpp_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3>& b) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   // Very simple long multiplication, only usable for small numbers of limb_type's
   // but that's the typical use case for this type anyway:
   //
   // Special cases first:
   //
   unsigned                                                                                          as = a.size();
   unsigned                                                                                          bs = b.size();
   typename cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>::const_limb_pointer pa = a.limbs();
   typename cpp_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3>::const_limb_pointer pb = b.limbs();

   if (as == 1)
   {
      if (bs == 1)
      {
         result = static_cast<double_limb_type>(*pa) * static_cast<double_limb_type>(*pb);
      }
      else
      {
         limb_type l = *pa;
         eval_multiply(result, b, l);
      }
      return;
   }
   if (bs == 1)
   {
      limb_type l = *pb;
      eval_multiply(result, a, l);
      return;
   }

   if ((void*)&result == (void*)&a)
   {
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t(a);
      eval_multiply(result, t, b);
      return;
   }
   if ((void*)&result == (void*)&b)
   {
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t(b);
      eval_multiply(result, a, t);
      return;
   }

   result.resize(as + bs, as + bs);
   typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_pointer pr = result.limbs();
#ifdef BOOST_NO_CXX14_CONSTEXPR
   static const double_limb_type limb_max        = ~static_cast<limb_type>(0u);
   static const double_limb_type double_limb_max = ~static_cast<double_limb_type>(0u);
#else
   constexpr const double_limb_type limb_max = ~static_cast<limb_type>(0u);
   constexpr const double_limb_type double_limb_max = ~static_cast<double_limb_type>(0u);
#endif
   BOOST_STATIC_ASSERT((double_limb_max - (2U * limb_max)) >= (limb_max * limb_max));

   double_limb_type carry = 0;
#ifndef BOOST_MP_NO_CONSTEXPR_DETECTION
   if (BOOST_MP_IS_CONST_EVALUATED(as))
   {
      for (unsigned i = 0; i < result.size(); ++i)
         pr[i] = 0;
   }
   else
#endif
   {
      std::memset(pr, 0, result.size() * sizeof(limb_type));
   }

   // It is here where we have reduced the problem of multiplication
   // to the level of raw pointers and lengths for the inputs and result.
   // It is hoped that this tighe loop area is the same for all
   // forms of multiplciation n*m -> x used in cpp_int.

   // The case of n*n -> n multiplication has not yet been optimized,
   // but it would be easy to do so.

   for (unsigned i = 0; i < as; ++i)
   {
      if(pa[i] != 0)
      {
         carry = 0;

         unsigned j = 0;

         for (; j < bs && ((i + j) < result.size()); ++j)
         {
            BOOST_ASSERT(i + j < result.size());

#if (!defined(__GLIBCXX__) && !defined(__GLIBCPP__)) || !BOOST_WORKAROUND(BOOST_GCC_VERSION, <= 50100)
            BOOST_ASSERT(    !std::numeric_limits<double_limb_type>::is_specialized
                         || ((std::numeric_limits<double_limb_type>::max)() - carry > static_cast<double_limb_type>(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::max_limb_value) * static_cast<double_limb_type>(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::max_limb_value)));
#endif
            carry += static_cast<double_limb_type>(pa[i] * static_cast<double_limb_type>(pb[j]));

            BOOST_ASSERT(    !std::numeric_limits<double_limb_type>::is_specialized
                         || ((std::numeric_limits<double_limb_type>::max)() - carry >= pr[i + j]));

            carry += pr[i + j];

#ifdef __MSVC_RUNTIME_CHECKS
            pr[i + j] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
            pr[i + j] = static_cast<limb_type>(carry);
#endif
            carry >>= cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;

            BOOST_ASSERT(carry <= (cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::max_limb_value));
         }

         if(i + j < result.size())
         {
#ifdef __MSVC_RUNTIME_CHECKS
            pr[(i + j)] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
            pr[(i + j)] = static_cast<limb_type>(carry);
#endif
         }
      }
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2, unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3, cpp_int_check_type Checked3, class Allocator3>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<   (is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value  == false)
                                                     && (is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value  == false)
                                                     && (is_trivial_cpp_int<cpp_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3> >::value  == false)
                                                     && (                   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::variable == true)>::type
eval_multiply(
          cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a,
    const cpp_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3>& b) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   eval_multiply_a_by_b(result, a, b);

   using result_cpp_int_backend_type = cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>;

   using result_const_reverse_iterator =
      std::reverse_iterator<typename result_cpp_int_backend_type::const_limb_pointer>;

   typename result_cpp_int_backend_type::const_limb_pointer pr = result.limbs();

   const bool trim_highest_limb = (*result_const_reverse_iterator(pr + result.size()) == 0);

   if(trim_highest_limb)
   {
      result.resize(a.size() + b.size() - 1, a.size() + b.size() - 1);
   }

   result.sign(b.sign() != a.sign());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2, unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3, cpp_int_check_type Checked3, class Allocator3>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<   (is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value  == false)
                                                     && (is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value  == false)
                                                     && (is_trivial_cpp_int<cpp_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3> >::value  == false)
                                                     && (                   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::variable == false)>::type
eval_multiply(
          cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a,
    const cpp_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3>& b) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   eval_multiply_a_by_b(result, a, b);

   result.sign(b.sign() != a.sign());

   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && !is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   eval_multiply(result, result, a);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const limb_type& val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   eval_multiply(result, result, val);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && !is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a,
    const double_limb_type&                                                     val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   if (val <= (std::numeric_limits<limb_type>::max)())
   {
      eval_multiply(result, a, static_cast<limb_type>(val));
   }
   else
   {
#if BOOST_ENDIAN_LITTLE_BYTE && !defined(BOOST_MP_TEST_NO_LE)
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t(val);
#else
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t;
      t = val;
#endif
      eval_multiply(result, a, t);
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const double_limb_type& val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   eval_multiply(result, result, val);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && !is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a,
    const signed_limb_type&                                                     val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   if (val > 0)
      eval_multiply(result, a, static_cast<limb_type>(val));
   else
   {
      eval_multiply(result, a, static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(val)));
      result.negate();
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const signed_limb_type& val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   eval_multiply(result, result, val);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && !is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>& a,
    const signed_double_limb_type&                                              val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   if (val > 0)
   {
      if (val <= (std::numeric_limits<limb_type>::max)())
      {
         eval_multiply(result, a, static_cast<limb_type>(val));
         return;
      }
   }
   else if (val >= -static_cast<signed_double_limb_type>((std::numeric_limits<limb_type>::max)()))
   {
      eval_multiply(result, a, static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(val)));
      result.negate();
      return;
   }
#if BOOST_ENDIAN_LITTLE_BYTE && !defined(BOOST_MP_TEST_NO_LE)
   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t(val);
#else
   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t;
   t = val;
#endif
   eval_multiply(result, a, t);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const signed_double_limb_type& val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   eval_multiply(result, result, val);
}

//
// Now over again for trivial cpp_int's:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && (is_signed_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value || is_signed_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value)>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& o) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   *result.limbs() = detail::checked_multiply(*result.limbs(), *o.limbs(), typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
   result.sign(result.sign() != o.sign());
   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && is_unsigned_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& o) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   *result.limbs() = detail::checked_multiply(*result.limbs(), *o.limbs(), typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && (is_signed_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value || is_signed_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value)>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   *result.limbs() = detail::checked_multiply(*a.limbs(), *b.limbs(), typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
   result.sign(a.sign() != b.sign());
   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && is_unsigned_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   *result.limbs() = detail::checked_multiply(*a.limbs(), *b.limbs(), typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
   result.normalize();
}

//
// Special routines for multiplying two integers to obtain a multiprecision result:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
    signed_double_limb_type a, signed_double_limb_type b)
{
#ifdef BOOST_NO_CXX14_CONSTEXPR
   static const signed_double_limb_type mask      = ~static_cast<limb_type>(0);
   static const unsigned                limb_bits = sizeof(limb_type) * CHAR_BIT;
#else
   constexpr const signed_double_limb_type mask = ~static_cast<limb_type>(0);
   constexpr const unsigned limb_bits = sizeof(limb_type) * CHAR_BIT;
#endif
   bool                                 s         = false;
   if (a < 0)
   {
      a = -a;
      s = true;
   }
   if (b < 0)
   {
      b = -b;
      s = !s;
   }
   double_limb_type w = a & mask;
   double_limb_type x = a >> limb_bits;
   double_limb_type y = b & mask;
   double_limb_type z = b >> limb_bits;

   result.resize(4, 4);
   limb_type* pr = result.limbs();

   double_limb_type carry = w * y;
#ifdef __MSVC_RUNTIME_CHECKS
   pr[0] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += w * z + x * y;
   pr[1] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += x * z;
   pr[2] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#else
   pr[0] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += w * z + x * y;
   pr[1] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += x * z;
   pr[2] = static_cast<limb_type>(carry);
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#endif
   result.sign(s);
   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
    double_limb_type a, double_limb_type b)
{
#ifdef BOOST_NO_CXX14_CONSTEXPR
   static const signed_double_limb_type mask      = ~static_cast<limb_type>(0);
   static const unsigned                limb_bits = sizeof(limb_type) * CHAR_BIT;
#else
   constexpr const signed_double_limb_type mask = ~static_cast<limb_type>(0);
   constexpr const unsigned limb_bits = sizeof(limb_type) * CHAR_BIT;
#endif

   double_limb_type w = a & mask;
   double_limb_type x = a >> limb_bits;
   double_limb_type y = b & mask;
   double_limb_type z = b >> limb_bits;

   result.resize(4, 4);
   limb_type* pr = result.limbs();

   double_limb_type carry = w * y;
#ifdef __MSVC_RUNTIME_CHECKS
   pr[0] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += w * z;
   pr[1] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   pr[2] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry = x * y + pr[1];
   pr[1] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += pr[2] + x * z;
   pr[2] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#else
   pr[0] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += w * z;
   pr[1] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   pr[2] = static_cast<limb_type>(carry);
   carry = x * y + pr[1];
   pr[1] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += pr[2] + x * z;
   pr[2] = static_cast<limb_type>(carry);
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#endif
   result.sign(false);
   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1,
          unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2, class Allocator2>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value && is_trivial_cpp_int<cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> const& a,
    cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> const& b)
{
   typedef typename boost::multiprecision::detail::canonical<typename cpp_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2>::local_limb_type, cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::type canonical_type;
   eval_multiply(result, static_cast<canonical_type>(*a.limbs()), static_cast<canonical_type>(*b.limbs()));
   result.sign(a.sign() != b.sign());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class SI>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_signed<SI>::value && (sizeof(SI) <= sizeof(signed_double_limb_type) / 2)>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
    SI a, SI b)
{
   result = static_cast<signed_double_limb_type>(a) * static_cast<signed_double_limb_type>(b);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class UI>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_unsigned<UI>::value && (sizeof(UI) <= sizeof(signed_double_limb_type) / 2)>::type
eval_multiply(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
    UI a, UI b)
{
   result = static_cast<double_limb_type>(a) * static_cast<double_limb_type>(b);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

}}} // namespace boost::multiprecision::backends

#endif
