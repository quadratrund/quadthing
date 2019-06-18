#ifndef SWITCH_H
#define SWITCH_H
extern uint8_t outputs[8];
extern uint8_t outState[8];
extern uint32_t safetyTimeout;
extern uint32_t lastNow;

bool switch_outputs(String params, String suffix = "");
#endif
