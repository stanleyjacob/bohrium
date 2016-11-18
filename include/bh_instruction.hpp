#ifndef __BH_INSTRUCTION_H
#define __BH_INSTRUCTION_H

#include <boost/serialization/is_bitwise_serializable.hpp>
#include <boost/serialization/array.hpp>
#include <set>

#include "bh_opcode.h"
#include <bh_array.hpp>
#include "bh_error.h"

// Forward declaration of class boost::serialization::access
namespace boost {namespace serialization {class access;}}

// Maximum number of operands in a instruction.
#define BH_MAX_NO_OPERANDS (3)

//Memory layout of the Bohrium instruction
struct bh_instruction
{
    //Opcode: Identifies the operation
    bh_opcode  opcode;
    //Id of each operand
    bh_view  operand[BH_MAX_NO_OPERANDS];
    //Constant included in the instruction (Used if one of the operands == NULL)
    bh_constant constant;
    //Flag that indicates whether this instruction construct the output array (i.e. is the first operation on that array)
    //For now, this flag is only used by the code generators.
    bool constructor;

    bh_instruction(){}
    bh_instruction(const bh_instruction& instr)
    {
        opcode = instr.opcode;
        constant = instr.constant;
        constructor = instr.constructor;
        std::memcpy(operand, instr.operand, bh_noperands(opcode) * sizeof(bh_view));
    }

    // Return a set of all bases used by the instruction
    std::set<const bh_base *> get_bases() const;

    // Check if all views in this instruction is contiguous
    bool is_contiguous() const;

    // Check if all view in this instruction have the same shape
    bool all_same_shape() const;

    // Is this instruction (and all its views) reshapable?
    bool reshapable() const;

    // The maximum number of dimensions of a view in this instruction
    int64_t max_ndim() const;

    // Returns the shape of the view with the greatest number of dimensions
    // if equal, the combined maximum is returned
    std::vector<int64_t> dominating_shape() const;

    // Reshape the views of the instruction to 'shape'
    void reshape(const std::vector<int64_t> &shape);

    // Reshape the views of the instruction to 'shape' (no checks!)
    void reshape_force(const std::vector<int64_t> &shape);

    // Returns the type of the operand at given index (support constants)
    bh_type operand_type(int operand_index) const;

    // Equality
    bool operator==(const bh_instruction& other) const
    {
        if (opcode != other.opcode) return false;
        if (constant != other.constant) return false;

        for (bh_intp i = 0; i < BH_MAX_NO_OPERANDS; ++i) {
            if (operand[i] != other.operand[i]) return false;
        }

        return true;
    }

    // Inequality
    bool operator!=(const bh_instruction& other) const
    {
        return !(*this == other);
    }

    // Serialization using Boost
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & opcode;
        //We use make_array as a hack to make bh_constant BOOST_IS_BITWISE_SERIALIZABLE
        ar & boost::serialization::make_array(&constant, 1);
        const size_t nop = bh_noperands(opcode);
        for(size_t i=0; i<nop; ++i)
            ar & operand[i];
    }
};
BOOST_IS_BITWISE_SERIALIZABLE(bh_constant)

//Implements pprint of an instruction
DLLEXPORT std::ostream& operator<<(std::ostream& out, const bh_instruction& instr);

/* Retrive the operands of a instruction.
 *
 * @instruction  The instruction in question
 * @return The operand list
 */
DLLEXPORT bh_view *bh_inst_operands(bh_instruction *instruction);

/* Determines whether instruction 'a' depends on instruction 'b',
 * which is true when:
 *      'b' writes to an array that 'a' access
 *                        or
 *      'a' writes to an array that 'b' access
 *
 * @a The first instruction
 * @b The second instruction
 * @return The boolean answer
 */
DLLEXPORT bool bh_instr_dependency(const bh_instruction *a, const bh_instruction *b);


#endif
