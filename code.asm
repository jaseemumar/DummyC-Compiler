void factorial(){
pushi(ebp);
move(esp,ebp);
loadi(ind(ebp, 4), edx);
move(0, ecx);
cmpi(edx, ecx);
jle(L_2);
move(0,edx);
j(L_3);
L_2:
move(1,edx);
L_3:
cmpi(0,edx);
je(L_0);
move(1, edx);
storei(edx, ind(ebp, 8));
j(L_1);
L_0:
pushi(0);
loadi(ind(ebp, 4), ecx);
move(1, edx);
muli(-1.0, edx);
addi(edx, ecx);
pushi(ecx);
factorial();
popi(1);
loadi(ind(esp),ecx);
popi(1);
loadi(ind(ebp, 4), edx);
muli(ecx, edx);
storei(edx, ind(ebp, 8));
L_1:
loadi(ind(ebp), ebp);
popi(1);
}
void main(){
pushi(ebp);
move(esp,ebp);
move(-4,ecx);
pushi(0);
move(6, edx);
pushi(edx);
factorial();
popi(1);
loadi(ind(esp),edx);
popi(1);
storei(edx, ind(ebp,ecx));
loadi(ind(ebp, -4), edx);
print_int(edx);
print_string("\n");
move(-16,ecx);
move(8, edx);
storei(edx, ind(ebp,ecx));
loadi(ind(ebp, -16), edx);
print_int(edx);
print_string("\n");
move(-4,ecx);
move(4, edx);
storei(edx, ind(ebp,ecx));
move(-8,ecx);
move(23, edx);
storei(edx, ind(ebp,ecx));
loadi(ind(ebp, -4), edx);
loadi(ind(ebp, -16), ecx);
muli(ecx, edx);
move(-12,ecx);
storef(edx, ind(ebp,ecx));
loadi(ind(ebp, -12), edx);
print_int(edx);
print_string("\n");
move(8, edx);
move(5, ecx);
divi(ecx, edx);
move(-12,ecx);
storef(edx, ind(ebp,ecx));
loadi(ind(ebp, -12), edx);
print_int(edx);
print_string("\n");
loadi(ind(ebp, -4), edx);
loadi(ind(ebp, -16), ecx);
muli(ecx, edx);
loadi(ind(ebp, -8), ecx);
loadi(ind(ebp, -16), ebx);
muli(ebx, ecx);
move(5, ebx);
divi(ebx, ecx);
addi(ecx, edx);
move(-12,ecx);
storef(edx, ind(ebp,ecx));
loadi(ind(ebp, -12), edx);
print_int(edx);
print_string("\n");
loadi(ind(ebp, -4), edx);
loadi(ind(ebp, -16), ecx);
muli(ecx, edx);
loadi(ind(ebp, -8), ecx);
loadi(ind(ebp, -16), ebx);
muli(ebx, ecx);
move(5, ebx);
divi(ebx, ecx);
addi(ecx, edx);
loadi(ind(ebp, -16), ecx);
muli(-1.0, ecx);
addi(ecx, edx);
move(-12,ecx);
storef(edx, ind(ebp,ecx));
loadi(ind(ebp, -4), edx);
loadi(ind(ebp, -4), ecx);
addi(ecx, edx);
loadi(ind(ebp, -16), ecx);
addi(ecx, edx);
loadi(ind(ebp, -8), ecx);
addi(ecx, edx);
loadi(ind(ebp, -12), ecx);
addi(ecx, edx);
loadi(ind(ebp, -12), ecx);
loadi(ind(ebp, -8), ebx);
muli(ebx, ecx);
loadi(ind(ebp, -16), ebx);
addi(ebx, ecx);
addi(ecx, edx);
move(-16,ecx);
storef(edx, ind(ebp,ecx));
loadi(ind(ebp, -16), edx);
print_int(edx);
print_string("\n");
loadi(ind(ebp, -12), edx);
print_int(edx);
print_string("\n");
move(-4,ecx);
move(0, edx);
storei(edx, ind(ebp,ecx));
move(-16,ecx);
move(0, edx);
storei(edx, ind(ebp,ecx));
L_4:
loadi(ind(ebp, -16), edx);
move(4, ecx);
cmpi(edx, ecx);
jl(L_6);
move(0,edx);
j(L_7);
L_6:
move(1,edx);
L_7:
cmpi(0,edx);
je(L_5);
move(-8,edx);
move(0, ecx);
storei(ecx, ind(ebp,edx));
L_8:
loadi(ind(ebp, -8), ecx);
move(10, edx);
cmpi(ecx, edx);
jl(L_10);
move(0,ecx);
j(L_11);
L_10:
move(1,ecx);
L_11:
cmpi(0,ecx);
je(L_9);
loadi(ind(ebp, -8), ecx);
move(5, edx);
cmpi(0,edx);
jne(L_18);
loadi(ind(ebp, -16), edx);
cmpi(0,edx);
jne(L_18);
move(0,edx);
j(L_19);
L_18:
move(1,edx);
L_19:
cmpi(0, edx);
jne(L_16);
move(1,edx);
j(L_17);
L_16:
move(0,edx);
L_17:
cmpi(0,edx);
je(L_14);
loadi(ind(ebp, -4), edx);
move(10, ebx);
cmpi(edx, ebx);
jg(L_20);
move(0,edx);
j(L_21);
L_20:
move(1,edx);
L_21:
cmpi(0,edx);
je(L_14);
move(1,edx);
j(L_15);
L_14:
move(0,edx);
L_15:
cmpi(0,edx);
je(L_12);
j(L_13);
L_12:
loadi(ind(ebp, -4), edx);
loadi(ind(ebp, -8), ebx);
addi(ebx, edx);
move(-4,ebx);
storef(edx, ind(ebp,ebx));
L_13:
move(-8,ebx);
loadi(ind(ebp,ebx), edx);
addi(1, edx);
storei(edx, ind(ebp,ebx));
addi(-1, edx);
j(L_8);
L_9:
loadi(ind(ebp, -16), edx);
move(1, ebx);
addi(ebx, edx);
move(-16,ebx);
storef(edx, ind(ebp,ebx));
j(L_4);
L_5:
loadi(ind(ebp, -4), edx);
print_int(edx);
print_string("\n");
loadi(ind(ebp), ebp);
popi(1);
}
