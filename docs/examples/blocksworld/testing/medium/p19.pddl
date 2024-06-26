;; blocks=104, out_folder=testing/medium, instance_id=19, seed=1025

(define (problem blocksworld-19)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 b91 b92 b93 b94 b95 b96 b97 b98 b99 b100 b101 b102 b103 b104 - object)
 (:init 
    (arm-empty)
    (clear b19)
    (on b19 b10)
    (on-table b10)
    (clear b4)
    (on b4 b60)
    (on b60 b54)
    (on b54 b91)
    (on b91 b22)
    (on b22 b83)
    (on-table b83)
    (clear b28)
    (on b28 b89)
    (on b89 b95)
    (on b95 b93)
    (on b93 b25)
    (on b25 b29)
    (on b29 b98)
    (on b98 b69)
    (on b69 b78)
    (on b78 b99)
    (on b99 b40)
    (on b40 b7)
    (on b7 b11)
    (on b11 b31)
    (on b31 b100)
    (on b100 b75)
    (on-table b75)
    (clear b34)
    (on b34 b36)
    (on b36 b64)
    (on b64 b68)
    (on b68 b66)
    (on b66 b65)
    (on b65 b59)
    (on b59 b88)
    (on b88 b102)
    (on b102 b51)
    (on b51 b94)
    (on b94 b85)
    (on b85 b14)
    (on b14 b33)
    (on b33 b92)
    (on b92 b12)
    (on b12 b2)
    (on-table b2)
    (clear b37)
    (on-table b37)
    (clear b49)
    (on b49 b79)
    (on b79 b52)
    (on b52 b17)
    (on b17 b81)
    (on b81 b20)
    (on b20 b21)
    (on b21 b61)
    (on b61 b48)
    (on b48 b80)
    (on b80 b47)
    (on b47 b58)
    (on b58 b77)
    (on b77 b56)
    (on b56 b24)
    (on b24 b55)
    (on-table b55)
    (clear b63)
    (on b63 b3)
    (on-table b3)
    (clear b57)
    (on b57 b90)
    (on b90 b26)
    (on b26 b71)
    (on b71 b74)
    (on b74 b43)
    (on b43 b96)
    (on b96 b70)
    (on b70 b53)
    (on b53 b42)
    (on b42 b23)
    (on b23 b86)
    (on b86 b103)
    (on-table b103)
    (clear b5)
    (on-table b5)
    (clear b35)
    (on b35 b76)
    (on b76 b1)
    (on-table b1)
    (clear b67)
    (on b67 b15)
    (on b15 b16)
    (on b16 b39)
    (on b39 b73)
    (on b73 b101)
    (on-table b101)
    (clear b84)
    (on b84 b72)
    (on b72 b44)
    (on b44 b13)
    (on-table b13)
    (clear b97)
    (on b97 b9)
    (on b9 b38)
    (on-table b38)
    (clear b18)
    (on b18 b46)
    (on b46 b62)
    (on b62 b41)
    (on b41 b50)
    (on b50 b32)
    (on b32 b87)
    (on b87 b82)
    (on b82 b30)
    (on b30 b45)
    (on b45 b104)
    (on b104 b6)
    (on b6 b8)
    (on b8 b27)
    (on-table b27))
 (:goal  (and 
    (clear b41)
    (on b41 b31)
    (on b31 b19)
    (on b19 b92)
    (on b92 b65)
    (on-table b65)
    (clear b87)
    (on b87 b76)
    (on-table b76)
    (clear b2)
    (on b2 b99)
    (on b99 b81)
    (on b81 b100)
    (on b100 b70)
    (on b70 b88)
    (on b88 b5)
    (on b5 b10)
    (on b10 b11)
    (on b11 b85)
    (on-table b85)
    (clear b38)
    (on b38 b43)
    (on b43 b3)
    (on b3 b64)
    (on b64 b69)
    (on b69 b101)
    (on b101 b78)
    (on b78 b53)
    (on b53 b17)
    (on b17 b27)
    (on b27 b59)
    (on b59 b72)
    (on b72 b12)
    (on b12 b9)
    (on b9 b15)
    (on b15 b37)
    (on b37 b93)
    (on b93 b25)
    (on b25 b103)
    (on b103 b33)
    (on b33 b40)
    (on b40 b68)
    (on b68 b73)
    (on b73 b32)
    (on b32 b80)
    (on b80 b102)
    (on b102 b16)
    (on b16 b96)
    (on b96 b29)
    (on b29 b26)
    (on b26 b39)
    (on b39 b24)
    (on b24 b28)
    (on-table b28)
    (clear b7)
    (on b7 b8)
    (on b8 b14)
    (on-table b14)
    (clear b22)
    (on b22 b44)
    (on b44 b66)
    (on b66 b56)
    (on b56 b82)
    (on b82 b23)
    (on b23 b48)
    (on b48 b45)
    (on b45 b98)
    (on b98 b34)
    (on b34 b97)
    (on b97 b60)
    (on-table b60)
    (clear b54)
    (on b54 b50)
    (on b50 b51)
    (on b51 b83)
    (on b83 b30)
    (on b30 b47)
    (on b47 b91)
    (on b91 b95)
    (on b95 b6)
    (on b6 b86)
    (on b86 b58)
    (on b58 b18)
    (on b18 b35)
    (on b35 b104)
    (on b104 b55)
    (on b55 b77)
    (on b77 b71)
    (on-table b71)
    (clear b94)
    (on b94 b46)
    (on b46 b20)
    (on b20 b84)
    (on-table b84)
    (clear b1)
    (on b1 b52)
    (on b52 b79)
    (on b79 b67)
    (on b67 b61)
    (on b61 b13)
    (on b13 b62)
    (on b62 b36)
    (on b36 b74)
    (on b74 b89)
    (on b89 b57)
    (on b57 b42)
    (on b42 b90)
    (on-table b90)
    (clear b63)
    (on b63 b4)
    (on b4 b49)
    (on b49 b75)
    (on b75 b21)
    (on-table b21))))
