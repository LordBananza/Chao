char lowInUse;
char highInUse;

void sleep_ms (char ms, char multiplier) {

  asm(mov #128, t0lr);
  asm(mov #200, t0hr);
  
  /*
  TODO For multiplier times, load the proper values into T0LR and T0HR, and tT0PRR (prescaler), based on the timer period. Then set the control register to run, and wait in a while loop until the overflow bit is high
  */
  
  asm(mov #%01000000, t0con);
  asm(mov #128, t0prr);
  
  char waiting = 1;
  
  while (waiting) {
      asm(ld t0l);
      asm(bn t0con, 1, .skipPoint);
      waiting = 0;
      asm(mov #0, t0con);
      asm (.skipPoint:  nop);
  }
  return;
}


/* void sleep(s) {


} */ 
