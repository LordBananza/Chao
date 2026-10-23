char count;

void pushStack() {
	/*This function only serves as a means to allow for pushing a variable onto the stack*/
	return;
}

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
	
	asm(pop trh);
	asm(pop trl);
	
	count = 0;
	char rowCount = 0;
	
	asm(mov #0, xbnk);
	asm(mov #$80, 2);
	asm(mov #0, 3);
	
	while (rowCount < 8) {
		count = 0;
		while (count < 12) {
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
	
	while (rowCount < 8) {
		count = 0;
		while (count < 12) {
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

void drawLine(char xCoord, char yCoord, char length) {
	
	asm(pop trh);
	asm(pop trl);
	
	char offset = xCoord;
	offset = offset / 8;
	offset = offset * 8;
	offset = xCoord - offset;
	
	asm(mov #0, xbnk);
		
	if (yCoord > 15) {
		asm(mov #1, xbnk);
		yCoord = yCoord - 16;
	}
	
	count = 0;
	asm(mov #$80, 2);
	
	xCoord = xCoord/8;
	pushStack(xCoord);
	
	asm(pop b);
	asm(ld 2);
	asm(add b);
	asm(st 2);	
	
	/*while (count < xCoord) {
		asm(inc 2);
		count = count + 8;
	}*/
	
	length = length/8;
	
	
	if (yCoord == 1) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord - 1;
	}
	
	if (yCoord == 3) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord - 1;
	}
	
	if (yCoord == 5) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 7) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 9) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 11) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 13) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 15) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	count = 1;
	
	yCoord = yCoord*8;
	pushStack(yCoord);
	
	asm(pop b);
	asm(ld 2);
	asm(add b);
	asm(st 2);	
	
	/*while (count < yCoord) {
	
		asm(ld 2);
		asm(add #$10);
		asm(st 2);
		count = count + 2;
		
	}*/
	
	
	count = 0;
	
	asm(mov #0, 3);

	while (count < length) {
			
			asm(ld 3);
			asm(ldc);
			
			
			/*
			Shift the the loaded byte to the right based on the offset (determined by x coordinate)
			*/
			asm(st 4);
			asm(st 5);
			
			
			  char i = 0;
			  while (i < offset) {
			    asm(ld 4);
			    asm(clr1 psw, cy);
			    asm(rorc);
			    asm(st 4);
			    i = i + 1;
			  }

			 
			  
			
			asm(ld 4);
			
			asm(or @R2);
			asm(st @R2);
			
			asm(inc 2);
			
			i = offset;
			while (i < 8) {
			    asm(ld 5);
			    asm(clr1 psw, cy);
			    asm(rolc);
			    asm(st 5);
			    i = i + 1;
			  }
			  
			  asm(nop);
			
			asm(ld 5);
			asm(st @R2);
			
			
			
			asm(nop);
				
			/*
			Shift the inverse amount of bits to the left, to ensure the full byte is drawn
			*/
			
			asm(inc 3);
			
			count = count + 1;
	}
		
	
	return;
}

void draw(char xCoord, char yCoord, char length, char height, char trh, char trl) {
	
	asm(clr1 ocr, 5);
	
	char end = yCoord;
	end = end + height;
	
	char bytes = length;
	bytes = length / 8;
	
	
	while (yCoord < end) {
		
		drawLine(xCoord, yCoord, length, trh, trl);
		yCoord = yCoord + 1;
		trl = trl + bytes;
	}
	
	asm(set1 ocr, 5);

	return;
}

void clearLine(char xCoord, char yCoord, char length) {
	
	char offset = xCoord;
	offset = offset / 8;
	offset = offset * 8;
	offset = xCoord - offset;
	
	asm(mov #0, xbnk);
		
	if (yCoord > 15) {
		asm(mov #1, xbnk);
		yCoord = yCoord - 16;
	}
	
	count = 0;
	asm(mov #$80, 2);
	
	xCoord = xCoord/8;
	pushStack(xCoord);
	
	asm(pop b);
	asm(ld 2);
	asm(add b);
	asm(st 2);	
	
	/*while (count < xCoord) {
		asm(inc 2);
		count = count + 8;
	}*/
	
	length = length/8;
	
	
	if (yCoord == 1) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord - 1;
	}
	
	if (yCoord == 3) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord - 1;
	}
	
	if (yCoord == 5) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 7) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 9) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 11) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 13) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	if (yCoord == 15) {
		asm(ld 2);
		asm(add #$6);
		asm(st 2);
		yCoord = yCoord -1;
	}
	
	count = 1;
	
	yCoord = yCoord*8;
	pushStack(yCoord);
	
	asm(pop b);
	asm(ld 2);
	asm(add b);
	asm(st 2);	
	
	/*while (count < yCoord) {
	
		asm(ld 2);
		asm(add #$10);
		asm(st 2);
		count = count + 2;
		
	}*/
	
	
	count = 0;
	
	asm(mov #0, 3);

	while (count < length) {
			
			asm(ld 3);
			asm(ldc);
			
			
			/*
			Shift the the loaded byte to the right based on the offset (determined by x coordinate)
			*/
			
			asm(mov #0, 4);
			asm(mov #0, 5);
			
			
			  char i = 0;
			   while (i < offset) {
			    asm(ld 4);
			    asm(set1 psw, cy);
			    asm(rorc);
			    asm(st 4);
			    i = i + 1;
			  }
			  

			 
			  
			
			asm(ld 4);
			
			asm(and @R2);
			asm(st @R2);
			
			asm(inc 2);
			
			
			/*
			Shift the inverse amount of bits to the left, to ensure the full byte is drawn
			*/
			
			asm(nop);
			
			i = offset;
			while (i < 8) {
			    asm(ld 5);
			    asm(set1 psw, cy);
			    asm(rolc);
			    asm(st 5);
			    i = i + 1;
			  }
			  
			  
			
			asm(ld 5);
			asm(and @R2);
			asm(st @R2);
				
			
			
			count = count + 1;
	}
		
	
	return;
}

void clear(char xCoord, char yCoord, char length, char height) {

      asm(clr1 ocr, 5);
	
	char end = yCoord;
	end = end + height;
	
	
	while (yCoord < end) {
		
		clearLine(xCoord, yCoord, length);
		yCoord = yCoord + 1;
	}
	
	asm(set1 ocr, 5);

	return;

}
