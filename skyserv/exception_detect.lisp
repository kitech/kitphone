
;; (constant 'SIGINT 2)
;; (signal SIGINT "default")

;; (set 'haha "45678")

;; (set 'sip_servs (list
;;                  "server1"
;;                  "server2"
;;                  ))
;; (define (get_sip_servers) (first sip_servs))

;; (get_sip_servers)

;(define (area x y) (* x y))
;(new Tree 'state_tree)

(define states (list))
(define (on_skype_status handler_name status_line)
    (println status_line handler_name
             )

  ; (setf states (append states (list handler_name)))
  (if-not (member (list handler_name) states)
      (begin 
       (setf states (append states (list handler_name)))
       (setf states (append states (list (list))))
       (println "lllllll")
       )
      (begin (println "vvvvv"))
      )

  (println states)

  (catch
      (for (x 0 (- (length states) 1) 2)
           (begin
            ;(println x  (nth x states))
            (if (= (nth x states) handler_name)
                (begin 
                 ;(println "found it")
                 (throw (nth (+ x 1) states))))
            )
           )
    'elem
    )

  (println elem)

  (setf fields (parse status_line))
  (print fields)
  (if (= (nth 0 fields) "USER")
      (begin
       (println "user found")
       
       )
      )
)