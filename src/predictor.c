//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;
int threshold;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

// Gshare Data structures
uint32_t gshare_history_register;
uint32_t* gshare_history_table;
uint32_t gshare_mask;
int gshare_size;


// Tournament data structures
uint32_t tournament_global_history_register;
uint32_t* tournament_global_history_table;
uint32_t tournmanet_global_mask;
int tournment_global_size;
uint32_t* tournament_local_history_register;
uint32_t tournament_local_history_mask;
int tournament_local_history_size;
uint32_t* tournament_local_history_table;
int tournament_local_history_table_size;
uint32_t* tournament_choice_history_table;


// Custom data structures
uint64_t custom_history_register;
int custom_weight_table_size;
uint32_t custom_pc_index_mask;
int32_t* custom_weights;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//


//------------------------------------//
//        Init Functions              //
//------------------------------------//

void init_gshare_predictor() {
  gshare_size = 1 << ghistoryBits;
  gshare_mask = gshare_size - 1;
  gshare_history_register = 0;
  gshare_history_table = (uint32_t *)malloc(sizeof(uint32_t) * gshare_size);
  for (size_t i = 0; i < gshare_size; ++i) {
    // All 2-bit predictors should be initialized to WN (Weakly Not Taken).
    gshare_history_table[i] = WN;
  }
}

void init_tournament_predictor() {
  // init global
  tournment_global_size = 1 << ghistoryBits;
  tournmanet_global_mask = tournment_global_size - 1;
  tournament_global_history_register = 0;
  tournament_global_history_table = (uint32_t *)malloc(sizeof(uint32_t) * tournment_global_size);
  for (size_t i = 0; i < tournment_global_size; ++i) {
    tournament_global_history_table[i] = WN;
  }

  // init local
  tournament_local_history_size = 1 << pcIndexBits;
  tournament_local_history_mask = tournament_local_history_size - 1;
  tournament_local_history_register = (uint32_t *)malloc(sizeof(uint32_t) * tournament_local_history_size);
  for (size_t i = 0; i < tournament_local_history_size; ++i) {
    tournament_local_history_register[i] = NOTTAKEN;
  }
  tournament_local_history_table_size = 1 << lhistoryBits;
  tournament_local_history_table = (uint32_t *)malloc(sizeof(uint32_t) * tournament_local_history_table_size);
  for (size_t i = 0; i < tournament_local_history_table_size; ++i) {
    tournament_local_history_table[i] = WN;
  }
  tournament_choice_history_table = (uint32_t *)malloc(sizeof(uint32_t) * tournment_global_size);
  for (size_t i = 0; i < tournment_global_size; ++i) {
    tournament_choice_history_table[i] = WN;
  }
}

void init_perception_predictor() {
  threshold = 1.93 * ghistoryBits + 14;
  custom_history_register = NOTTAKEN;

  custom_weight_table_size = 1 << pcIndexBits;
  custom_pc_index_mask = custom_weight_table_size - 1;
  custom_weights = (int32_t*)malloc(sizeof(int32_t) * (ghistoryBits + 1) * custom_weight_table_size);
  for (size_t i = 0; i < (ghistoryBits + 1) * custom_weight_table_size; ++i) {
    custom_weights[i] = 0;
  }
}

void init_custom_predictor() {
  init_perception_predictor();
  init_gshare_predictor();
  init_tournament_predictor();
}

// Initialize the predictor
//
void init_predictor() {
  switch (bpType) {
    case STATIC:
      return;
    case GSHARE:
      init_gshare_predictor();
      return;
    case TOURNAMENT:
      init_tournament_predictor();
      return;
    case CUSTOM:
      // init_custom_predictor();
      init_perception_predictor();
      return;
    default:
      return;
  }
}

//------------------------------------//
//        Prediction Functions        //
//------------------------------------//

uint8_t make_prediction_gshare(uint32_t pc) {
  uint32_t i = (pc ^ gshare_history_register) & gshare_mask;
  uint32_t pattern = gshare_history_table[i];
  if (pattern >= WT) {
    return TAKEN;
  } else {
    return NOTTAKEN;
  }
}

uint32_t make_prediction_tournament_global(uint32_t pc) {
  uint32_t i = tournament_global_history_register & tournmanet_global_mask;
  uint32_t pattern = tournament_global_history_table[i];
  if (pattern >= WT) {
    return TAKEN;
  } else {
    return NOTTAKEN;
  }
}

uint32_t make_prediction_tournament_local(uint32_t pc) {
  uint32_t i = pc & tournament_local_history_mask;
  uint32_t history_i = tournament_local_history_register[i];
  uint32_t pattern = tournament_local_history_table[history_i];
  if (pattern >= WT) {
    return TAKEN;
  } else {
    return NOTTAKEN;
  }
}

uint8_t make_prediction_tournament(uint32_t pc) {
  uint32_t choice = tournament_choice_history_table[tournament_global_history_register];
  if (choice >= WT) {
    return make_prediction_tournament_local(pc);
  } else {
    return make_prediction_tournament_global(pc);
  }
}

uint8_t make_prediction_perception(uint32_t pc) {
  size_t base_idx = (pc & custom_pc_index_mask) * (ghistoryBits + 1);
  int prediction = custom_weights[base_idx + ghistoryBits];
  for (size_t i = 0; i < ghistoryBits; ++i) {
    uint8_t bit = (custom_history_register >> i) & 1;
    if (bit > 0) {
      prediction += custom_weights[base_idx + i];
    }
    else {
      prediction -= custom_weights[base_idx + i];
    }
  }
  return prediction > 0;
}

uint8_t make_prediction_custom(uint32_t pc) {
  uint8_t perception_outcome = make_prediction_perception(pc);
  uint8_t gshare_outcome = make_prediction_gshare(pc);
  uint8_t tournament_outcome = make_prediction_tournament(pc);

  if ((perception_outcome + gshare_outcome + tournament_outcome) > 1)
    return TAKEN;
  else return NOTTAKEN;
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc) {
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return make_prediction_gshare(pc);
    case TOURNAMENT:
      return make_prediction_tournament(pc);
    case CUSTOM:
      // return make_prediction_custom(pc);
      return make_prediction_perception(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

void train_predictor_gshare(uint32_t pc, uint8_t outcome) {
  uint32_t i = (pc ^ gshare_history_register) & gshare_mask;
  uint32_t pattern = gshare_history_table[i];
  if (outcome == TAKEN) {
    if (pattern != ST) {
      gshare_history_table[i]++;
    }
  } else {
    if (pattern != SN) {
      gshare_history_table[i]--;
    }
  }
  gshare_history_register = (gshare_history_register << 1 | outcome) & gshare_mask;
}

void train_predictor_tournament(uint32_t pc, uint8_t outcome) {
  uint32_t global_outcome = make_prediction_tournament_global(pc);
  uint32_t local_outcome = make_prediction_tournament_local(pc);

  // update choice
  if (global_outcome == outcome && local_outcome != outcome && tournament_choice_history_table[tournament_global_history_register] != SN) {
    tournament_choice_history_table[tournament_global_history_register]--;
  }
  if (local_outcome == outcome && global_outcome != outcome && tournament_choice_history_table[tournament_global_history_register] != ST) {
    tournament_choice_history_table[tournament_global_history_register]++;
  }

  // update global
  if (outcome == TAKEN) {
    if (tournament_global_history_table[tournament_global_history_register] != ST) {
      tournament_global_history_table[tournament_global_history_register]++;
    }
  } else {
    if (tournament_global_history_table[tournament_global_history_register] != SN) {
      tournament_global_history_table[tournament_global_history_register]--;
    }
  }

  // update local
  uint32_t i = pc & tournament_local_history_mask;
  uint32_t history_i = tournament_local_history_register[i];
  if (outcome == TAKEN) {
    if (tournament_local_history_table[history_i] != ST) {
      tournament_local_history_table[history_i]++;
    }
  } else {
    if (tournament_local_history_table[history_i] != SN) {
      tournament_local_history_table[history_i]--;
    }
  }

  // update registers
  tournament_global_history_register = (tournament_global_history_register << 1 | outcome) & tournmanet_global_mask;
  tournament_local_history_register[i] = (tournament_local_history_register[i] << 1 | outcome) & tournament_local_history_mask;
}

void train_predictor_perception(uint32_t pc, uint8_t outcome) {
  size_t base_idx = (pc & custom_pc_index_mask) * (ghistoryBits + 1);
  int prediction = custom_weights[base_idx + ghistoryBits];
  for (size_t i = 0; i < ghistoryBits; ++i) {
    uint8_t bit = (custom_history_register >> i) & 1;
    if (bit > 0) {
      prediction += custom_weights[base_idx + i];
    }
    else {
      prediction -= custom_weights[base_idx + i];
    }
  }
  uint8_t custom_outcome = prediction > 0;
  
  // train
  if ((custom_outcome != outcome) || (abs(prediction) < threshold)) {
    if (outcome == 1) {
      ++custom_weights[base_idx + ghistoryBits];
    }
    else {
      --custom_weights[base_idx + ghistoryBits];
    }
    for (size_t i = 0; i < ghistoryBits; ++i) {
      uint8_t bit = (custom_history_register >> i) & 1;
      if (bit == outcome) {
        ++custom_weights[base_idx + i];
      }
      else {
        --custom_weights[base_idx + i];
      }
    }
  }

  // update history
  custom_history_register = (custom_history_register << 1) | outcome;
}

void train_predictor_custom(uint32_t pc, uint8_t outcome) {
  train_predictor_gshare(pc, outcome);
  train_predictor_tournament(pc, outcome);
  train_predictor_perception(pc, outcome);
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome) {
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return;
    case GSHARE:
      train_predictor_gshare(pc, outcome);
      return;
    case TOURNAMENT:
      train_predictor_tournament(pc, outcome);
      return;
    case CUSTOM:
      // train_predictor_custom(pc, outcome);
      train_predictor_perception(pc, outcome);
      return;
    default:
      return;
  }
}
