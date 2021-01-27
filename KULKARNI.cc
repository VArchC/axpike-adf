uint8_t Kulkarni_base(uint8_t a, uint8_t b) {
  if (a == 0x3 && b == 0x3) {
    return 0x7;
  }
  else {
    return a*b;
  }
}

IM Kulkarni(word a, word b, word r, bool w, processor_t* p) {
  uint64_t res = 0;
  uint32_t op_a;
  uint32_t op_b;

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

  for (int i = 0; i < 64; i += 2) {
    for (int j = 0; j < 64; j += 2) {
      res += Kulkarni_base( (op_a&(0x3<<j))>>j, (op_b&(0x3<<i))>>i ) << (i + j);
    }
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
