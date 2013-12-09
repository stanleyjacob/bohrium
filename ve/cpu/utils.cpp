#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include "bh.h"
#include "bh_ve_cpu.h"

#include "auto.cpp"

/**
 * Read the entire file provided via filename into memory.
 *
 * It is the resposibility of the caller to de-allocate the buffer.
 *
 * @return size_t bytes read.
 */
size_t read_file(const char* filename, char** contents)
{
    int size = 0;
    
    std::ifstream file(filename, std::ios::in|std::ios::binary|std::ios::ate);
    
    if (file.is_open()) {
        size = file.tellg();
        *contents = (char*)malloc(size);
        file.seekg(0, std::ios::beg);
        file.read(*contents, size);
        file.close();
    }

    return size;
}

void assign_string(char*& output, const char* input)
{
    size_t length = strlen(input);

    output = (char*)malloc(sizeof(char) * length+1);
    if (!output) {
        std::cout << "Something went terribly wrong!" << std::endl;
    }
    strncpy(output, input, length);
    output[length] = '\0';
}

inline
bool is_dense(bh_view *operand)
{
    if ((operand->ndim == 3) && \
        (operand->stride[0] == 1) && \
        (operand->stride[1] == operand->shape[0]) && \
        (operand->stride[2] == operand->shape[1]*operand->shape[0])
    ) {
        return true;
    } else if ((operand->ndim == 2) && \
               (operand->stride[0] == 1) && \
               (operand->stride[1] == operand->shape[0])) {
        return true;
    } else if ((operand->ndim == 1) && (operand->stride[0] == 1)) {
        return true;
    }

    return false;
}

/**
 * Compute the layoutmask of the instruction.
 *
 */
int bh_layoutmask(bh_instruction *instr)
{
    int mask = 0;
    const int nops = bh_operands(instr->opcode);

    switch(nops) {
        case 3:
            mask |= (is_dense(&instr->operand[0])) ? A0_DENSE : A0_STRIDED;
            if (bh_is_constant(&instr->operand[2])) {
                mask |= (is_dense(&instr->operand[1])) ? A1_DENSE : A1_STRIDED;
                mask |= A2_CONSTANT;
            } else if (bh_is_constant(&instr->operand[1])) {
                mask |= A1_CONSTANT;
                mask |= (is_dense(&instr->operand[2])) ? A2_DENSE : A2_STRIDED;
            } else {
                mask |= (is_dense(&instr->operand[1])) ? A1_DENSE : A1_STRIDED;
                mask |= (is_dense(&instr->operand[2])) ? A2_DENSE : A2_STRIDED;
            }
            break;

        case 2:
            mask |= (is_dense(&instr->operand[0])) ? A0_DENSE : A0_STRIDED;
            if (bh_is_constant(&instr->operand[1])) {
                mask |= A1_CONSTANT;
            } else {
                mask |= (is_dense(&instr->operand[1])) ? A1_DENSE : A1_STRIDED;
            }
            break;

        case 1:
            mask |= (is_dense(&instr->operand[0])) ? A0_DENSE : A0_STRIDED;
            if (bh_is_constant(&instr->operand[1])) {
                mask |= A1_CONSTANT;
            } else {
                mask |= (is_dense(&instr->operand[1])) ? A1_DENSE : A1_STRIDED;
            }           
            break;

        case 0:
        default:
            break;
    }
    return mask;
}

const char* bh_layoutmask_to_shorthand(const int mask)
{
    switch(mask) {
        case 1: return "C";
        case 2: return "D";
        case 4: return "S";
        case 8: return "P";

        case 17: return "CC";
        case 18: return "DC";
        case 20: return "SC";
        case 24: return "PC";
        case 33: return "CD";
        case 34: return "DD";
        case 36: return "SD";
        case 40: return "PD";
        case 65: return "CS";
        case 66: return "DS";
        case 68: return "SS";
        case 72: return "PS";
        case 129: return "CP";
        case 130: return "DP";
        case 132: return "SP";
        case 136: return "PP";
        case 273: return "CCC";
        case 274: return "DCC";
        case 276: return "SCC";
        case 280: return "PCC";
        case 289: return "CDC";
        case 290: return "DDC";
        case 292: return "SDC";
        case 296: return "PDC";
        case 321: return "CSC";
        case 322: return "DSC";
        case 324: return "SSC";
        case 328: return "PSC";
        case 385: return "CPC";
        case 386: return "DPC";
        case 388: return "SPC";
        case 392: return "PPC";
        case 529: return "CCD";
        case 530: return "DCD";
        case 532: return "SCD";
        case 536: return "PCD";
        case 545: return "CDD";
        case 546: return "DDD";
        case 548: return "SDD";
        case 552: return "PDD";
        case 577: return "CSD";
        case 578: return "DSD";
        case 580: return "SSD";
        case 584: return "PSD";
        case 641: return "CPD";
        case 642: return "DPD";
        case 644: return "SPD";
        case 648: return "PPD";
        case 1041: return "CCS";
        case 1042: return "DCS";
        case 1044: return "SCS";
        case 1048: return "PCS";
        case 1057: return "CDS";
        case 1058: return "DDS";
        case 1060: return "SDS";
        case 1064: return "PDS";
        case 1089: return "CSS";
        case 1090: return "DSS";
        case 1092: return "SSS";
        case 1096: return "PSS";
        case 1153: return "CPS";
        case 1154: return "DPS";
        case 1156: return "SPS";
        case 1160: return "PPS";
        case 2065: return "CCP";
        case 2066: return "DCP";
        case 2068: return "SCP";
        case 2072: return "PCP";
        case 2081: return "CDP";
        case 2082: return "DDP";
        case 2084: return "SDP";
        case 2088: return "PDP";
        case 2113: return "CSP";
        case 2114: return "DSP";
        case 2116: return "SSP";
        case 2120: return "PSP";
        case 2177: return "CPP";
        case 2178: return "DPP";
        case 2180: return "SPP";
        case 2184: return "PPP";

        default:
            printf("Err: Unsupported layoutmask [%d]\n", mask);
            return "_UNS_";
    }
}

int bh_typesig(bh_instruction *instr)
{
    int typesig;
    const int nops = bh_operands(instr->opcode);
    switch(nops) {
        case 3:
            typesig = instr->operand[0].base->type+1;

            if (bh_is_constant(&instr->operand[1])) {             
                typesig += ((1+instr->constant.type) << 4) \
                          +((1+instr->operand[2].base->type) << 8);

            } else if (bh_is_constant(&instr->operand[2])) {      
                typesig += ((1+instr->operand[1].base->type) << 4) \
                          +((1+instr->constant.type) << 8);

            } else {                                                
                typesig += ((1+instr->operand[1].base->type) << 4) \
                          +((1+instr->operand[2].base->type) << 8);
            }
            break;
        case 2:
            typesig = instr->operand[0].base->type+1;

            if (bh_is_constant(&instr->operand[1])) {
                typesig += ((1+instr->constant.type) << 4);
            } else {
                typesig += ((1+instr->operand[1].base->type) << 4);
            }
            break;
        case 1:
            typesig = (1+instr->operand[0].base->type);
            break;
        case 0:
        default:
            typesig = 0;
            break;
    }
    return typesig;
}

const char* bh_typesig_to_shorthand(int typesig)
{
    switch(typesig) {
        // Sig-Length 3
        case 546: return "bbb"; // BH_INT8 + (BH_INT8 << 4) + (BH_INT8 << 8)
        case 3276: return "ddd"; // BH_FLOAT64 + (BH_FLOAT64 << 4) + (BH_FLOAT64 << 8)
        case 1911: return "SSS"; // BH_UINT16 + (BH_UINT16 << 4) + (BH_UINT16 << 8)
        case 2457: return "LLL"; // BH_UINT64 + (BH_UINT64 << 4) + (BH_UINT64 << 8)
        case 819: return "sss"; // BH_INT16 + (BH_INT16 << 4) + (BH_INT16 << 8)
        case 3003: return "fff"; // BH_FLOAT32 + (BH_FLOAT32 << 4) + (BH_FLOAT32 << 8)
        case 273: return "zzz"; // BH_BOOL + (BH_BOOL << 4) + (BH_BOOL << 8)
        case 1092: return "iii"; // BH_INT32 + (BH_INT32 << 4) + (BH_INT32 << 8)
        case 1638: return "BBB"; // BH_UINT8 + (BH_UINT8 << 4) + (BH_UINT8 << 8)
        case 1365: return "lll"; // BH_INT64 + (BH_INT64 << 4) + (BH_INT64 << 8)
        case 2184: return "III"; // BH_UINT32 + (BH_UINT32 << 4) + (BH_UINT32 << 8)
        case 3549: return "ccc"; // BH_COMPLEX64 + (BH_COMPLEX64 << 4) + (BH_COMPLEX64 << 8)
        case 3822: return "CCC"; // BH_COMPLEX128 + (BH_COMPLEX128 << 4) + (BH_COMPLEX128 << 8)
        case 545: return "zbb"; // BH_BOOL + (BH_INT8 << 4) + (BH_INT8 << 8)
        case 3265: return "zdd"; // BH_BOOL + (BH_FLOAT64 << 4) + (BH_FLOAT64 << 8)
        case 1905: return "zSS"; // BH_BOOL + (BH_UINT16 << 4) + (BH_UINT16 << 8)
        case 2449: return "zLL"; // BH_BOOL + (BH_UINT64 << 4) + (BH_UINT64 << 8)
        case 817: return "zss"; // BH_BOOL + (BH_INT16 << 4) + (BH_INT16 << 8)
        case 2993: return "zff"; // BH_BOOL + (BH_FLOAT32 << 4) + (BH_FLOAT32 << 8)
        case 1089: return "zii"; // BH_BOOL + (BH_INT32 << 4) + (BH_INT32 << 8)
        case 1633: return "zBB"; // BH_BOOL + (BH_UINT8 << 4) + (BH_UINT8 << 8)
        case 1361: return "zll"; // BH_BOOL + (BH_INT64 << 4) + (BH_INT64 << 8)
        case 2177: return "zII"; // BH_BOOL + (BH_UINT32 << 4) + (BH_UINT32 << 8)
        case 3537: return "zcc"; // BH_BOOL + (BH_COMPLEX64 << 4) + (BH_COMPLEX64 << 8)
        case 3809: return "zCC"; // BH_BOOL + (BH_COMPLEX128 << 4) + (BH_COMPLEX128 << 8)
        case 1314: return "bbl"; // BH_INT8 + (BH_INT8 << 4) + (BH_INT64 << 8)
        case 1484: return "ddl"; // BH_FLOAT64 + (BH_FLOAT64 << 4) + (BH_INT64 << 8)
        case 1399: return "SSl"; // BH_UINT16 + (BH_UINT16 << 4) + (BH_INT64 << 8)
        case 1433: return "LLl"; // BH_UINT64 + (BH_UINT64 << 4) + (BH_INT64 << 8)
        case 1331: return "ssl"; // BH_INT16 + (BH_INT16 << 4) + (BH_INT64 << 8)
        case 1467: return "ffl"; // BH_FLOAT32 + (BH_FLOAT32 << 4) + (BH_INT64 << 8)
        case 1297: return "zzl"; // BH_BOOL + (BH_BOOL << 4) + (BH_INT64 << 8)
        case 1348: return "iil"; // BH_INT32 + (BH_INT32 << 4) + (BH_INT64 << 8)
        case 1382: return "BBl"; // BH_UINT8 + (BH_UINT8 << 4) + (BH_INT64 << 8)
        case 1416: return "IIl"; // BH_UINT32 + (BH_UINT32 << 4) + (BH_INT64 << 8)
        case 1501: return "ccl"; // BH_COMPLEX64 + (BH_COMPLEX64 << 4) + (BH_INT64 << 8)
        case 1518: return "CCl"; // BH_COMPLEX128 + (BH_COMPLEX128 << 4) + (BH_INT64 << 8)
        case 2456: return "ILL"; // BH_UINT32 + (BH_UINT64 << 4) + (BH_UINT64 << 8)
        // Sig-Length 2
        case 34: return "bb"; // BH_INT8 + (BH_INT8 << 4)
        case 204: return "dd"; // BH_FLOAT64 + (BH_FLOAT64 << 4)
        case 119: return "SS"; // BH_UINT16 + (BH_UINT16 << 4)
        case 153: return "LL"; // BH_UINT64 + (BH_UINT64 << 4)
        case 51: return "ss"; // BH_INT16 + (BH_INT16 << 4)
        case 187: return "ff"; // BH_FLOAT32 + (BH_FLOAT32 << 4)
        case 17: return "zz"; // BH_BOOL + (BH_BOOL << 4)
        case 68: return "ii"; // BH_INT32 + (BH_INT32 << 4)
        case 102: return "BB"; // BH_UINT8 + (BH_UINT8 << 4)
        case 85: return "ll"; // BH_INT64 + (BH_INT64 << 4)
        case 136: return "II"; // BH_UINT32 + (BH_UINT32 << 4)
        case 221: return "cc"; // BH_COMPLEX64 + (BH_COMPLEX64 << 4)
        case 238: return "CC"; // BH_COMPLEX128 + (BH_COMPLEX128 << 4)
        case 177: return "zf"; // BH_BOOL + (BH_FLOAT32 << 4)
        case 193: return "zd"; // BH_BOOL + (BH_FLOAT64 << 4)
        case 33: return "zb"; // BH_BOOL + (BH_INT8 << 4)
        case 38: return "Bb"; // BH_UINT8 + (BH_INT8 << 4)
        case 35: return "sb"; // BH_INT16 + (BH_INT8 << 4)
        case 39: return "Sb"; // BH_UINT16 + (BH_INT8 << 4)
        case 36: return "ib"; // BH_INT32 + (BH_INT8 << 4)
        case 40: return "Ib"; // BH_UINT32 + (BH_INT8 << 4)
        case 37: return "lb"; // BH_INT64 + (BH_INT8 << 4)
        case 41: return "Lb"; // BH_UINT64 + (BH_INT8 << 4)
        case 43: return "fb"; // BH_FLOAT32 + (BH_INT8 << 4)
        case 44: return "db"; // BH_FLOAT64 + (BH_INT8 << 4)
        case 194: return "bd"; // BH_INT8 + (BH_FLOAT64 << 4)
        case 198: return "Bd"; // BH_UINT8 + (BH_FLOAT64 << 4)
        case 195: return "sd"; // BH_INT16 + (BH_FLOAT64 << 4)
        case 199: return "Sd"; // BH_UINT16 + (BH_FLOAT64 << 4)
        case 196: return "id"; // BH_INT32 + (BH_FLOAT64 << 4)
        case 200: return "Id"; // BH_UINT32 + (BH_FLOAT64 << 4)
        case 197: return "ld"; // BH_INT64 + (BH_FLOAT64 << 4)
        case 201: return "Ld"; // BH_UINT64 + (BH_FLOAT64 << 4)
        case 203: return "fd"; // BH_FLOAT32 + (BH_FLOAT64 << 4)
        case 113: return "zS"; // BH_BOOL + (BH_UINT16 << 4)
        case 114: return "bS"; // BH_INT8 + (BH_UINT16 << 4)
        case 118: return "BS"; // BH_UINT8 + (BH_UINT16 << 4)
        case 115: return "sS"; // BH_INT16 + (BH_UINT16 << 4)
        case 116: return "iS"; // BH_INT32 + (BH_UINT16 << 4)
        case 120: return "IS"; // BH_UINT32 + (BH_UINT16 << 4)
        case 117: return "lS"; // BH_INT64 + (BH_UINT16 << 4)
        case 121: return "LS"; // BH_UINT64 + (BH_UINT16 << 4)
        case 123: return "fS"; // BH_FLOAT32 + (BH_UINT16 << 4)
        case 124: return "dS"; // BH_FLOAT64 + (BH_UINT16 << 4)
        case 145: return "zL"; // BH_BOOL + (BH_UINT64 << 4)
        case 146: return "bL"; // BH_INT8 + (BH_UINT64 << 4)
        case 150: return "BL"; // BH_UINT8 + (BH_UINT64 << 4)
        case 147: return "sL"; // BH_INT16 + (BH_UINT64 << 4)
        case 151: return "SL"; // BH_UINT16 + (BH_UINT64 << 4)
        case 148: return "iL"; // BH_INT32 + (BH_UINT64 << 4)
        case 152: return "IL"; // BH_UINT32 + (BH_UINT64 << 4)
        case 149: return "lL"; // BH_INT64 + (BH_UINT64 << 4)
        case 155: return "fL"; // BH_FLOAT32 + (BH_UINT64 << 4)
        case 156: return "dL"; // BH_FLOAT64 + (BH_UINT64 << 4)
        case 49: return "zs"; // BH_BOOL + (BH_INT16 << 4)
        case 50: return "bs"; // BH_INT8 + (BH_INT16 << 4)
        case 54: return "Bs"; // BH_UINT8 + (BH_INT16 << 4)
        case 55: return "Ss"; // BH_UINT16 + (BH_INT16 << 4)
        case 52: return "is"; // BH_INT32 + (BH_INT16 << 4)
        case 56: return "Is"; // BH_UINT32 + (BH_INT16 << 4)
        case 53: return "ls"; // BH_INT64 + (BH_INT16 << 4)
        case 57: return "Ls"; // BH_UINT64 + (BH_INT16 << 4)
        case 59: return "fs"; // BH_FLOAT32 + (BH_INT16 << 4)
        case 60: return "ds"; // BH_FLOAT64 + (BH_INT16 << 4)
        case 178: return "bf"; // BH_INT8 + (BH_FLOAT32 << 4)
        case 182: return "Bf"; // BH_UINT8 + (BH_FLOAT32 << 4)
        case 179: return "sf"; // BH_INT16 + (BH_FLOAT32 << 4)
        case 183: return "Sf"; // BH_UINT16 + (BH_FLOAT32 << 4)
        case 180: return "if"; // BH_INT32 + (BH_FLOAT32 << 4)
        case 184: return "If"; // BH_UINT32 + (BH_FLOAT32 << 4)
        case 181: return "lf"; // BH_INT64 + (BH_FLOAT32 << 4)
        case 185: return "Lf"; // BH_UINT64 + (BH_FLOAT32 << 4)
        case 188: return "df"; // BH_FLOAT64 + (BH_FLOAT32 << 4)
        case 18: return "bz"; // BH_INT8 + (BH_BOOL << 4)
        case 22: return "Bz"; // BH_UINT8 + (BH_BOOL << 4)
        case 19: return "sz"; // BH_INT16 + (BH_BOOL << 4)
        case 23: return "Sz"; // BH_UINT16 + (BH_BOOL << 4)
        case 20: return "iz"; // BH_INT32 + (BH_BOOL << 4)
        case 24: return "Iz"; // BH_UINT32 + (BH_BOOL << 4)
        case 21: return "lz"; // BH_INT64 + (BH_BOOL << 4)
        case 25: return "Lz"; // BH_UINT64 + (BH_BOOL << 4)
        case 27: return "fz"; // BH_FLOAT32 + (BH_BOOL << 4)
        case 28: return "dz"; // BH_FLOAT64 + (BH_BOOL << 4)
        case 65: return "zi"; // BH_BOOL + (BH_INT32 << 4)
        case 66: return "bi"; // BH_INT8 + (BH_INT32 << 4)
        case 70: return "Bi"; // BH_UINT8 + (BH_INT32 << 4)
        case 67: return "si"; // BH_INT16 + (BH_INT32 << 4)
        case 71: return "Si"; // BH_UINT16 + (BH_INT32 << 4)
        case 72: return "Ii"; // BH_UINT32 + (BH_INT32 << 4)
        case 69: return "li"; // BH_INT64 + (BH_INT32 << 4)
        case 73: return "Li"; // BH_UINT64 + (BH_INT32 << 4)
        case 75: return "fi"; // BH_FLOAT32 + (BH_INT32 << 4)
        case 76: return "di"; // BH_FLOAT64 + (BH_INT32 << 4)
        case 97: return "zB"; // BH_BOOL + (BH_UINT8 << 4)
        case 98: return "bB"; // BH_INT8 + (BH_UINT8 << 4)
        case 99: return "sB"; // BH_INT16 + (BH_UINT8 << 4)
        case 103: return "SB"; // BH_UINT16 + (BH_UINT8 << 4)
        case 100: return "iB"; // BH_INT32 + (BH_UINT8 << 4)
        case 104: return "IB"; // BH_UINT32 + (BH_UINT8 << 4)
        case 101: return "lB"; // BH_INT64 + (BH_UINT8 << 4)
        case 105: return "LB"; // BH_UINT64 + (BH_UINT8 << 4)
        case 107: return "fB"; // BH_FLOAT32 + (BH_UINT8 << 4)
        case 108: return "dB"; // BH_FLOAT64 + (BH_UINT8 << 4)
        case 81: return "zl"; // BH_BOOL + (BH_INT64 << 4)
        case 82: return "bl"; // BH_INT8 + (BH_INT64 << 4)
        case 86: return "Bl"; // BH_UINT8 + (BH_INT64 << 4)
        case 83: return "sl"; // BH_INT16 + (BH_INT64 << 4)
        case 87: return "Sl"; // BH_UINT16 + (BH_INT64 << 4)
        case 84: return "il"; // BH_INT32 + (BH_INT64 << 4)
        case 88: return "Il"; // BH_UINT32 + (BH_INT64 << 4)
        case 89: return "Ll"; // BH_UINT64 + (BH_INT64 << 4)
        case 91: return "fl"; // BH_FLOAT32 + (BH_INT64 << 4)
        case 92: return "dl"; // BH_FLOAT64 + (BH_INT64 << 4)
        case 129: return "zI"; // BH_BOOL + (BH_UINT32 << 4)
        case 130: return "bI"; // BH_INT8 + (BH_UINT32 << 4)
        case 134: return "BI"; // BH_UINT8 + (BH_UINT32 << 4)
        case 131: return "sI"; // BH_INT16 + (BH_UINT32 << 4)
        case 135: return "SI"; // BH_UINT16 + (BH_UINT32 << 4)
        case 132: return "iI"; // BH_INT32 + (BH_UINT32 << 4)
        case 133: return "lI"; // BH_INT64 + (BH_UINT32 << 4)
        case 137: return "LI"; // BH_UINT64 + (BH_UINT32 << 4)
        case 139: return "fI"; // BH_FLOAT32 + (BH_UINT32 << 4)
        case 140: return "dI"; // BH_FLOAT64 + (BH_UINT32 << 4)
        case 29: return "cz"; // BH_COMPLEX64 + (BH_BOOL << 4)
        case 30: return "Cz"; // BH_COMPLEX128 + (BH_BOOL << 4)
        case 45: return "cb"; // BH_COMPLEX64 + (BH_INT8 << 4)
        case 46: return "Cb"; // BH_COMPLEX128 + (BH_INT8 << 4)
        case 109: return "cB"; // BH_COMPLEX64 + (BH_UINT8 << 4)
        case 110: return "CB"; // BH_COMPLEX128 + (BH_UINT8 << 4)
        case 61: return "cs"; // BH_COMPLEX64 + (BH_INT16 << 4)
        case 62: return "Cs"; // BH_COMPLEX128 + (BH_INT16 << 4)
        case 125: return "cS"; // BH_COMPLEX64 + (BH_UINT16 << 4)
        case 126: return "CS"; // BH_COMPLEX128 + (BH_UINT16 << 4)
        case 77: return "ci"; // BH_COMPLEX64 + (BH_INT32 << 4)
        case 78: return "Ci"; // BH_COMPLEX128 + (BH_INT32 << 4)
        case 141: return "cI"; // BH_COMPLEX64 + (BH_UINT32 << 4)
        case 142: return "CI"; // BH_COMPLEX128 + (BH_UINT32 << 4)
        case 93: return "cl"; // BH_COMPLEX64 + (BH_INT64 << 4)
        case 94: return "Cl"; // BH_COMPLEX128 + (BH_INT64 << 4)
        case 157: return "cL"; // BH_COMPLEX64 + (BH_UINT64 << 4)
        case 158: return "CL"; // BH_COMPLEX128 + (BH_UINT64 << 4)
        case 189: return "cf"; // BH_COMPLEX64 + (BH_FLOAT32 << 4)
        case 190: return "Cf"; // BH_COMPLEX128 + (BH_FLOAT32 << 4)
        case 205: return "cd"; // BH_COMPLEX64 + (BH_FLOAT64 << 4)
        case 206: return "Cd"; // BH_COMPLEX128 + (BH_FLOAT64 << 4)
        case 222: return "Cc"; // BH_COMPLEX128 + (BH_COMPLEX64 << 4)
        case 237: return "cC"; // BH_COMPLEX64 + (BH_COMPLEX128 << 4)
        // Sig-Length 1
        case 2: return "b"; // BH_INT8
        case 12: return "d"; // BH_FLOAT64
        case 7: return "S"; // BH_UINT16
        case 9: return "L"; // BH_UINT64
        case 3: return "s"; // BH_INT16
        case 11: return "f"; // BH_FLOAT32
        case 4: return "i"; // BH_INT32
        case 6: return "B"; // BH_UINT8
        case 5: return "l"; // BH_INT64
        case 8: return "I"; // BH_UINT32
        case 13: return "c"; // BH_COMPLEX64
        case 14: return "C"; // BH_COMPLEX128

        default:
            printf("Err: Unsupported type signature %d.\n", typesig);
            return "_UNS_";
    }
}

const char* bhopcode_to_cexpr(bh_opcode opcode)
{
    switch(opcode) {

        case BH_ADD_REDUCE:
            return "rvar += *tmp_current";
        case BH_MULTIPLY_REDUCE:
            return "rvar *= *tmp_current";
        case BH_MINIMUM_REDUCE:
            return "rvar = rvar < *tmp_current ? rvar : *tmp_current";
        case BH_MAXIMUM_REDUCE:
            return "rvar = rvar < *tmp_current ? *tmp_current : rvar";
        case BH_LOGICAL_AND_REDUCE:
            return "rvar = rvar && *tmp_current";
        case BH_BITWISE_AND_REDUCE:
            return "rvar &= *tmp_current";
        case BH_LOGICAL_OR_REDUCE:
            return "rvar = rvar || *tmp_current";
        case BH_BITWISE_OR_REDUCE:
            return "rvar |= *tmp_current";

        case BH_LOGICAL_XOR_REDUCE:
            return "rvar = !rvar != !*tmp_current";
        case BH_BITWISE_XOR_REDUCE:
            return "rvar = rvar ^ *tmp_current";

        // Binary elementwise: ADD, MULTIPLY...
        case BH_ADD:
            return "*a0_current = *a1_current + *a2_current";
        case BH_SUBTRACT:
            return "*a0_current = *a1_current - *a2_current";
        case BH_MULTIPLY:
            return "*a0_current = *a1_current * *a2_current";
        case BH_DIVIDE:
            return "*a0_current = *a1_current / *a2_current";
        case BH_POWER:
            return "*a0_current = pow( *a1_current, *a2_current )";
        case BH_GREATER:
            return "*a0_current = *a1_current > *a2_current";
        case BH_GREATER_EQUAL:
            return "*a0_current = *a1_current >= *a2_current";
        case BH_LESS:
            return "*a0_current = *a1_current < *a2_current";
        case BH_LESS_EQUAL:
            return "*a0_current = *a1_current <= *a2_current";
        case BH_EQUAL:
            return "*a0_current = *a1_current == *a2_current";
        case BH_NOT_EQUAL:
            return "*a0_current = *a1_current != *a2_current";
        case BH_LOGICAL_AND:
            return "*a0_current = *a1_current && *a2_current";
        case BH_LOGICAL_OR:
            return "*a0_current = *a1_current || *a2_current";
        case BH_LOGICAL_XOR:
            return "*a0_current = (!*a1_current != !*a2_current)";
        case BH_MAXIMUM:
            return "*a0_current = *a1_current < *a2_current ? *a2_current : *a1_current";
        case BH_MINIMUM:
            return "*a0_current = *a1_current < *a2_current ? *a1_current : *a2_current";
        case BH_BITWISE_AND:
            return "*a0_current = *a1_current & *a2_current";
        case BH_BITWISE_OR:
            return "*a0_current = *a1_current | *a2_current";
        case BH_BITWISE_XOR:
            return "*a0_current = *a1_current ^ *a2_current";
        case BH_LEFT_SHIFT:
            return "*a0_current = (*a1_current) << (*a2_current)";
        case BH_RIGHT_SHIFT:
            return "*a0_current = (*a1_current) >> (*a2_current)";
        case BH_ARCTAN2:
            return "*a0_current = atan2( *a1_current, *a2_current )";
        case BH_MOD:
            return "*a0_current = *a1_current - floor(*a1_current / *a2_current) * *a2_current";
        case BH_RANDOM:
            return  "threefry2x64_ctr_t ctr = {{*a2_current, 0}};            //index\n" \
                    "threefry2x64_key_t key = {{*a1_current, 0xdeadbeef}};  //seed\n"  \
                    "threefry2x64_ctr_t   c = threefry2x64(ctr, key);\n"       \
                    "*a0_current = c.v[0];\n";

        // Unary elementwise: SQRT, SIN...
        case BH_ABSOLUTE:
            return "*a0_current = *a1_current < 0.0 ? -*a1_current: *a1_current";
        case BH_LOGICAL_NOT:
            return "*a0_current = !*a1_current";
        case BH_INVERT:
            return "*a0_current = ~*a1_current";
        case BH_COS:
            return "*a0_current = cos( *a1_current )";
        case BH_SIN:
            return "*a0_current = sin( *a1_current )";
        case BH_TAN:
            return "*a0_current = tan( *a1_current )";
        case BH_COSH:
            return "*a0_current = cosh( *a1_current )";
        case BH_SINH:
            return "*a0_current = sinh( *a1_current )";
        case BH_TANH:
            return "*a0_current = tanh( *a1_current )";
        case BH_ARCSIN:
            return "*a0_current = asin( *a1_current )";
        case BH_ARCCOS:
            return "*a0_current = acos( *a1_current )";
        case BH_ARCTAN:
            return "*a0_current = atan( *a1_current )";
        case BH_ARCSINH:
            return "*a0_current = asinh( *a1_current )";
        case BH_ARCCOSH:
            return "*a0_current = acosh( *a1_current )";
        case BH_ARCTANH:
            return "*a0_current = atanh( *a1_current )";
        case BH_EXP:
            return "*a0_current = exp( *a1_current )";
        case BH_EXP2:
            return "*a0_current = pow( 2, *a1_current )";
        case BH_EXPM1:
            return "*a0_current = expm1( *a1_current )";
        case BH_LOG:
            return "*a0_current = log( *a1_current )";
        case BH_LOG2:
            return "*a0_current = log2( *a1_current )";
        case BH_LOG10:
            return "*a0_current = log10( *a1_current )";
        case BH_LOG1P:
            return "*a0_current = log1p( *a1_current )";
        case BH_SQRT:
            return "*a0_current = sqrt( *a1_current )";
        case BH_CEIL:
            return "*a0_current = ceil( *a1_current )";
        case BH_TRUNC:
            return "*a0_current = trunc( *a1_current )";
        case BH_FLOOR:
            return "*a0_current = floor( *a1_current )";
        case BH_RINT:
            return "*a0_current = (*a1_current > 0.0) ? floor(*a1_current + 0.5) : ceil(*a1_current - 0.5)";
        case BH_ISNAN:
            return "*a0_current = isnan(*a1_current)";
        case BH_ISINF:
            return "*a0_current = isinf(*a1_current)";
        case BH_IDENTITY:
            return "*a0_current = *a1_current";

        default:
            return "__UNKNOWN__";
    }
}
