;; blocks=239, out_folder=testing/hard, instance_id=8, seed=1014

(define (problem blocksworld-08)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 b91 b92 b93 b94 b95 b96 b97 b98 b99 b100 b101 b102 b103 b104 b105 b106 b107 b108 b109 b110 b111 b112 b113 b114 b115 b116 b117 b118 b119 b120 b121 b122 b123 b124 b125 b126 b127 b128 b129 b130 b131 b132 b133 b134 b135 b136 b137 b138 b139 b140 b141 b142 b143 b144 b145 b146 b147 b148 b149 b150 b151 b152 b153 b154 b155 b156 b157 b158 b159 b160 b161 b162 b163 b164 b165 b166 b167 b168 b169 b170 b171 b172 b173 b174 b175 b176 b177 b178 b179 b180 b181 b182 b183 b184 b185 b186 b187 b188 b189 b190 b191 b192 b193 b194 b195 b196 b197 b198 b199 b200 b201 b202 b203 b204 b205 b206 b207 b208 b209 b210 b211 b212 b213 b214 b215 b216 b217 b218 b219 b220 b221 b222 b223 b224 b225 b226 b227 b228 b229 b230 b231 b232 b233 b234 b235 b236 b237 b238 b239 - object)
 (:init 
    (arm-empty)
    (clear b136)
    (on b136 b178)
    (on b178 b54)
    (on b54 b80)
    (on b80 b164)
    (on b164 b78)
    (on b78 b182)
    (on b182 b149)
    (on b149 b67)
    (on b67 b214)
    (on b214 b116)
    (on-table b116)
    (clear b87)
    (on-table b87)
    (clear b222)
    (on b222 b94)
    (on b94 b207)
    (on b207 b189)
    (on b189 b197)
    (on b197 b40)
    (on b40 b70)
    (on b70 b96)
    (on b96 b61)
    (on b61 b236)
    (on b236 b233)
    (on b233 b202)
    (on b202 b47)
    (on b47 b68)
    (on b68 b115)
    (on b115 b112)
    (on-table b112)
    (clear b184)
    (on b184 b146)
    (on b146 b211)
    (on b211 b93)
    (on b93 b36)
    (on b36 b105)
    (on b105 b100)
    (on b100 b24)
    (on-table b24)
    (clear b128)
    (on b128 b176)
    (on b176 b179)
    (on b179 b200)
    (on b200 b79)
    (on b79 b219)
    (on b219 b21)
    (on b21 b58)
    (on b58 b192)
    (on b192 b22)
    (on b22 b84)
    (on b84 b142)
    (on b142 b196)
    (on b196 b225)
    (on b225 b14)
    (on b14 b51)
    (on b51 b210)
    (on b210 b97)
    (on b97 b127)
    (on b127 b33)
    (on b33 b60)
    (on b60 b98)
    (on b98 b129)
    (on b129 b162)
    (on b162 b73)
    (on b73 b44)
    (on b44 b12)
    (on b12 b220)
    (on b220 b130)
    (on b130 b125)
    (on b125 b153)
    (on b153 b81)
    (on b81 b104)
    (on b104 b41)
    (on b41 b103)
    (on-table b103)
    (clear b4)
    (on b4 b139)
    (on-table b139)
    (clear b10)
    (on b10 b82)
    (on b82 b209)
    (on b209 b111)
    (on b111 b1)
    (on-table b1)
    (clear b106)
    (on b106 b166)
    (on b166 b113)
    (on b113 b218)
    (on b218 b20)
    (on-table b20)
    (clear b212)
    (on b212 b154)
    (on b154 b221)
    (on b221 b231)
    (on-table b231)
    (clear b30)
    (on b30 b201)
    (on b201 b71)
    (on b71 b148)
    (on b148 b77)
    (on b77 b117)
    (on b117 b7)
    (on b7 b52)
    (on b52 b228)
    (on b228 b232)
    (on b232 b185)
    (on b185 b156)
    (on b156 b28)
    (on b28 b90)
    (on b90 b42)
    (on b42 b99)
    (on b99 b191)
    (on b191 b175)
    (on b175 b122)
    (on b122 b167)
    (on b167 b26)
    (on-table b26)
    (clear b224)
    (on-table b224)
    (clear b198)
    (on b198 b50)
    (on-table b50)
    (clear b31)
    (on b31 b83)
    (on b83 b158)
    (on b158 b75)
    (on b75 b183)
    (on b183 b138)
    (on b138 b155)
    (on b155 b170)
    (on b170 b49)
    (on b49 b171)
    (on b171 b227)
    (on b227 b213)
    (on b213 b172)
    (on-table b172)
    (clear b230)
    (on b230 b177)
    (on-table b177)
    (clear b8)
    (on b8 b35)
    (on b35 b216)
    (on b216 b141)
    (on b141 b124)
    (on b124 b215)
    (on b215 b76)
    (on b76 b11)
    (on b11 b223)
    (on b223 b147)
    (on b147 b173)
    (on-table b173)
    (clear b237)
    (on b237 b89)
    (on-table b89)
    (clear b181)
    (on b181 b108)
    (on b108 b43)
    (on b43 b165)
    (on b165 b91)
    (on b91 b206)
    (on b206 b229)
    (on b229 b5)
    (on b5 b143)
    (on b143 b88)
    (on b88 b3)
    (on b3 b69)
    (on-table b69)
    (clear b188)
    (on b188 b190)
    (on b190 b168)
    (on b168 b25)
    (on b25 b34)
    (on b34 b16)
    (on b16 b194)
    (on b194 b39)
    (on b39 b169)
    (on b169 b102)
    (on b102 b19)
    (on b19 b74)
    (on b74 b6)
    (on b6 b157)
    (on b157 b161)
    (on b161 b217)
    (on b217 b114)
    (on b114 b187)
    (on b187 b13)
    (on b13 b55)
    (on b55 b205)
    (on b205 b32)
    (on b32 b101)
    (on-table b101)
    (clear b123)
    (on b123 b18)
    (on b18 b17)
    (on b17 b57)
    (on b57 b134)
    (on b134 b137)
    (on b137 b53)
    (on b53 b29)
    (on b29 b118)
    (on b118 b59)
    (on b59 b150)
    (on-table b150)
    (clear b46)
    (on b46 b152)
    (on b152 b204)
    (on b204 b86)
    (on b86 b235)
    (on b235 b23)
    (on b23 b234)
    (on b234 b131)
    (on b131 b92)
    (on b92 b226)
    (on b226 b163)
    (on b163 b119)
    (on b119 b72)
    (on b72 b195)
    (on b195 b15)
    (on b15 b203)
    (on b203 b48)
    (on b48 b63)
    (on b63 b140)
    (on b140 b199)
    (on b199 b135)
    (on b135 b85)
    (on b85 b144)
    (on b144 b95)
    (on b95 b37)
    (on b37 b133)
    (on b133 b64)
    (on b64 b38)
    (on b38 b186)
    (on b186 b145)
    (on b145 b193)
    (on b193 b56)
    (on b56 b238)
    (on b238 b27)
    (on b27 b121)
    (on-table b121)
    (clear b151)
    (on b151 b107)
    (on b107 b180)
    (on b180 b208)
    (on-table b208)
    (clear b65)
    (on b65 b126)
    (on b126 b239)
    (on b239 b120)
    (on b120 b45)
    (on b45 b160)
    (on b160 b159)
    (on b159 b174)
    (on-table b174)
    (clear b110)
    (on b110 b109)
    (on b109 b9)
    (on-table b9)
    (clear b66)
    (on b66 b62)
    (on-table b62)
    (clear b2)
    (on b2 b132)
    (on-table b132))
 (:goal  (and 
    (clear b202)
    (on b202 b198)
    (on b198 b21)
    (on b21 b78)
    (on b78 b235)
    (on b235 b145)
    (on b145 b33)
    (on b33 b84)
    (on b84 b63)
    (on b63 b10)
    (on b10 b234)
    (on b234 b91)
    (on b91 b172)
    (on b172 b139)
    (on b139 b17)
    (on b17 b156)
    (on b156 b147)
    (on b147 b25)
    (on-table b25)
    (clear b146)
    (on b146 b20)
    (on b20 b216)
    (on b216 b110)
    (on b110 b119)
    (on-table b119)
    (clear b125)
    (on b125 b152)
    (on-table b152)
    (clear b39)
    (on b39 b58)
    (on b58 b140)
    (on b140 b8)
    (on b8 b148)
    (on b148 b3)
    (on b3 b205)
    (on b205 b59)
    (on b59 b53)
    (on b53 b228)
    (on b228 b40)
    (on b40 b193)
    (on b193 b47)
    (on b47 b182)
    (on b182 b189)
    (on-table b189)
    (clear b23)
    (on b23 b116)
    (on b116 b54)
    (on b54 b207)
    (on b207 b191)
    (on-table b191)
    (clear b12)
    (on b12 b41)
    (on b41 b128)
    (on b128 b133)
    (on b133 b153)
    (on b153 b161)
    (on b161 b9)
    (on b9 b55)
    (on b55 b124)
    (on b124 b56)
    (on b56 b220)
    (on b220 b114)
    (on b114 b57)
    (on b57 b36)
    (on b36 b30)
    (on b30 b79)
    (on b79 b200)
    (on b200 b174)
    (on b174 b31)
    (on b31 b213)
    (on b213 b169)
    (on b169 b185)
    (on b185 b180)
    (on b180 b226)
    (on b226 b233)
    (on b233 b217)
    (on b217 b158)
    (on b158 b162)
    (on b162 b129)
    (on b129 b5)
    (on b5 b192)
    (on b192 b131)
    (on b131 b2)
    (on b2 b80)
    (on b80 b121)
    (on b121 b173)
    (on b173 b120)
    (on b120 b151)
    (on-table b151)
    (clear b104)
    (on b104 b98)
    (on b98 b18)
    (on b18 b186)
    (on b186 b4)
    (on b4 b100)
    (on b100 b64)
    (on b64 b102)
    (on b102 b211)
    (on b211 b136)
    (on b136 b45)
    (on b45 b26)
    (on b26 b150)
    (on b150 b112)
    (on b112 b85)
    (on b85 b66)
    (on b66 b86)
    (on b86 b176)
    (on b176 b89)
    (on b89 b76)
    (on b76 b52)
    (on b52 b43)
    (on b43 b65)
    (on b65 b197)
    (on b197 b225)
    (on b225 b143)
    (on b143 b178)
    (on-table b178)
    (clear b224)
    (on b224 b50)
    (on b50 b72)
    (on b72 b177)
    (on b177 b126)
    (on b126 b68)
    (on b68 b42)
    (on b42 b118)
    (on b118 b93)
    (on b93 b165)
    (on b165 b99)
    (on b99 b49)
    (on b49 b32)
    (on b32 b87)
    (on b87 b48)
    (on b48 b209)
    (on b209 b62)
    (on b62 b77)
    (on b77 b16)
    (on b16 b144)
    (on b144 b75)
    (on b75 b208)
    (on-table b208)
    (clear b13)
    (on-table b13)
    (clear b117)
    (on b117 b187)
    (on b187 b11)
    (on-table b11)
    (clear b109)
    (on b109 b101)
    (on-table b101)
    (clear b44)
    (on b44 b130)
    (on b130 b206)
    (on b206 b111)
    (on b111 b179)
    (on-table b179)
    (clear b92)
    (on b92 b24)
    (on b24 b70)
    (on b70 b157)
    (on b157 b22)
    (on b22 b67)
    (on b67 b137)
    (on b137 b230)
    (on b230 b105)
    (on b105 b134)
    (on b134 b142)
    (on b142 b19)
    (on b19 b170)
    (on b170 b34)
    (on-table b34)
    (clear b88)
    (on b88 b94)
    (on b94 b184)
    (on b184 b229)
    (on b229 b6)
    (on b6 b199)
    (on-table b199)
    (clear b61)
    (on-table b61)
    (clear b95)
    (on b95 b7)
    (on b7 b163)
    (on b163 b28)
    (on b28 b113)
    (on-table b113)
    (clear b164)
    (on b164 b221)
    (on b221 b175)
    (on b175 b160)
    (on b160 b123)
    (on b123 b231)
    (on-table b231)
    (clear b122)
    (on b122 b222)
    (on b222 b82)
    (on b82 b1)
    (on b1 b37)
    (on b37 b223)
    (on b223 b212)
    (on-table b212)
    (clear b237)
    (on b237 b168)
    (on b168 b196)
    (on b196 b138)
    (on b138 b90)
    (on b90 b73)
    (on b73 b167)
    (on b167 b108)
    (on b108 b166)
    (on b166 b227)
    (on b227 b190)
    (on b190 b218)
    (on b218 b29)
    (on b29 b236)
    (on b236 b106)
    (on b106 b14)
    (on b14 b215)
    (on b215 b154)
    (on b154 b149)
    (on b149 b159)
    (on b159 b15)
    (on b15 b74)
    (on b74 b38)
    (on-table b38)
    (clear b132)
    (on b132 b194)
    (on b194 b71)
    (on b71 b103)
    (on b103 b155)
    (on b155 b204)
    (on-table b204)
    (clear b171)
    (on-table b171)
    (clear b27)
    (on b27 b238)
    (on b238 b96)
    (on b96 b69)
    (on b69 b81)
    (on b81 b83)
    (on b83 b135)
    (on b135 b195)
    (on b195 b127)
    (on b127 b181)
    (on-table b181)
    (clear b201)
    (on b201 b203)
    (on b203 b51)
    (on b51 b107)
    (on b107 b214)
    (on b214 b232)
    (on b232 b60)
    (on b60 b115)
    (on b115 b188)
    (on b188 b35)
    (on b35 b97)
    (on b97 b239)
    (on b239 b183)
    (on-table b183)
    (clear b219)
    (on b219 b141)
    (on b141 b46)
    (on-table b46)
    (clear b210)
    (on-table b210))))
