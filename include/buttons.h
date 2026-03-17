void popStack() {
	/*do nothing*/
	return;
}


void buttonAPressed() {

	/*Collect value from p3, then AND them to only look at bit 5 (A button)
	If bit 5 is 0, A is pressed, return 1
	Otherwise, return 0*/

	asm(ld p3);
	asm(and #$10);
	asm(push acc);
	char a = popStack();
	
	if (a == 0) {
		return 1;
	}
	
	return 0;
}
