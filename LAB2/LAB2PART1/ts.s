	// ts.s fi;e
	
	.global start
start:
         LDR sp, =stack_top
         BL main
         B .
