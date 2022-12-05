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

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

// TODO(shiyang): Gshare Data structures

// TODO(xuanda): Tournament Data structures

// TODO(all): Custom??????????


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//


//------------------------------------//
//        Init Functions              //
//------------------------------------//

void init_gshare_predictor() {

}


void init_tournament_predictor() {

}


void init_custom_predictor() {

}

// Initialize the predictor
//
void init_predictor() {
  switch (bpType) {
    case STATIC:
      return;
    case GSHARE:
      init_gshare_predictor();
    case TOURNAMENT:
      init_tournament_predictor();
    case CUSTOM:
      init_custom_predictor();
    default:
      break;
  }
}

//------------------------------------//
//        Prediction Functions        //
//------------------------------------//

// TODO(shiyang):
uint8_t make_prediction_gshare(uint32_t pc) {
  return NOTTAKEN;
}

// TODO(xuanda):
uint8_t make_prediction_tournament(uint32_t pc) {
  return NOTTAKEN;
}

// TODO(all):
uint8_t make_prediction_custom(uint32_t pc) {
  return NOTTAKEN;
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
      return make_prediction_custom(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// TODO(shiyang):
void train_predictor_gshare(uint32_t pc, uint8_t outcome) {

}

// TODO(xuanda):
void train_predictor_tournament(uint32_t pc, uint8_t outcome) {

}

// TODO(all):
void train_predictor_custom(uint32_t pc, uint8_t outcome) {

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
      train_predictor_custom(pc, outcome);
      return;
    default:
      return;
  }
}
