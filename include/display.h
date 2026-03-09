void clearScreen() {
	char count = 0;
	 
	asm(clr1 ocr,5);
	asm(mov #0, xbnk);
	asm(mov #$80, 2);
	
	
	while (count < 124) {
		asm(mov #$00, @R2);
		asm(inc 2);
		count = count + 1;
		
		
	}
	
	count = 0;
	asm(mov #$80, 2);
	asm(mov #1, xbnk);
	
	while (count < 124) {
		asm(mov #$00, @R2);
		asm(inc 2);
		count = count + 1;
	}
	
	asm(set1 ocr,5);
	
	return;
}

void fillScreen() {
	char count = 0;
	 
	asm(clr1 ocr,5);
	asm(mov #0, xbnk);
	asm(mov #$80, 2);
	
	
	while (count < 124) {
		asm(mov #$ff, @R2);
		asm(inc 2);
		count = count + 1;
		
		
	}
	
	count = 0;
	asm(mov #$80, 2);
	asm(mov #1, xbnk);
	
	while (count < 124) {
		asm(mov #$ff, @R2);
		asm(inc 2);
		count = count + 1;
	}
	
	asm(set1 ocr,5);
	
	return;
}

void clearScreenSlow() {
	char count = 0;
	 
	asm(mov #0, xbnk);
	asm(mov #$80, 2);
	
	
	while (count < 124) {
		asm(mov #$ff, @R2);
		asm(inc 2);
		count = count + 1;
		
		
	}
	
	count = 0;
	asm(mov #$80, 2);
	asm(mov #1, xbnk);
	
	while (count < 124) {
		asm(mov #$ff, @R2);
		asm(inc 2);
		count = count + 1;
	}
	
	
	return;
}

void fillScreenSlow() {
	char count = 0;
	 
	asm(mov #0, xbnk);
	asm(mov #$80, 2);
	
	
	while (count < 124) {
		asm(mov #$ff, @R2);
		asm(inc 2);
		count = count + 1;
		
		
	}
	
	count = 0;
	asm(mov #$80, 2);
	asm(mov #1, xbnk);
	
	while (count < 124) {
		asm(mov #$ff, @R2);
		asm(inc 2);
		count = count + 1;
	}
	
	
	return;
}

void drawScreen(char headLow, char headHigh) {
	/*TODO get high and low bytes of ROM address in pointers.h*/
	return;
}
