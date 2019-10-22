/**
* @file
* @brief QP native, platform-independent priority sets of 32 or 64 elements.
* @ingroup qf
* @cond
******************************************************************************
* Last updated for version 6.3.8
* Last updated on  2019-01-10
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* https://www.state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* @endcond
*/
#ifndef qpset_h
#define qpset_h

#if (!defined QF_MAX_ACTIVE) || (QF_MAX_ACTIVE < 1) || (1024 < QF_MAX_ACTIVE)
    #error "QF_MAX_ACTIVE not defined or out of range. Valid range is 1..1024"
#elif (QF_MAX_ACTIVE <= 8)
    typedef uint8_t QPSetBits;
#elif (QF_MAX_ACTIVE <= 16)
    typedef uint16_t QPSetBits;
#elif (QF_MAX_ACTIVE > 16)
    /*! bitmask for the internal representation of QPSet elements */
    typedef uint32_t QPSetBits;
#endif

#if (QF_MAX_ACTIVE <= 32)

/****************************************************************************/
/*! Priority Set of up to 32 elements */
/**
* The priority set represents the set of active objects that are ready to
* run and need to be considered by the scheduling algorithm. The set is
* capable of storing up to 32 priority levels.
*/
typedef struct {
    QPSetBits volatile bits;  /*!< bitmask with a bit for each element */
} QPSet;

/*! Makes the priority set @p me_ empty */
#define QPSet_setEmpty(me_) ((me_)->bits = (QPSetBits)0)

/*! Evaluates to TRUE if the priority set @p me_ is empty */
#define QPSet_isEmpty(me_) ((me_)->bits == (QPSetBits)0)

/*! Evaluates to TRUE if the priority set @p me_ is not empty */
#define QPSet_notEmpty(me_) ((me_)->bits != (QPSetBits)0)

/*! Evaluates to TRUE if the priority set @p me_ has element @p n_ */
#define QPSet_hasElement(me_, n_) \
    (((me_)->bits & ((QPSetBits)1 << ((n_)-(uint_fast8_t)1))) != (QPSetBits)0)

/*! Insert element @p n_ into the set @p me_, n_= 1..32 */
#define QPSet_insert(me_, n_) \
    ((me_)->bits |= (QPSetBits)((QPSetBits)1 << ((n_) - (uint_fast8_t)1)))

/*! Remove element n_ from the set @p me_, n_= 1..32 */
#define QPSet_remove(me_, n_) \
    ((me_)->bits &= (QPSetBits)(~((QPSetBits)1 << ((n_) - (uint_fast8_t)1))))

/*! Find the maximum element in the set, and assign it to n_ */
/** @note if the set @p me_ is empty, @p n_ is set to zero.
*/
#define QPSet_findMax(me_, n_) \
    ((n_) = QF_LOG2((me_)->bits))

#elif  (QF_MAX_ACTIVE <= 64)

/****************************************************************************/
/*! Priority Set of up to 64 elements */
/**
* The priority set represents the set of active objects that are ready to
* run and need to be considered by the scheduling algorithm. The set is
* capable of storing up to 64 priority levels.
*/
typedef struct {
    uint32_t volatile bits[2];
} QPSet;

/*! Makes the priority set @p me_ empty */
#define QPSet_setEmpty(me_)  do { \
    (me_)->bits[0] = (uint32_t)0; \
    (me_)->bits[1] = (uint32_t)0; \
} while (0)

/*! Evaluates to TRUE if the priority set @p me_ is empty */
/* the following logic avoids UB in volatile access for MISRA compliantce */
#define QPSet_isEmpty(me_) \
    (((me_)->bits[0] == (uint32_t)0) \
     ? ((me_)->bits[1] == (uint32_t)0) \
     : 0)

/*! Evaluates to TRUE if the priority set @p me_ is not empty */
/* the following logic avoids UB in volatile access for MISRA compliantce */
#define QPSet_notEmpty(me_) \
    (((me_)->bits[0] != (uint32_t)0) \
     ? 1 \
     : ((me_)->bits[1] != (uint32_t)0))

/*! Evaluates to TRUE if the priority set @p me_ has element @p n_. */
#define QPSet_hasElement(me_, n_) \
    (((n_) <= (uint_fast8_t)32) \
     ? (((me_)->bits[0] & ((uint32_t)1 << ((n_) - (uint_fast8_t)1 ))) \
       != (uint32_t)0) \
     : (((me_)->bits[1] & ((uint32_t)1 << ((n_) - (uint_fast8_t)33))) \
       != (uint32_t)0))

/*! insert element @p n_ into the set @p me_, n_ = 1..64 */
#define QPSet_insert(me_, n_) do { \
    if ((n_) <= (uint_fast8_t)32) { \
        ((me_)->bits[0] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)1 ))); \
    } \
    else { \
        ((me_)->bits[1] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)33))); \
    } \
} while (0)

/*! Remove element n_ from the set @p me_, n_= 1..64 */
#define QPSet_remove(me_, n_) do { \
    if ((n_) <= (uint_fast8_t)32) { \
        ((me_)->bits[0] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)1 )))); \
    } \
    else { \
        ((me_)->bits[1] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)33)))); \
    } \
} while (0)

/*! Find the maximum element in the set, and assign it to @p n_ */
/** @note if the set @p me_ is empty, @p n_ is set to zero.
*/
#define QPSet_findMax(me_, n_) \
    ((n_) = ((me_)->bits[1] != (uint32_t)0) \
        ? (QF_LOG2((me_)->bits[1]) + (uint_fast8_t)32) \
        : (QF_LOG2((me_)->bits[0])))

#elif  (QF_MAX_ACTIVE <= 256)
/****************************************************************************/
/*! Priority Set of up to 256 elements */
/**
* The priority set represents the set of active objects that are ready to
* run and need to be considered by the scheduling algorithm. The set is
* capable of storing up to 64 priority levels.
*/
typedef struct {
	uint32_t volatile bits[8];
} QPSet;

/*! Makes the priority set @p me_ empty */
#define QPSet_setEmpty(me_)  do { \
    (me_)->bits[0] = (uint32_t)0; \
    (me_)->bits[1] = (uint32_t)0; \
    (me_)->bits[2] = (uint32_t)0; \
    (me_)->bits[3] = (uint32_t)0; \
    (me_)->bits[4] = (uint32_t)0; \
    (me_)->bits[5] = (uint32_t)0; \
    (me_)->bits[6] = (uint32_t)0; \
    (me_)->bits[7] = (uint32_t)0; \
} while (0)

/*! Evaluates to TRUE if the priority set @p me_ is empty */
/* the following logic avoids UB in volatile access for MISRA compliantce */
#define QPSet_isEmpty(me_) \
    (((me_)->bits[0] == (uint32_t)0) \
     ? ((me_)->bits[1] == (uint32_t)0) \
     ? ((me_)->bits[2] == (uint32_t)0) \
     ? ((me_)->bits[3] == (uint32_t)0) \
     ? ((me_)->bits[4] == (uint32_t)0) \
     ? ((me_)->bits[5] == (uint32_t)0) \
     ? ((me_)->bits[6] == (uint32_t)0) \
     ? ((me_)->bits[7] == (uint32_t)0) \
     : 0)

/*! Evaluates to TRUE if the priority set @p me_ is not empty */
/* the following logic avoids UB in volatile access for MISRA compliantce */
#define QPSet_notEmpty(me_) \
    (((me_)->bits[0] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[1] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[2] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[3] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[4] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[5] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[6] != (uint32_t)0) \
     ? 1 \
     : ((me_)->bits[7] != (uint32_t)0))

/*! Evaluates to TRUE if the priority set @p me_ has element @p n_. */
#define QPSet_hasElement(me_, n_) \
    (((n_) <= (uint_fast8_t)32) \
     ? (((me_)->bits[0] & ((uint32_t)1 << ((n_) - (uint_fast8_t)1 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast8_t)64) \
     ? (((me_)->bits[1] & ((uint32_t)1 << ((n_) - (uint_fast8_t)33 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast8_t)96) \
     ? (((me_)->bits[2] & ((uint32_t)1 << ((n_) - (uint_fast8_t)65 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast8_t)128) \
     ? (((me_)->bits[3] & ((uint32_t)1 << ((n_) - (uint_fast8_t)97 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast8_t)160) \
     ? (((me_)->bits[4] & ((uint32_t)1 << ((n_) - (uint_fast8_t)129 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast8_t)192) \
     ? (((me_)->bits[5] & ((uint32_t)1 << ((n_) - (uint_fast8_t)161 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast8_t)224) \
     ? (((me_)->bits[6] & ((uint32_t)1 << ((n_) - (uint_fast8_t)193 ))) \
       != (uint32_t)0) \
     : (((me_)->bits[7] & ((uint32_t)1 << ((n_) - (uint_fast8_t)225))) \
       != (uint32_t)0))

/*! insert element @p n_ into the set @p me_, n_ = 1..64 */
#define QPSet_insert(me_, n_) do { \
    if ((n_) <= (uint_fast8_t)32) { \
        ((me_)->bits[0] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)1 ))); \
    } \
    else if ((n_) <= (uint_fast8_t)64) { \
        ((me_)->bits[1] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)33 ))); \
    } \
    else if ((n_) <= (uint_fast8_t)96) { \
        ((me_)->bits[2] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)65 ))); \
    } \
    else if ((n_) <= (uint_fast8_t)128) { \
        ((me_)->bits[3] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)97 ))); \
    } \
    else if ((n_) <= (uint_fast8_t)160) { \
        ((me_)->bits[4] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)129 ))); \
    } \
    else if ((n_) <= (uint_fast8_t)192) { \
        ((me_)->bits[5] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)161 ))); \
    } \
    else if ((n_) <= (uint_fast8_t)224) { \
        ((me_)->bits[6] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)193 ))); \
    } \
    else { \
        ((me_)->bits[7] |= ((uint32_t)1 << ((n_) - (uint_fast8_t)225))); \
    } \
} while (0)

/*! Remove element n_ from the set @p me_, n_= 1..64 */
#define QPSet_remove(me_, n_) do { \
    if ((n_) <= (uint_fast8_t)32) { \
        ((me_)->bits[0] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)1 )))); \
    } \
    else if ((n_) <= (uint_fast8_t)64) { \
        ((me_)->bits[1] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)33 )))); \
    } \
    else if ((n_) <= (uint_fast8_t)96) { \
        ((me_)->bits[2] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)65 )))); \
    } \
    else if ((n_) <= (uint_fast8_t)128) { \
        ((me_)->bits[3] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)97 )))); \
    } \
    else if ((n_) <= (uint_fast8_t)160) { \
        ((me_)->bits[4] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)129 )))); \
    } \
    else if ((n_) <= (uint_fast8_t)192) { \
        ((me_)->bits[5] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)161 )))); \
    } \
    else if ((n_) <= (uint_fast8_t)224) { \
        ((me_)->bits[6] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)193 )))); \
    } \
    else { \
        ((me_)->bits[7] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast8_t)225)))); \
    } \
} while (0)

/*! Find the maximum element in the set, and assign it to @p n_ */
/** @note if the set @p me_ is empty, @p n_ is set to zero.
*/
#define QPSet_findMax(me_, n_) \
    ((n_) = ((me_)->bits[7] != (uint32_t)0) \
        ? (QF_LOG2((me_)->bits[7]) + (uint_fast8_t)224) \
		: ((me_)->bits[6] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[6]) + (uint_fast8_t)192) \
		: ((me_)->bits[5] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[5]) + (uint_fast8_t)160) \
		: ((me_)->bits[4] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[4]) + (uint_fast8_t)128) \
		: ((me_)->bits[3] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[3]) + (uint_fast8_t)96) \
		: ((me_)->bits[2] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[2]) + (uint_fast8_t)64) \
		: ((me_)->bits[1] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[1]) + (uint_fast8_t)32) \
	: (QF_LOG2((me_)->bits[0])))


#else /* QF_MAX_ACTIVE > 256 */
/****************************************************************************/
/*! Priority Set of up to 1024 elements */
/**
* The priority set represents the set of active objects that are ready to
* run and need to be considered by the scheduling algorithm. The set is
* capable of storing up to 64 priority levels.
*/
typedef struct {
	uint32_t volatile bits[32];
} QPSet;

/*! Makes the priority set @p me_ empty */
#define QPSet_setEmpty(me_)  do { \
    (me_)->bits[0] = (uint32_t)0; \
    (me_)->bits[1] = (uint32_t)0; \
    (me_)->bits[2] = (uint32_t)0; \
    (me_)->bits[3] = (uint32_t)0; \
    (me_)->bits[4] = (uint32_t)0; \
    (me_)->bits[5] = (uint32_t)0; \
    (me_)->bits[6] = (uint32_t)0; \
    (me_)->bits[7] = (uint32_t)0; \
    (me_)->bits[8] = (uint32_t)0; \
    (me_)->bits[9] = (uint32_t)0; \
    (me_)->bits[10] = (uint32_t)0; \
    (me_)->bits[11] = (uint32_t)0; \
    (me_)->bits[12] = (uint32_t)0; \
    (me_)->bits[13] = (uint32_t)0; \
    (me_)->bits[14] = (uint32_t)0; \
    (me_)->bits[15] = (uint32_t)0; \
    (me_)->bits[16] = (uint32_t)0; \
    (me_)->bits[17] = (uint32_t)0; \
    (me_)->bits[18] = (uint32_t)0; \
    (me_)->bits[19] = (uint32_t)0; \
    (me_)->bits[20] = (uint32_t)0; \
    (me_)->bits[21] = (uint32_t)0; \
    (me_)->bits[22] = (uint32_t)0; \
    (me_)->bits[23] = (uint32_t)0; \
    (me_)->bits[24] = (uint32_t)0; \
    (me_)->bits[25] = (uint32_t)0; \
    (me_)->bits[26] = (uint32_t)0; \
    (me_)->bits[27] = (uint32_t)0; \
    (me_)->bits[28] = (uint32_t)0; \
    (me_)->bits[29] = (uint32_t)0; \
    (me_)->bits[30] = (uint32_t)0; \
    (me_)->bits[31] = (uint32_t)0; \
} while (0)

/*! Evaluates to TRUE if the priority set @p me_ is empty */
/* the following logic avoids UB in volatile access for MISRA compliantce */
#define QPSet_isEmpty(me_) \
    (((me_)->bits[0] == (uint32_t)0) \
     ? ((me_)->bits[1] == (uint32_t)0) \
     ? ((me_)->bits[2] == (uint32_t)0) \
     ? ((me_)->bits[3] == (uint32_t)0) \
     ? ((me_)->bits[4] == (uint32_t)0) \
     ? ((me_)->bits[5] == (uint32_t)0) \
     ? ((me_)->bits[6] == (uint32_t)0) \
     ? ((me_)->bits[8] == (uint32_t)0) \
     ? ((me_)->bits[9] == (uint32_t)0) \
     ? ((me_)->bits[10] == (uint32_t)0) \
     ? ((me_)->bits[11] == (uint32_t)0) \
     ? ((me_)->bits[12] == (uint32_t)0) \
     ? ((me_)->bits[13] == (uint32_t)0) \
     ? ((me_)->bits[14] == (uint32_t)0) \
     ? ((me_)->bits[15] == (uint32_t)0) \
     ? ((me_)->bits[16] == (uint32_t)0) \
     ? ((me_)->bits[17] == (uint32_t)0) \
     ? ((me_)->bits[18] == (uint32_t)0) \
     ? ((me_)->bits[19] == (uint32_t)0) \
     ? ((me_)->bits[20] == (uint32_t)0) \
     ? ((me_)->bits[21] == (uint32_t)0) \
     ? ((me_)->bits[22] == (uint32_t)0) \
     ? ((me_)->bits[23] == (uint32_t)0) \
     ? ((me_)->bits[24] == (uint32_t)0) \
     ? ((me_)->bits[25] == (uint32_t)0) \
     ? ((me_)->bits[26] == (uint32_t)0) \
     ? ((me_)->bits[27] == (uint32_t)0) \
     ? ((me_)->bits[28] == (uint32_t)0) \
     ? ((me_)->bits[29] == (uint32_t)0) \
     ? ((me_)->bits[30] == (uint32_t)0) \
     ? ((me_)->bits[31] == (uint32_t)0) \
     ? ((me_)->bits[32] == (uint32_t)0) \
     : 0)

/*! Evaluates to TRUE if the priority set @p me_ is not empty */
/* the following logic avoids UB in volatile access for MISRA compliantce */
#define QPSet_notEmpty(me_) \
    (((me_)->bits[0] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[1] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[2] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[3] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[4] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[5] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[6] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[7] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[8] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[9] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[10] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[11] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[12] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[13] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[14] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[15] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[16] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[17] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[18] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[19] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[20] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[21] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[22] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[23] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[24] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[25] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[26] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[27] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[28] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[29] != (uint32_t)0) \
     ? 1 \
	 : ((me_)->bits[30] != (uint32_t)0) \
     ? 1 \
     : ((me_)->bits[31] != (uint32_t)0))

/*! Evaluates to TRUE if the priority set @p me_ has element @p n_. */
#define QPSet_hasElement(me_, n_) \
    (((n_) <= (uint_fast16_t)32) \
     ? (((me_)->bits[0] & ((uint32_t)1 << ((n_) - (uint_fast16_t)1 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)64) \
     ? (((me_)->bits[1] & ((uint32_t)1 << ((n_) - (uint_fast16_t)33 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)96) \
     ? (((me_)->bits[2] & ((uint32_t)1 << ((n_) - (uint_fast16_t)65 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)128) \
     ? (((me_)->bits[3] & ((uint32_t)1 << ((n_) - (uint_fast16_t)97 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)160) \
     ? (((me_)->bits[4] & ((uint32_t)1 << ((n_) - (uint_fast16_t)129 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)192) \
     ? (((me_)->bits[5] & ((uint32_t)1 << ((n_) - (uint_fast16_t)161 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)224) \
     ? (((me_)->bits[6] & ((uint32_t)1 << ((n_) - (uint_fast16_t)193 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)256) \
     ? (((me_)->bits[7] & ((uint32_t)1 << ((n_) - (uint_fast16_t)225 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)288) \
     ? (((me_)->bits[8] & ((uint32_t)1 << ((n_) - (uint_fast16_t)257 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)320) \
     ? (((me_)->bits[9] & ((uint32_t)1 << ((n_) - (uint_fast16_t)289 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)352) \
     ? (((me_)->bits[10] & ((uint32_t)1 << ((n_) - (uint_fast16_t)321 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)384) \
     ? (((me_)->bits[11] & ((uint32_t)1 << ((n_) - (uint_fast16_t)353 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)416) \
     ? (((me_)->bits[12] & ((uint32_t)1 << ((n_) - (uint_fast16_t)385 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)448) \
     ? (((me_)->bits[13] & ((uint32_t)1 << ((n_) - (uint_fast16_t)417 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)480) \
     ? (((me_)->bits[14] & ((uint32_t)1 << ((n_) - (uint_fast16_t)449 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)512) \
     ? (((me_)->bits[15] & ((uint32_t)1 << ((n_) - (uint_fast16_t)481 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)544) \
     ? (((me_)->bits[16] & ((uint32_t)1 << ((n_) - (uint_fast16_t)513 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)576) \
     ? (((me_)->bits[17] & ((uint32_t)1 << ((n_) - (uint_fast16_t)545 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)608) \
     ? (((me_)->bits[18] & ((uint32_t)1 << ((n_) - (uint_fast16_t)577 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)640) \
     ? (((me_)->bits[19] & ((uint32_t)1 << ((n_) - (uint_fast16_t)609 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)672) \
     ? (((me_)->bits[20] & ((uint32_t)1 << ((n_) - (uint_fast16_t)641 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)704) \
     ? (((me_)->bits[21] & ((uint32_t)1 << ((n_) - (uint_fast16_t)673 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)736) \
     ? (((me_)->bits[22] & ((uint32_t)1 << ((n_) - (uint_fast16_t)705 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)768) \
     ? (((me_)->bits[23] & ((uint32_t)1 << ((n_) - (uint_fast16_t)737 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)800) \
     ? (((me_)->bits[24] & ((uint32_t)1 << ((n_) - (uint_fast16_t)769 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)832) \
     ? (((me_)->bits[25] & ((uint32_t)1 << ((n_) - (uint_fast16_t)801 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)864) \
     ? (((me_)->bits[26] & ((uint32_t)1 << ((n_) - (uint_fast16_t)833 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)896) \
     ? (((me_)->bits[27] & ((uint32_t)1 << ((n_) - (uint_fast16_t)865 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)928) \
     ? (((me_)->bits[28] & ((uint32_t)1 << ((n_) - (uint_fast16_t)897 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)960) \
     ? (((me_)->bits[29] & ((uint32_t)1 << ((n_) - (uint_fast16_t)929 ))) \
       != (uint32_t)0) \
	: ((n_) <= (uint_fast16_t)992) \
     ? (((me_)->bits[30] & ((uint32_t)1 << ((n_) - (uint_fast16_t)961 ))) \
       != (uint32_t)0) \
     : (((me_)->bits[31] & ((uint32_t)1 << ((n_) - (uint_fast16_t)993))) \
       != (uint32_t)0))

/*! insert element @p n_ into the set @p me_, n_ = 1..64 */
#define QPSet_insert(me_, n_) do { \
    if ((n_) <= (uint_fast16_t)32) { \
        ((me_)->bits[0] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)1 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)64) { \
        ((me_)->bits[1] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)33 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)96) { \
        ((me_)->bits[2] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)65 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)128) { \
        ((me_)->bits[3] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)97 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)160) { \
        ((me_)->bits[4] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)129 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)192) { \
        ((me_)->bits[5] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)161 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)224) { \
        ((me_)->bits[6] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)193 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)256) { \
        ((me_)->bits[7] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)225 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)288) { \
        ((me_)->bits[8] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)257 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)320) { \
        ((me_)->bits[9] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)289 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)352) { \
        ((me_)->bits[10] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)321 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)384) { \
        ((me_)->bits[11] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)353 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)416) { \
        ((me_)->bits[12] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)385 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)448) { \
        ((me_)->bits[13] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)417 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)480) { \
        ((me_)->bits[14] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)449 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)512) { \
        ((me_)->bits[15] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)481 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)544) { \
        ((me_)->bits[16] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)513 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)576) { \
        ((me_)->bits[17] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)545 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)608) { \
        ((me_)->bits[18] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)577 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)640) { \
        ((me_)->bits[19] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)609 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)672) { \
        ((me_)->bits[20] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)641 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)704) { \
        ((me_)->bits[21] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)673 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)736) { \
        ((me_)->bits[22] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)705 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)768) { \
        ((me_)->bits[23] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)737 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)800) { \
        ((me_)->bits[24] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)769 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)832) { \
        ((me_)->bits[25] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)801 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)864) { \
        ((me_)->bits[26] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)833 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)896) { \
        ((me_)->bits[27] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)865 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)928) { \
        ((me_)->bits[28] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)897 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)960) { \
        ((me_)->bits[29] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)929 ))); \
    } \
    else if ((n_) <= (uint_fast16_t)992) { \
        ((me_)->bits[30] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)961 ))); \
    } \
    else { \
        ((me_)->bits[31] |= ((uint32_t)1 << ((n_) - (uint_fast16_t)993))); \
    } \
} while (0)

/*! Remove element n_ from the set @p me_, n_= 1..64 */
#define QPSet_remove(me_, n_) do { \
    if ((n_) <= (uint_fast16_t)32) { \
        ((me_)->bits[0] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)1 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)64) { \
        ((me_)->bits[1] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)33 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)96) { \
        ((me_)->bits[2] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)65 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)128) { \
        ((me_)->bits[3] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)97 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)160) { \
        ((me_)->bits[4] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)129 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)192) { \
        ((me_)->bits[5] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)161 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)224) { \
        ((me_)->bits[6] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)193 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)256) { \
        ((me_)->bits[7] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)225 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)288) { \
        ((me_)->bits[8] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)257 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)320) { \
        ((me_)->bits[9] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)289 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)352) { \
        ((me_)->bits[10] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)321 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)384) { \
        ((me_)->bits[11] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)353 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)416) { \
        ((me_)->bits[12] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)385 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)448) { \
        ((me_)->bits[13] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)417 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)480) { \
        ((me_)->bits[14] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)449 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)512) { \
        ((me_)->bits[15] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)481 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)544) { \
        ((me_)->bits[16] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)513 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)576) { \
        ((me_)->bits[17] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)545 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)608) { \
        ((me_)->bits[18] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)577 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)640) { \
        ((me_)->bits[19] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)609 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)672) { \
        ((me_)->bits[20] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)641 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)704) { \
        ((me_)->bits[21] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)673 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)736) { \
        ((me_)->bits[22] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)705 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)768) { \
        ((me_)->bits[23] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)737 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)800) { \
        ((me_)->bits[24] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)769 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)832) { \
        ((me_)->bits[25] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)801 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)864) { \
        ((me_)->bits[26] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)833 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)896) { \
        ((me_)->bits[27] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)865 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)928) { \
        ((me_)->bits[28] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)897 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)960) { \
        ((me_)->bits[29] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)929 )))); \
    } \
    else if ((n_) <= (uint_fast16_t)992) { \
        ((me_)->bits[30] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)961 )))); \
    } \
    else { \
        ((me_)->bits[31] &= \
            (uint32_t)(~((uint32_t)1 << ((n_) - (uint_fast16_t)993)))); \
    } \
} while (0)

/*! Find the maximum element in the set, and assign it to @p n_ */
/** @note if the set @p me_ is empty, @p n_ is set to zero.
*/
#define QPSet_findMax(me_, n_) \
    ((n_) = ((me_)->bits[31] != (uint32_t)0) \
        ? (QF_LOG2((me_)->bits[31]) + (uint_fast16_t)992) \
		: ((me_)->bits[30] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[30]) + (uint_fast16_t)960) \
		: ((me_)->bits[29] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[29]) + (uint_fast16_t)928) \
		: ((me_)->bits[28] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[28]) + (uint_fast16_t)896) \
		: ((me_)->bits[27] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[27]) + (uint_fast16_t)864) \
		: ((me_)->bits[26] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[26]) + (uint_fast16_t)832) \
		: ((me_)->bits[25] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[25]) + (uint_fast16_t)800) \
		: ((me_)->bits[24] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[24]) + (uint_fast16_t)768) \
		: ((me_)->bits[23] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[23]) + (uint_fast16_t)736) \
		: ((me_)->bits[22] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[22]) + (uint_fast16_t)704) \
		: ((me_)->bits[21] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[21]) + (uint_fast16_t)672) \
		: ((me_)->bits[20] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[20]) + (uint_fast16_t)640) \
		: ((me_)->bits[19] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[19]) + (uint_fast16_t)608) \
		: ((me_)->bits[18] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[18]) + (uint_fast16_t)576) \
		: ((me_)->bits[17] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[17]) + (uint_fast16_t)544) \
		: ((me_)->bits[16] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[16]) + (uint_fast16_t)512) \
		: ((me_)->bits[15] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[15]) + (uint_fast16_t)480) \
		: ((me_)->bits[14] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[14]) + (uint_fast16_t)448) \
		: ((me_)->bits[13] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[13]) + (uint_fast16_t)416) \
		: ((me_)->bits[12] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[12]) + (uint_fast16_t)384) \
		: ((me_)->bits[11] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[11]) + (uint_fast16_t)352) \
		: ((me_)->bits[10] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[10]) + (uint_fast16_t)320) \
		: ((me_)->bits[9] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[9]) + (uint_fast16_t)288) \
		: ((me_)->bits[8] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[8]) + (uint_fast16_t)256) \
		: ((me_)->bits[7] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[7]) + (uint_fast16_t)224) \
		: ((me_)->bits[6] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[6]) + (uint_fast16_t)192) \
		: ((me_)->bits[5] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[5]) + (uint_fast16_t)160) \
		: ((me_)->bits[4] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[4]) + (uint_fast16_t)128) \
		: ((me_)->bits[3] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[3]) + (uint_fast16_t)96) \
		: ((me_)->bits[2] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[2]) + (uint_fast16_t)64) \
		: ((me_)->bits[1] != (uint32_t)0) \
		? (QF_LOG2((me_)->bits[1]) + (uint_fast16_t)32) \
	: (QF_LOG2((me_)->bits[0])))


#endif /* QF_MAX_ACTIVE */


/****************************************************************************/
/* Log-base-2 calculations ...*/
#ifndef QF_LOG2
    uint_fast8_t QF_LOG2(QPSetBits x);
#endif /* QF_LOG2 */

#endif /* qpset_h */
