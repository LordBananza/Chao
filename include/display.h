void clearScreen() {
	char count = 0;
	 
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
	
	
	return;
}

void fillScreen() {
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
