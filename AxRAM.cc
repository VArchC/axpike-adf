#include "sim.h"
#include <math.h>
#include <cstdlib>

unsigned long last_time_counter = 0;

unsigned long read_errors       = 0;

unsigned long stack_protected   = 0;

unsigned long instrs_last_log   = 0;

#define LOG(msg, ...) ;;

#define STACK_PROTECTION \
  if (p->get_state()->XPR[2] <= source->address) { \
    stack_protected++; \
    return; \
  }

std::vector<std::string> privileges(4);

bool RandomProbability(double probability) {
  return (static_cast<float>(rand()) <= (static_cast<float>(RAND_MAX) * probability));
}

bool UniformBitFlip(double probability, 
                   source_t* source, 
                   void* data,
                   unsigned long *error_counter,
                   unsigned int linesz,
                   bool persist=false) {

  unsigned int bit;
  unsigned int line;
    
  line = source->paddress & ~(linesz-1);

  if (RandomProbability(probability)) {

    void *addr;

    *error_counter++;
    bit = rand() % (linesz * 8);

    addr = source->bypass - (source->paddress - line);
    *(long long unsigned int *)addr ^= (0x1ULL << bit);
    data = (uint64_t*) (source->bypass);
    
    return true;
  }

  return false;
}


DM BitFlipRead(word sp, processor_t* p, source_t* source, void* data){

  memtracer_log_t memtracer_log = source->memtracer_log;
  bool loggable = memtracer_log.l2_miss;//All accesses to LLC
  bool exposed_to_error = memtracer_log.l2_miss && p->get_state()->prv == PRV_U;//Accesses to LLC of user level


  if (loggable) {
    privileges[PRV_U]  = "U";
    privileges[PRV_S]  = "S";
    privileges[PRV_HS] = "H";
    privileges[PRV_M]  = "M";

    auto  privilege = privileges[p->get_state()->prv];
    unsigned long instrs = p->ax_control.stats.instrs_counter - instrs_last_log;
    instrs_last_log = p->ax_control.stats.instrs_counter;

    if (memtracer_log.wb_address) {
      LOG(privilege + (exposed_to_error?" N ":" Y ") + " %lu %x %x\n", instrs, source->paddress, memtracer_log.wb_address);
    } else {
      LOG(privilege + (exposed_to_error?" N ":" Y ") + " %lu %x\n", instrs, source->paddress);
    }
  }


  //Verify if source is not DRAM or Privilege is not user
  if (!exposed_to_error)
    return;

  STACK_PROTECTION


  auto adele_params = p->get_adele_params();

  double read_probability = adele_params["mem_read_prob"].double_value;
  unsigned int linesz = adele_params["linesz"].int_value;

  UniformBitFlip(read_probability, source, data, &read_errors, linesz);

}



