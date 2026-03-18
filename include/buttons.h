char fromStack;

void popStack() {
	/*This function does nothing and returns nothing; its purpose is to allow a register value to
	be interactable by a Chao variable*/
	return;
}


void buttonAPressed() {

	asm(ld p3);
	asm(and #$10);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}

void buttonBPressed() {

	asm(ld p3);
	asm(and #$20);
	asm(push acc);
	fromStack = popStack();
	
	if (fromStack == 0) {
		return 1;
	}
	
	return 0;
}
