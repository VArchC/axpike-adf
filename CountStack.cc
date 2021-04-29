DM TrackStackAccess(word sp, processor_t* p, source_t* source, void* data) {
  if (source->address >= sp) {
    COUNTERS(uint64_t, "StackAccesses")++;
    if (source->op == source_t::READ) {
      COUNTERS(uint64_t, "StackReads")++;
    }
    else if (source->op == source_t::WRITE) {
      COUNTERS(uint64_t, "StackWrites")++;
    }
  }
}
