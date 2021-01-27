#include "adele/adf/EvoApprox8B/mul8_303.c"
#include "adele/adf/EvoApprox8B/mul8_469.c"
#include "adele/adf/EvoApprox8B/mul8_479.c"
#include "adele/adf/EvoApprox8B/mul8_423.c"
#include "adele/adf/EvoApprox8B/mul8_279.c"

uint16_t mul8_accurate (uint8_t a, uint8_t b) {
    return ((uint16_t)a) * ((uint16_t)b);
}

uint16_t (*GetMul8(int m))(uint8_t, uint8_t) {
    switch (m) {
        case 303:
            return mul8_303; break;
        case 469:
            return mul8_469; break;
        case 479:
            return mul8_479; break;
        case 423:
            return mul8_423; break;
        case 279:
            return mul8_279; break;
        default:
            return mul8_accurate; break;
    }
}

uint64_t Mul8Wallace(uint32_t in_a, uint32_t in_b, uint16_t (*mul8)(uint8_t, uint8_t))
{
    uint64_t result = 0;
    uint64_t partial_b[4] = { 0, 0, 0, 0 };
    uint16_t b_x_a[4][4];

    for (int i = 0; i < 4; i++) {
        uint8_t byte_from_b = (in_b >> (8 * i)) & 0xff;
        for (int j = 0; j < 4; j++) {
            uint8_t byte_from_a = (in_a >> (8 * j)) & 0xff;
            b_x_a[i][j] = mul8(byte_from_b, byte_from_a);
            uint64_t mask = ~(uint64_t)0x0;
            mask <<= 8 * (j + sizeof b_x_a[i][j]);
            uint64_t prev = partial_b[i] & mask;
            partial_b[i] += (uint64_t)b_x_a[i][j] << (8 * j);
            partial_b[i] = prev | (partial_b[i] & ~mask);
        }
        uint64_t mask = ~(uint64_t)0x0;
        if (i == 3) {
            mask = 0x0;
        } else {
            mask <<= 8 * (i + 5);
        }
        uint64_t prev = result & mask;
        result += partial_b[i] << (8 * i);
        result = prev | (result & ~mask);
    }

    return result;
}

IM EvoApprox8B(word a, word b, word r, int mul, bool w, processor_t* p){
    // Implementation of EvoApprox8B
    uint32_t op_a, op_b;
    if (a&0x80000000) {
      op_a = -a;
    }
    else {
      op_a = a;
    }
    if (b&0x80000000) {
      op_b = -b;
    }
    else {
      op_b = b;
    }
    uint16_t (*mul8)(uint8_t, uint8_t) = GetMul8(mul);
    uint64_t result = Mul8Wallace(op_a, op_b, mul8);
    if ((a&0x80000000)^(b&0x80000000)) {
      result = -result;
    }
    if (w) {
      r = (result & 0xFFFFFFFF);
      if (result & 0x80000000) {
        r = r|0xFFFFFFFF00000000ULL;
      }
    }
    else r = result;
}
