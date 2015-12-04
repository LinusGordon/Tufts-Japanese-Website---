#include "bitpack.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

 /******************************************************************************
 *                                  CONSTANTS
 ******************************************************************************/
Except_T Bitpack_Overflow = { "Overflow packing bits" };
const unsigned WORDSIZE = 64;

/******************************************************************************
 *                             FUNCTION DEFINITIONS
 ******************************************************************************/

/*
 * Bitpack Fitsu: checks whether the unsigned value n can be represented by
 *                width bits  
 *
 * Parameters:
 *             n: value to check
 *             width: width of the value
 *
 * Returns: true if the value can be represented by the width, false otherwise
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        unsigned shift = 1 << width;
        return(n < shift && width < WORDSIZE) ;
}

/*
 * Bitpack Fitss: checks whether the signed value n can be represented by
 *                width bits  
 *
 * Parameters:
 *             n: value to check
 *             width: width of the value
 *
 * Returns: true if the value can be represented by the width, false otherwise
 */
bool Bitpack_fitss( int64_t n, unsigned width)
{
        if(width == 0)
                return false;
        int min = (1 << (width -1)) * -1;
        unsigned max = (1 << (width -1)) - 1;  

        return (n >= min && n <= max && width <= WORDSIZE);
} 

/*
 * Bitpack Getu: extracts the field from a bitword using the width and lsb  
 *
 * Parameters:
 *             words: bitword containing fields
 *             width: width of the value
 *             lsb: least significant bit
 *
 * Return: Returns the value from the requested field
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{

        assert(width + lsb <= WORDSIZE);
        uint64_t val = word;
        uint64_t mask = ~0;
      
        mask = (mask >> (WORDSIZE - width)) << lsb;
        val = (val & mask) >> lsb;
        return val; 
}

/*
 * Bitpack Gets: extracts the field from a bitword using the width and lsb  
 *
 * Parameters:
 *             words: bitword containing fields
 *             width: width of the value
 *             lsb: least significant bit
 *
 * Return: Returns the value from the requested field
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        
        assert(width + lsb <= WORDSIZE);
        int64_t val = word;
        uint64_t mask = ~0;
      
        mask = (mask >> (WORDSIZE - width)) << lsb;
        val = (val & mask) >> lsb;
        val = val << (WORDSIZE - width) >> (WORDSIZE - width);
        return val;
}

/*
 * Bitpack Newu: inserts a new unsigned value into the passed in word  
 *
 * Parameters:
 *             words: bitword containing fields
 *             width: width of the value
 *             lsb: least significant bit
 *             value: value to be inserted
 *
 * Return: Returns a new word with the inserted value
 */
uint64_t Bitpack_newu(uint64_t word, 
                      unsigned width, 
                      unsigned lsb, 
                      uint64_t value)
{
        if(!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        assert(width + lsb <= WORDSIZE);
        uint64_t new_word = word;
        uint64_t mask = ~0;

        mask = (mask >> (WORDSIZE - width)) << lsb;
        new_word |= mask;

        mask = ~0;
        mask = (mask >> (WORDSIZE - width)) << lsb;
        mask = ~mask;
        new_word &= mask;

        value = value << lsb;
        new_word |= value;

        return new_word;
}

/*
 * Bitpack News: inserts a new signed value into the passed in word  
 *
 * Parameters:
 *             words: bitword containing bit fields
 *             width: width of the value
 *             lsb: least significant bit
 *             value: value to be inserted
 *
 * Return: Returns a new word with the inserted value
 */
uint64_t Bitpack_news(uint64_t word, 
                      unsigned width, 
                      unsigned lsb,  
                      int64_t value)
{
        
        if(!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        assert(width <= WORDSIZE);
        assert(width + lsb <= WORDSIZE);
        uint64_t new_word = word;
        uint64_t mask = ~0;
        mask = (mask >> (WORDSIZE - width)) << lsb;
        mask = ~mask; 

        new_word = new_word & mask; 

        value = value << lsb;
        uint64_t mask2 = ~0;
        mask2 = (mask2>> (WORDSIZE - width)) << lsb;
        value &= mask2;

        new_word |= value;

        return new_word;
}