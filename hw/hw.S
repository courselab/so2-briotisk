	.code16                 # bits 16	
	.global _start	        

_start:
	mov $0xe, %ah                      # mov ah, 0xe
	
	mov $'H', %al                      # mov al, 'H'
	int $0x10                          # int 0x10
	
	mov $'e', %al                      # mov al, 'e'
	int $0x10                          # int 0x10
	
	mov $'l', %al                      # mov al, 'l'
	int $0x10                          # int 0x10
	
	mov $'l', %al                      # mov al, 'l'
	int $0x10                          # int 0x10
	
	mov $'o', %al                      # mov al, 'o'
	int $0x10                          # int 0x10
	
	mov $' ', %al                      # mov al, ' '
	int $0x10                          # int 0x10
	
	mov $'W', %al                      # mov al, 'W'
	int $0x10                          # int 0x10
	
	mov $'o', %al                      # mov al, 'o'
	int $0x10                          # int 0x10
	
	mov $'r', %al                      # mov al, 'r'
	int $0x10                          # int 0x10
	
	mov $'l', %al                      # mov al, 'l'
	int $0x10                          # int 0x10
	
	mov $'d', %al                      # mov al, 'd'
	int $0x10                          # int 0x10

halt:
	hlt								   #hlt
	jmp halt						   #jmp hlt

	.fill 510 - (. - _start), 1, 0	   #times 510 - ($-$$) db 0
	.word 0xaa55					   #dw 0xaa55	
