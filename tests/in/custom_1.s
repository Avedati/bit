label1:
	jal ra, label2
	add x0, x0, x1
label2:
	addi t0, t3, 0b00111
	andi a0, a2, -3
	lhu s5 -4(s0)
	sh a2 0b110(s4)
	beq x0, x0 label1
	auipc a2, 0x12345000
	lui s3, -37
