; Listing generated by Microsoft (R) Optimizing Compiler Version 17.00.60610.1 

	TITLE	D:\Personal\Projects\GBEmu\Solution\Code\graphics\eagle.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB MSVCRTD
INCLUDELIB OLDNAMES

PUBLIC	?img_scale_eagle@@YAXPAIHH0@Z			; img_scale_eagle
PUBLIC	?ScaleImageEagle2X@@YAXPAIHH0@Z			; ScaleImageEagle2X
PUBLIC	__xmm@00000001000000010000000100000001
EXTRN	@_RTC_CheckStackVars@8:PROC
EXTRN	__RTC_InitBase:PROC
EXTRN	__RTC_Shutdown:PROC
;	COMDAT __xmm@00000001000000010000000100000001
CONST	SEGMENT
__xmm@00000001000000010000000100000001 DB 01H, 00H, 00H, 00H, 01H, 00H, 00H
	DB	00H, 01H, 00H, 00H, 00H, 01H, 00H, 00H, 00H
CONST	ENDS
;	COMDAT rtc$TMZ
rtc$TMZ	SEGMENT
__RTC_Shutdown.rtc$TMZ DD FLAT:__RTC_Shutdown
rtc$TMZ	ENDS
;	COMDAT rtc$IMZ
rtc$IMZ	SEGMENT
__RTC_InitBase.rtc$IMZ DD FLAT:__RTC_InitBase
rtc$IMZ	ENDS
; Function compile flags: /Odtp /RTCsu
; File d:\personal\projects\gbemu\solution\code\graphics\eagle.cpp
_TEXT	SEGMENT
$T1 = -416						; size = 16
$T2 = -400						; size = 16
$T3 = -384						; size = 16
_pixels$4 = -352					; size = 16
$T5 = -320						; size = 16
_inv_factor$6 = -304					; size = 16
$T7 = -288						; size = 16
$T8 = -272						; size = 16
$T9 = -256						; size = 16
$T10 = -240						; size = 16
$T11 = -224						; size = 16
_factor$12 = -208					; size = 16
$T13 = -192						; size = 16
$T14 = -176						; size = 16
$T15 = -160						; size = 16
$T16 = -144						; size = 16
_C_splat$17 = -128					; size = 16
_r3$18 = -112						; size = 16
_r2$19 = -96						; size = 16
_r1$20 = -80						; size = 16
_x$21 = -52						; size = 4
_output_line2$22 = -48					; size = 4
_output_line1$23 = -44					; size = 4
_line$24 = -40						; size = 4
_y$25 = -36						; size = 4
$T26 = -32						; size = 16
_set_one$ = -16						; size = 16
_in_rgb$ = 8						; size = 4
_in_width$ = 12						; size = 4
_in_height$ = 16					; size = 4
_output$ = 20						; size = 4
?ScaleImageEagle2X@@YAXPAIHH0@Z PROC			; ScaleImageEagle2X

; 80   : void ScaleImageEagle2X(uint32_t* in_rgb,int in_width,int in_height,uint32_t* output) {

	push	ebx
	mov	ebx, esp
	sub	esp, 8
	and	esp, -16				; fffffff0H
	add	esp, 4
	push	ebp
	mov	ebp, DWORD PTR [ebx+4]
	mov	DWORD PTR [esp+4], ebp
	mov	ebp, esp
	sub	esp, 424				; 000001a8H
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-424]
	mov	ecx, 106				; 0000006aH
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 81   :   #define S  (line-in_width)[x-1]
; 82   :   #define T  (line-in_width)[x+0]
; 83   :   #define U  (line-in_width)[x+1]
; 84   :   #define V  line[x-1]
; 85   :   #define C  line[x+0]
; 86   :   #define W  line[x+1]
; 87   :   #define X  (line+in_width)[x-1]
; 88   :   #define Y  (line+in_width)[x+0]
; 89   :   #define Z  (line+in_width)[x+1]
; 90   :   const __m128i set_one = _mm_set1_epi32(1);

	movdqa	xmm0, XMMWORD PTR __xmm@00000001000000010000000100000001
	movdqa	XMMWORD PTR $T26[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T26[ebp]
	movdqa	XMMWORD PTR _set_one$[ebp], xmm0

; 91   :   
; 92   :   for (int y=1;y<in_height-1;++y) {

	mov	DWORD PTR _y$25[ebp], 1
	jmp	SHORT $LN6@ScaleImage
$LN5@ScaleImage:
	mov	eax, DWORD PTR _y$25[ebp]
	add	eax, 1
	mov	DWORD PTR _y$25[ebp], eax
$LN6@ScaleImage:
	mov	ecx, DWORD PTR _in_height$[ebx]
	sub	ecx, 1
	cmp	DWORD PTR _y$25[ebp], ecx
	jge	$LN7@ScaleImage

; 93   :     auto line = &in_rgb[y*in_width];

	mov	edx, DWORD PTR _y$25[ebp]
	imul	edx, DWORD PTR _in_width$[ebx]
	mov	eax, DWORD PTR _in_rgb$[ebx]
	lea	ecx, DWORD PTR [eax+edx*4]
	mov	DWORD PTR _line$24[ebp], ecx

; 94   :     auto output_line1 = &output[2+(y<<1)*(in_width<<1)];

	mov	edx, DWORD PTR _y$25[ebp]
	shl	edx, 1
	mov	eax, DWORD PTR _in_width$[ebx]
	shl	eax, 1
	imul	edx, eax
	mov	ecx, DWORD PTR _output$[ebx]
	lea	edx, DWORD PTR [ecx+edx*4+8]
	mov	DWORD PTR _output_line1$23[ebp], edx

; 95   :     auto output_line2 = &output[2+(1+(y<<1))*(in_width<<1)];

	mov	eax, DWORD PTR _y$25[ebp]
	lea	ecx, DWORD PTR [eax+eax+1]
	mov	edx, DWORD PTR _in_width$[ebx]
	shl	edx, 1
	imul	ecx, edx
	mov	eax, DWORD PTR _output$[ebx]
	lea	ecx, DWORD PTR [eax+ecx*4+8]
	mov	DWORD PTR _output_line2$22[ebp], ecx

; 96   :     for (int x=1;x<in_width-1;++x) {

	mov	DWORD PTR _x$21[ebp], 1
	jmp	SHORT $LN3@ScaleImage
$LN2@ScaleImage:
	mov	edx, DWORD PTR _x$21[ebp]
	add	edx, 1
	mov	DWORD PTR _x$21[ebp], edx
$LN3@ScaleImage:
	mov	eax, DWORD PTR _in_width$[ebx]
	sub	eax, 1
	cmp	DWORD PTR _x$21[ebp], eax
	jge	$LN1@ScaleImage

; 97   :       __m128i r1,r2,r3;
; 98   :       const __m128i C_splat = _mm_set1_epi32(C);

	mov	ecx, DWORD PTR _x$21[ebp]
	mov	edx, DWORD PTR _line$24[ebp]
	mov	eax, DWORD PTR [edx+ecx*4]
	movd	xmm0, eax
	pshufd	xmm0, xmm0, 0
	movdqa	XMMWORD PTR $T16[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T16[ebp]
	movdqa	XMMWORD PTR _C_splat$17[ebp], xmm0

; 99   :         
; 100  :       r1 = _mm_set_epi32(V,T,V,W);

	mov	ecx, DWORD PTR _x$21[ebp]
	mov	edx, DWORD PTR _line$24[ebp]
	mov	eax, DWORD PTR [edx+ecx*4+4]
	mov	ecx, DWORD PTR _x$21[ebp]
	mov	edx, DWORD PTR _line$24[ebp]
	mov	ecx, DWORD PTR [edx+ecx*4-4]
	mov	edx, DWORD PTR _in_width$[ebx]
	shl	edx, 2
	mov	esi, DWORD PTR _line$24[ebp]
	sub	esi, edx
	mov	edx, DWORD PTR _x$21[ebp]
	mov	edx, DWORD PTR [esi+edx*4]
	mov	esi, DWORD PTR _x$21[ebp]
	mov	edi, DWORD PTR _line$24[ebp]
	mov	esi, DWORD PTR [edi+esi*4-4]
	movd	xmm0, esi
	movd	xmm1, edx
	movd	xmm2, ecx
	movd	xmm3, eax
	punpckldq xmm3, xmm1
	punpckldq xmm2, xmm0
	punpckldq xmm3, xmm2
	movdqa	XMMWORD PTR $T15[ebp], xmm3
	movdqa	xmm0, XMMWORD PTR $T15[ebp]
	movdqa	XMMWORD PTR _r1$20[ebp], xmm0

; 101  :       r2 = _mm_set_epi32(S,U,X,Z);

	mov	eax, DWORD PTR _in_width$[ebx]
	mov	ecx, DWORD PTR _line$24[ebp]
	lea	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _x$21[ebp]
	mov	ecx, DWORD PTR [edx+eax*4+4]
	mov	edx, DWORD PTR _in_width$[ebx]
	mov	eax, DWORD PTR _line$24[ebp]
	lea	edx, DWORD PTR [eax+edx*4]
	mov	eax, DWORD PTR _x$21[ebp]
	mov	edx, DWORD PTR [edx+eax*4-4]
	mov	eax, DWORD PTR _in_width$[ebx]
	shl	eax, 2
	mov	esi, DWORD PTR _line$24[ebp]
	sub	esi, eax
	mov	eax, DWORD PTR _x$21[ebp]
	mov	eax, DWORD PTR [esi+eax*4+4]
	mov	esi, DWORD PTR _in_width$[ebx]
	shl	esi, 2
	mov	edi, DWORD PTR _line$24[ebp]
	sub	edi, esi
	mov	esi, DWORD PTR _x$21[ebp]
	mov	esi, DWORD PTR [edi+esi*4-4]
	movd	xmm0, esi
	movd	xmm1, eax
	movd	xmm2, edx
	movd	xmm3, ecx
	punpckldq xmm3, xmm1
	punpckldq xmm2, xmm0
	punpckldq xmm3, xmm2
	movdqa	XMMWORD PTR $T14[ebp], xmm3
	movdqa	xmm0, XMMWORD PTR $T14[ebp]
	movdqa	XMMWORD PTR _r2$19[ebp], xmm0

; 102  :       r3 = _mm_set_epi32(T,W,Y,Y);

	mov	ecx, DWORD PTR _in_width$[ebx]
	mov	edx, DWORD PTR _line$24[ebp]
	lea	eax, DWORD PTR [edx+ecx*4]
	mov	ecx, DWORD PTR _x$21[ebp]
	mov	edx, DWORD PTR [eax+ecx*4]
	mov	eax, DWORD PTR _in_width$[ebx]
	mov	ecx, DWORD PTR _line$24[ebp]
	lea	eax, DWORD PTR [ecx+eax*4]
	mov	ecx, DWORD PTR _x$21[ebp]
	mov	eax, DWORD PTR [eax+ecx*4]
	mov	ecx, DWORD PTR _x$21[ebp]
	mov	esi, DWORD PTR _line$24[ebp]
	mov	ecx, DWORD PTR [esi+ecx*4+4]
	mov	esi, DWORD PTR _in_width$[ebx]
	shl	esi, 2
	mov	edi, DWORD PTR _line$24[ebp]
	sub	edi, esi
	mov	esi, DWORD PTR _x$21[ebp]
	mov	esi, DWORD PTR [edi+esi*4]
	movd	xmm0, esi
	movd	xmm1, ecx
	movd	xmm2, eax
	movd	xmm3, edx
	punpckldq xmm3, xmm1
	punpckldq xmm2, xmm0
	punpckldq xmm3, xmm2
	movdqa	XMMWORD PTR $T13[ebp], xmm3
	movdqa	xmm0, XMMWORD PTR $T13[ebp]
	movdqa	XMMWORD PTR _r3$18[ebp], xmm0

; 103  : 
; 104  :       auto factor = _mm_and_si128(
; 105  :         _mm_and_si128(_mm_cmpeq_epi32(r1,r2),set_one),
; 106  :         _mm_and_si128(_mm_cmpeq_epi32(r2,r3),set_one));

	movdqa	xmm0, XMMWORD PTR _r2$19[ebp]
	pcmpeqd	xmm0, XMMWORD PTR _r3$18[ebp]
	movdqa	XMMWORD PTR $T9[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T9[ebp]
	pand	xmm0, XMMWORD PTR _set_one$[ebp]
	movdqa	XMMWORD PTR $T8[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR _r1$20[ebp]
	pcmpeqd	xmm0, XMMWORD PTR _r2$19[ebp]
	movdqa	XMMWORD PTR $T11[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T11[ebp]
	pand	xmm0, XMMWORD PTR _set_one$[ebp]
	movdqa	XMMWORD PTR $T10[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T10[ebp]
	pand	xmm0, XMMWORD PTR $T8[ebp]
	movdqa	XMMWORD PTR $T7[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T7[ebp]
	movdqa	XMMWORD PTR _factor$12[ebp], xmm0

; 107  :       auto inv_factor = _mm_sub_epi32(set_one,factor);

	movdqa	xmm0, XMMWORD PTR _set_one$[ebp]
	psubd	xmm0, XMMWORD PTR _factor$12[ebp]
	movdqa	XMMWORD PTR $T5[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T5[ebp]
	movdqa	XMMWORD PTR _inv_factor$6[ebp], xmm0

; 108  : 
; 109  :       auto pixels = _mm_add_epi32(_mm_mullo_epi32(r2,factor),_mm_mullo_epi32(C_splat,inv_factor));

	movdqa	xmm0, XMMWORD PTR _C_splat$17[ebp]
	pmulld	xmm0, XMMWORD PTR _inv_factor$6[ebp]
	movdqa	XMMWORD PTR $T2[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR _r2$19[ebp]
	pmulld	xmm0, XMMWORD PTR _factor$12[ebp]
	movdqa	XMMWORD PTR $T3[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T3[ebp]
	paddd	xmm0, XMMWORD PTR $T2[ebp]
	movdqa	XMMWORD PTR $T1[ebp], xmm0
	movdqa	xmm0, XMMWORD PTR $T1[ebp]
	movdqa	XMMWORD PTR _pixels$4[ebp], xmm0

; 110  : 
; 111  :       *output_line1++ = pixels.m128i_u32[3];

	mov	edx, 4
	imul	edx, 3
	mov	eax, DWORD PTR _output_line1$23[ebp]
	mov	ecx, DWORD PTR _pixels$4[ebp+edx]
	mov	DWORD PTR [eax], ecx
	mov	edx, DWORD PTR _output_line1$23[ebp]
	add	edx, 4
	mov	DWORD PTR _output_line1$23[ebp], edx

; 112  :       *output_line1++ = pixels.m128i_u32[2];

	mov	eax, 4
	shl	eax, 1
	mov	ecx, DWORD PTR _output_line1$23[ebp]
	mov	edx, DWORD PTR _pixels$4[ebp+eax]
	mov	DWORD PTR [ecx], edx
	mov	eax, DWORD PTR _output_line1$23[ebp]
	add	eax, 4
	mov	DWORD PTR _output_line1$23[ebp], eax

; 113  :       *output_line2++ = pixels.m128i_u32[1];

	mov	ecx, 4
	shl	ecx, 0
	mov	edx, DWORD PTR _output_line2$22[ebp]
	mov	eax, DWORD PTR _pixels$4[ebp+ecx]
	mov	DWORD PTR [edx], eax
	mov	ecx, DWORD PTR _output_line2$22[ebp]
	add	ecx, 4
	mov	DWORD PTR _output_line2$22[ebp], ecx

; 114  :       *output_line2++ = pixels.m128i_u32[0];

	mov	edx, 4
	imul	edx, 0
	mov	eax, DWORD PTR _output_line2$22[ebp]
	mov	ecx, DWORD PTR _pixels$4[ebp+edx]
	mov	DWORD PTR [eax], ecx
	mov	edx, DWORD PTR _output_line2$22[ebp]
	add	edx, 4
	mov	DWORD PTR _output_line2$22[ebp], edx

; 115  :     }

	jmp	$LN2@ScaleImage
$LN1@ScaleImage:

; 116  :   }

	jmp	$LN5@ScaleImage
$LN7@ScaleImage:

; 117  : #undef S
; 118  : #undef T
; 119  : #undef U
; 120  : #undef V
; 121  : #undef W
; 122  : #undef X
; 123  : #undef Y
; 124  : #undef Z
; 125  : #undef C
; 126  : };

	push	edx
	mov	ecx, ebp
	push	eax
	lea	edx, DWORD PTR $LN11@ScaleImage
	call	@_RTC_CheckStackVars@8
	pop	eax
	pop	edx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	mov	esp, ebx
	pop	ebx
	ret	0
	npad	3
$LN11@ScaleImage:
	DD	1
	DD	$LN10@ScaleImage
$LN10@ScaleImage:
	DD	-352					; fffffea0H
	DD	16					; 00000010H
	DD	$LN9@ScaleImage
$LN9@ScaleImage:
	DB	112					; 00000070H
	DB	105					; 00000069H
	DB	120					; 00000078H
	DB	101					; 00000065H
	DB	108					; 0000006cH
	DB	115					; 00000073H
	DB	0
?ScaleImageEagle2X@@YAXPAIHH0@Z ENDP			; ScaleImageEagle2X
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu
; File d:\personal\projects\gbemu\solution\code\graphics\eagle.cpp
_TEXT	SEGMENT
tv208 = -80						; size = 4
tv204 = -76						; size = 4
tv190 = -72						; size = 4
tv186 = -68						; size = 4
tv173 = -64						; size = 4
tv169 = -60						; size = 4
tv146 = -56						; size = 4
_test$1 = -49						; size = 1
_Z$2 = -48						; size = 4
_Y$3 = -44						; size = 4
_X$4 = -40						; size = 4
_W$5 = -36						; size = 4
_C$6 = -32						; size = 4
_V$7 = -28						; size = 4
_U$8 = -24						; size = 4
_T$9 = -20						; size = 4
_S$10 = -16						; size = 4
_x$11 = -12						; size = 4
_line$12 = -8						; size = 4
_y$13 = -4						; size = 4
_in_rgb$ = 8						; size = 4
_in_width$ = 12						; size = 4
_in_height$ = 16					; size = 4
_output$ = 20						; size = 4
?img_scale_eagle@@YAXPAIHH0@Z PROC			; img_scale_eagle

; 54   : void img_scale_eagle(uint32_t* in_rgb,int in_width,int in_height,uint32_t* output) {

	push	ebp
	mov	ebp, esp
	sub	esp, 80					; 00000050H
	push	edi
	lea	edi, DWORD PTR [ebp-80]
	mov	ecx, 20					; 00000014H
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 55   :     for (int y=1;y<in_height-1;++y) {

	mov	DWORD PTR _y$13[ebp], 1
	jmp	SHORT $LN6@img_scale_
$LN5@img_scale_:
	mov	eax, DWORD PTR _y$13[ebp]
	add	eax, 1
	mov	DWORD PTR _y$13[ebp], eax
$LN6@img_scale_:
	mov	ecx, DWORD PTR _in_height$[ebp]
	sub	ecx, 1
	cmp	DWORD PTR _y$13[ebp], ecx
	jge	$LN7@img_scale_

; 56   :       auto line = &in_rgb[y*in_width];

	mov	edx, DWORD PTR _y$13[ebp]
	imul	edx, DWORD PTR _in_width$[ebp]
	mov	eax, DWORD PTR _in_rgb$[ebp]
	lea	ecx, DWORD PTR [eax+edx*4]
	mov	DWORD PTR _line$12[ebp], ecx

; 57   :       for (int x=1;x<in_width-1;++x) {

	mov	DWORD PTR _x$11[ebp], 1
	jmp	SHORT $LN3@img_scale_
$LN2@img_scale_:
	mov	edx, DWORD PTR _x$11[ebp]
	add	edx, 1
	mov	DWORD PTR _x$11[ebp], edx
$LN3@img_scale_:
	mov	eax, DWORD PTR _in_width$[ebp]
	sub	eax, 1
	cmp	DWORD PTR _x$11[ebp], eax
	jge	$LN1@img_scale_

; 58   :         auto S = (line-in_width)[x-1];

	mov	ecx, DWORD PTR _in_width$[ebp]
	shl	ecx, 2
	mov	edx, DWORD PTR _line$12[ebp]
	sub	edx, ecx
	mov	eax, DWORD PTR _x$11[ebp]
	mov	ecx, DWORD PTR [edx+eax*4-4]
	mov	DWORD PTR _S$10[ebp], ecx

; 59   :         auto T = (line-in_width)[x+0];

	mov	edx, DWORD PTR _in_width$[ebp]
	shl	edx, 2
	mov	eax, DWORD PTR _line$12[ebp]
	sub	eax, edx
	mov	ecx, DWORD PTR _x$11[ebp]
	mov	edx, DWORD PTR [eax+ecx*4]
	mov	DWORD PTR _T$9[ebp], edx

; 60   :         auto U = (line-in_width)[x+1];

	mov	eax, DWORD PTR _in_width$[ebp]
	shl	eax, 2
	mov	ecx, DWORD PTR _line$12[ebp]
	sub	ecx, eax
	mov	edx, DWORD PTR _x$11[ebp]
	mov	eax, DWORD PTR [ecx+edx*4+4]
	mov	DWORD PTR _U$8[ebp], eax

; 61   :         auto V = line[x-1];

	mov	ecx, DWORD PTR _x$11[ebp]
	mov	edx, DWORD PTR _line$12[ebp]
	mov	eax, DWORD PTR [edx+ecx*4-4]
	mov	DWORD PTR _V$7[ebp], eax

; 62   :         auto C = line[x+0];

	mov	ecx, DWORD PTR _x$11[ebp]
	mov	edx, DWORD PTR _line$12[ebp]
	mov	eax, DWORD PTR [edx+ecx*4]
	mov	DWORD PTR _C$6[ebp], eax

; 63   :         auto W = line[x+1];

	mov	ecx, DWORD PTR _x$11[ebp]
	mov	edx, DWORD PTR _line$12[ebp]
	mov	eax, DWORD PTR [edx+ecx*4+4]
	mov	DWORD PTR _W$5[ebp], eax

; 64   :         auto X = (line+in_width)[x-1];

	mov	ecx, DWORD PTR _in_width$[ebp]
	mov	edx, DWORD PTR _line$12[ebp]
	lea	eax, DWORD PTR [edx+ecx*4]
	mov	ecx, DWORD PTR _x$11[ebp]
	mov	edx, DWORD PTR [eax+ecx*4-4]
	mov	DWORD PTR _X$4[ebp], edx

; 65   :         auto Y = (line+in_width)[x+0];

	mov	eax, DWORD PTR _in_width$[ebp]
	mov	ecx, DWORD PTR _line$12[ebp]
	lea	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _x$11[ebp]
	mov	ecx, DWORD PTR [edx+eax*4]
	mov	DWORD PTR _Y$3[ebp], ecx

; 66   :         auto Z = (line+in_width)[x+1];

	mov	edx, DWORD PTR _in_width$[ebp]
	mov	eax, DWORD PTR _line$12[ebp]
	lea	ecx, DWORD PTR [eax+edx*4]
	mov	edx, DWORD PTR _x$11[ebp]
	mov	eax, DWORD PTR [ecx+edx*4+4]
	mov	DWORD PTR _Z$2[ebp], eax

; 67   : 
; 68   :         auto test = (V==S && V==T);

	mov	ecx, DWORD PTR _V$7[ebp]
	cmp	ecx, DWORD PTR _S$10[ebp]
	jne	SHORT $LN9@img_scale_
	mov	edx, DWORD PTR _V$7[ebp]
	cmp	edx, DWORD PTR _T$9[ebp]
	jne	SHORT $LN9@img_scale_
	mov	DWORD PTR tv146[ebp], 1
	jmp	SHORT $LN10@img_scale_
$LN9@img_scale_:
	mov	DWORD PTR tv146[ebp], 0
$LN10@img_scale_:
	mov	al, BYTE PTR tv146[ebp]
	mov	BYTE PTR _test$1[ebp], al

; 69   : 
; 70   :         output[(x<<1)+(y<<2)*in_width] = S*test + C*(1-test);

	movzx	ecx, BYTE PTR _test$1[ebp]
	imul	ecx, DWORD PTR _S$10[ebp]
	movzx	edx, BYTE PTR _test$1[ebp]
	mov	eax, 1
	sub	eax, edx
	imul	eax, DWORD PTR _C$6[ebp]
	add	ecx, eax
	mov	edx, DWORD PTR _y$13[ebp]
	shl	edx, 2
	imul	edx, DWORD PTR _in_width$[ebp]
	mov	eax, DWORD PTR _x$11[ebp]
	lea	edx, DWORD PTR [edx+eax*2]
	mov	eax, DWORD PTR _output$[ebp]
	mov	DWORD PTR [eax+edx*4], ecx

; 71   :         output[(1+(x<<1))+(y<<2)*in_width] = U*((T==U && T==W)) + C*(1-(T==U && T==W));

	mov	ecx, DWORD PTR _T$9[ebp]
	cmp	ecx, DWORD PTR _U$8[ebp]
	jne	SHORT $LN11@img_scale_
	mov	edx, DWORD PTR _T$9[ebp]
	cmp	edx, DWORD PTR _W$5[ebp]
	jne	SHORT $LN11@img_scale_
	mov	DWORD PTR tv169[ebp], 1
	jmp	SHORT $LN12@img_scale_
$LN11@img_scale_:
	mov	DWORD PTR tv169[ebp], 0
$LN12@img_scale_:
	mov	eax, DWORD PTR _T$9[ebp]
	cmp	eax, DWORD PTR _U$8[ebp]
	jne	SHORT $LN13@img_scale_
	mov	ecx, DWORD PTR _T$9[ebp]
	cmp	ecx, DWORD PTR _W$5[ebp]
	jne	SHORT $LN13@img_scale_
	mov	DWORD PTR tv173[ebp], 1
	jmp	SHORT $LN14@img_scale_
$LN13@img_scale_:
	mov	DWORD PTR tv173[ebp], 0
$LN14@img_scale_:
	mov	edx, DWORD PTR _U$8[ebp]
	imul	edx, DWORD PTR tv169[ebp]
	mov	eax, 1
	sub	eax, DWORD PTR tv173[ebp]
	imul	eax, DWORD PTR _C$6[ebp]
	add	edx, eax
	mov	ecx, DWORD PTR _y$13[ebp]
	shl	ecx, 2
	imul	ecx, DWORD PTR _in_width$[ebp]
	mov	eax, DWORD PTR _x$11[ebp]
	lea	ecx, DWORD PTR [ecx+eax*2+1]
	mov	eax, DWORD PTR _output$[ebp]
	mov	DWORD PTR [eax+ecx*4], edx

; 72   :         output[(x<<1)+(2+(y<<2))*in_width] = X*((V==X && X==Y)) + C*(1-(V==X && X==Y));

	mov	ecx, DWORD PTR _V$7[ebp]
	cmp	ecx, DWORD PTR _X$4[ebp]
	jne	SHORT $LN15@img_scale_
	mov	edx, DWORD PTR _X$4[ebp]
	cmp	edx, DWORD PTR _Y$3[ebp]
	jne	SHORT $LN15@img_scale_
	mov	DWORD PTR tv186[ebp], 1
	jmp	SHORT $LN16@img_scale_
$LN15@img_scale_:
	mov	DWORD PTR tv186[ebp], 0
$LN16@img_scale_:
	mov	eax, DWORD PTR _V$7[ebp]
	cmp	eax, DWORD PTR _X$4[ebp]
	jne	SHORT $LN17@img_scale_
	mov	ecx, DWORD PTR _X$4[ebp]
	cmp	ecx, DWORD PTR _Y$3[ebp]
	jne	SHORT $LN17@img_scale_
	mov	DWORD PTR tv190[ebp], 1
	jmp	SHORT $LN18@img_scale_
$LN17@img_scale_:
	mov	DWORD PTR tv190[ebp], 0
$LN18@img_scale_:
	mov	edx, DWORD PTR _X$4[ebp]
	imul	edx, DWORD PTR tv186[ebp]
	mov	eax, 1
	sub	eax, DWORD PTR tv190[ebp]
	imul	eax, DWORD PTR _C$6[ebp]
	add	edx, eax
	mov	ecx, DWORD PTR _y$13[ebp]
	lea	eax, DWORD PTR [ecx*4+2]
	imul	eax, DWORD PTR _in_width$[ebp]
	mov	ecx, DWORD PTR _x$11[ebp]
	lea	eax, DWORD PTR [eax+ecx*2]
	mov	ecx, DWORD PTR _output$[ebp]
	mov	DWORD PTR [ecx+eax*4], edx

; 73   :         output[(1+(x<<1))+(2+(y<<2))*in_width] = Z*((W==Z && W==Y)) + C*(1-(W==Z && W==Y));

	mov	edx, DWORD PTR _W$5[ebp]
	cmp	edx, DWORD PTR _Z$2[ebp]
	jne	SHORT $LN19@img_scale_
	mov	eax, DWORD PTR _W$5[ebp]
	cmp	eax, DWORD PTR _Y$3[ebp]
	jne	SHORT $LN19@img_scale_
	mov	DWORD PTR tv204[ebp], 1
	jmp	SHORT $LN20@img_scale_
$LN19@img_scale_:
	mov	DWORD PTR tv204[ebp], 0
$LN20@img_scale_:
	mov	ecx, DWORD PTR _W$5[ebp]
	cmp	ecx, DWORD PTR _Z$2[ebp]
	jne	SHORT $LN21@img_scale_
	mov	edx, DWORD PTR _W$5[ebp]
	cmp	edx, DWORD PTR _Y$3[ebp]
	jne	SHORT $LN21@img_scale_
	mov	DWORD PTR tv208[ebp], 1
	jmp	SHORT $LN22@img_scale_
$LN21@img_scale_:
	mov	DWORD PTR tv208[ebp], 0
$LN22@img_scale_:
	mov	eax, DWORD PTR _Z$2[ebp]
	imul	eax, DWORD PTR tv204[ebp]
	mov	ecx, 1
	sub	ecx, DWORD PTR tv208[ebp]
	imul	ecx, DWORD PTR _C$6[ebp]
	add	eax, ecx
	mov	edx, DWORD PTR _y$13[ebp]
	lea	ecx, DWORD PTR [edx*4+2]
	imul	ecx, DWORD PTR _in_width$[ebp]
	mov	edx, DWORD PTR _x$11[ebp]
	lea	ecx, DWORD PTR [ecx+edx*2+1]
	mov	edx, DWORD PTR _output$[ebp]
	mov	DWORD PTR [edx+ecx*4], eax

; 74   :         
; 75   :       }

	jmp	$LN2@img_scale_
$LN1@img_scale_:

; 76   :     }

	jmp	$LN5@img_scale_
$LN7@img_scale_:

; 77   :   };

	pop	edi
	mov	esp, ebp
	pop	ebp
	ret	0
?img_scale_eagle@@YAXPAIHH0@Z ENDP			; img_scale_eagle
_TEXT	ENDS
END
