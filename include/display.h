char count;

void clearScreen() {
	count = 0;
	 
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
	count = 0;
	 
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
	count = 0;
	 
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

void fillScreenSlow() {
	count = 0;
	 
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

void drawScreen() {
	
	asm(clr1 ocr,5);
	
	count = 0;
	char rowCount = 0;
	
	asm(mov #0, xbnk);
	asm(mov #$80, 2);
	asm(mov #0, 3);
	
	while (rowCount > 8) {
		count = 0;
		while (count > 12) {
			asm(ld 3);
			asm(ldc);
			asm(st @R2);
			asm(inc 2);
			asm (inc 3);
		
			count = count + 1;
		}
		asm(ld 2);
		asm(add #4);
		asm(st 2);
		rowCount = rowCount + 1;
	}
	
	asm(mov #1, xbnk);
	asm(mov #$80, 2);
	rowCount = 0;
	
	while (rowCount > 8) {
		count = 0;
		while (count > 12) {
			asm(ld 3);
			asm(ldc);
			asm(st @R2);
			asm(inc 2);
			asm (inc 3);
		
			count = count + 1;
		}
		asm(ld 2);
		asm(add #4);
		asm(st 2);
		rowCount = rowCount + 1;
	}
	
	
	asm(set1 ocr,5);
	
	return;
}
