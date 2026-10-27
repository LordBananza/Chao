char lowInUse;
char highInUse;

void sleep_ms (char ms, char multiplier) {

  pushStack(ms);
  
  asm(pop 5);
  
  asm(mov #255, acc);
  asm(sub 5);
  asm(st t0lr);
  /*
  TODO For multiplier times, load the proper values into T0LR and T0HR, and tT0PRR (prescaler), based on the timer period. Then set the control register to run, and wait in a while loop until the overflow bit is high
  */
  

  asm(mov #255, t0prr);
  asm(mov #%01000000, t0con);
  
  char waiting = 1;
  char i = 0;
  
  while (waiting) {
      while (i < multiplier) {
      asm(ld t0l);
      asm(bn t0con, 1, .skipPoint);
      waiting = 0;
      asm(mov #%01000000, t0con);
      i = i + 1;
      asm (.skipPoint:  nop);
      }
      asm(mov #0, t0con);
  }
  return;
}




void sleep(char s) {

  pushStack(s);
  
  asm(pop 5);
  
  asm(mov #255, acc);
  asm(sub 5);
  asm(clr1 psw, cy);
  asm(rolc);
  asm(st t0lr);
  /*
  TODO For multiplier times, load the proper values into T0LR and T0HR, and tT0PRR (prescaler), based on the timer period. Then set the control register to run, and wait in a while loop until the overflow bit is high
  */
  

  asm(mov #0, t0prr);
  asm(mov #%01000000, t0con);
  
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
