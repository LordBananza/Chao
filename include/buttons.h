char fromStack;

void popStack() {
	/*This function does nothing and returns nothing; its purpose is to allow a register value to
	be interactable by a Chao variable*/
	return;
}


char buttonAPressed() {

	asm(ld p3);
	asm(and #$10);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}

char buttonBPressed() {

	asm(ld p3);
	asm(and #$20);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}

char buttonUpPressed() {

	asm(ld p3);
	asm(and #$1);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}

char buttonDownPressed() {

	asm(ld p3);
	asm(and #$2);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}

char buttonLeftPressed() {

	asm(ld p3);
	asm(and #$4);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}

char buttonRightPressed() {

	asm(ld p3);
	asm(and #$8);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}
