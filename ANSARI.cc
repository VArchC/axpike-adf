void nearestOneDetector(uint32_t x, uint32_t* m, uint32_t* k) {
  uint32_t check;
  *m = 1;
  *k = 0;
  for (int i = 30; i > 0; i--) {
    check = (x&(0x3 << i)) >> i;
    if (check == 0x3) {
      *m = 0x1 << (i+2);
      *k = i+2;
      break;
    }
    if (check == 0x2) {
      *m = 0x1 << (i+1);
      *k = i+1;
      break;
    }
  }
}

uint32_t setOneAdder(uint32_t a, uint32_t b, uint32_t c) {
  uint32_t r = 0x2AA; // SOA-11, LSBs = 01010101010
  uint32_t carry = ((a&0x400) + (b&0x400) + (c&0x400)) & 0x800;
  r += (a&0xFFFFF800) + (b&0xFFFFF800) + (c&0xFFFFF800) + carry;
  return r;
}

IM Ansari(word a, word b, word r, bool w, bool soa, processor_t* p) {
  uint64_t res = 0;
  uint32_t op_a;
  uint32_t op_b;
  uint32_t m1, m2, k1, k2, k12, q1, q2;

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

  nearestOneDetector(op_a, &m1, &k1);
  nearestOneDetector(op_b, &m2, &k2);
  q1 = op_a - m1;
  q2 = op_b - m2;
  if (soa) {
    res = setOneAdder(0x1 << (k1+k2), q2 << k1, q1 << k2);
  }
  else {
    res = (0x1 << (k1+k2)) + (q2 << k1) + (q1 << k2);
  }

  if ((a&0x80000000)^(b&0x80000000)) {
    res = -res;
  }

  if (w) {
    r = (res & 0xFFFFFFFF);
    if (res & 0x80000000) {
      r = r|0xFFFFFFFF00000000ULL;
    }
  }
  else {
    r = res;
  }
}
