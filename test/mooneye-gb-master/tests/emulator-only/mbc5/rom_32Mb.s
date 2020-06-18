; Copyright (C) 2014-2020 Joonas Javanainen <joonas.javanainen@gmail.com>
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.

; Tests banking behaviour of a MBC5 cart with a 32 Mbit ROM

; See gb-ctr for details: https://github.com/Gekkio/gb-ctr

; Results have been verified using a flash cartridge with a genuine MBC5 chip
; and support for configuring ROM/RAM sizes.

.define CART_ROM_BANKS 256

.include "harness/mbc5_rom.s"

.bank 0 slot 0
.section "expected banks" FREE

expected_banks:
; $4000-$7FFF area
.dw   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
.dw  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
.dw  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47
.dw  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63
.dw  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79
.dw  80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95
.dw  96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111
.dw 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127
.dw 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
.dw 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159
.dw 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175
.dw 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191
.dw 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207
.dw 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223
.dw 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
.dw 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255
.dw   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
.dw  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
.dw  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47
.dw  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63
.dw  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79
.dw  80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95
.dw  96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111
.dw 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127
.dw 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
.dw 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159
.dw 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175
.dw 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191
.dw 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207
.dw 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223
.dw 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
.dw 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255

.ends
