;; blocks=9, out_folder=testing/easy, instance_id=6, seed=1012

(define (problem blocksworld-06)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 - object)
 (:init 
    (arm-empty)
    (clear b9)
    (on b9 b8)
    (on b8 b5)
    (on b5 b1)
    (on b1 b7)
    (on b7 b4)
    (on b4 b2)
    (on b2 b3)
    (on-table b3)
    (clear b6)
    (on-table b6))
 (:goal  (and 
    (clear b9)
    (on b9 b3)
    (on b3 b6)
    (on b6 b2)
    (on b2 b5)
    (on b5 b4)
    (on b4 b1)
    (on b1 b8)
    (on b8 b7)
    (on-table b7))))
