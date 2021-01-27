#include "sim.h"
#include <math.h>
#include <cstdlib>

unsigned long last_time_counter = 0;

unsigned long read_errors       = 0;
unsigned long write_errors      = 0;
unsigned long hold_errors       = 0;

unsigned long stack_protected   = 0;

unsigned long counter = 0;

#define MODULO 100000

#define STACK_PROTECTION \
  if (p->get_state()->XPR[2] <= source->address) { \
    stack_protected++; \
    return; \
  }

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

  //Verify if source is not DRAM or Privilege is not user
  if (source->hierarchy != AxPIKE::Source::L3 ||
     (p->get_state()->prv != PRV_U) )
    return;

  STACK_PROTECTION


  //if (counter++ % MODULO == 0)
    //std::cerr << "counter " << std::dec << counter << std::endl;

  //unsigned long time_counter = p->get_time_counter();
  

  //unsigned long time_lapse = last_time_counter - time_counter;
  //last_time_counter = time_counter;

  //double hold_probability = p->get_adele_params()["mem_hold_prob"].double_value;
  //double probability = 1- pow(1-hold_probability, time_lapse);
  //std::cerr << "Hold probability: " << hold_probability << std::endl;
  //RandomBitFlip(probability, source, data, &hold_errors, true);

  auto adele_params = p->get_adele_params();

  double read_probability = adele_params["mem_read_prob"].double_value;
  //unsigned int linesz = adele_params.find("linesz") != adele_params.end()? adele_params["linesz"].int_value : 32;
  unsigned int linesz = adele_params["linesz"].int_value;
  //std::cerr << "Read probability: " << read_probability << std::endl;
  UniformBitFlip(read_probability, source, data, &read_errors, linesz);
  //if (UniformBitFlip(read_probability, source, data, &read_errors, linesz)) {
    //std::cerr << "Error inserted at " << data << std::endl;
  //}
}



//DM BitFlipWrite(word sp, processor_t* p, source_t* source, void* data){
//  //Verify if source is not DRAM
//  if (source->hierarchy != AxPIKE::Source::L3)
//    return;
//
//  STACK_PROTECTION
//  
//  //if (counter++ % MODULO == 0)
//    //std::cerr << "counter " << std::dec << counter << std::endl;
//
//  double write_probability = p->get_adele_params()["mem_write_prob"].double_value;
//  unsigned int linesz = p->get_adele_params()["linesz"].int_value;
//  //std::cerr << "Write probability: " << write_probability << std::endl;
//  UniformBitFlip(write_probability, source, data, &write_errors, linesz);
//}


//EM simple_em(processor_t* p){
//	// Implementation of simple_em
//  return 1;
//}

