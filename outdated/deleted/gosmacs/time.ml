;;; Painfully written by Philippe Deschamp of INRIA	;;;
;;; around Wed Aug 13 16:30:11 1986			;;;

;;; Modified Tue Aug 19 1986 to speed up drastically	;;;
;;; updates: comments are considered hourly entities,	;;;
;;; and as such not updated within the hour; moreover,	;;;
;;; integer variables receive now only integer values!	;;;

;;; Modified Thu Apr 16 1987 to try taking into account ;;;
;;; the error of Emacs which does not know about local	;;;
;;; time zones...					;;;

; A simple clock for the mode line.

(declare-global
    &&time-is_on
    &&time-global_mode_string
    &&time-local_delta)

(if (! (is-bound Big-Ben))
    (progn (declare-global Big-Ben)
	   (setq Big-Ben 1)))

(defun
    (time
	 (if (! &&time-is_on)
	     (progn char
		    (setq &&time-global_mode_string global-mode-string)
		    (setq &&time-is_on 1)
		    (time-compute-local-delta "**time-scratch**")
		    (bind-to-key "change-time-&-abort" "\^G")
		    (bind-to-key "change-time-&-abort" "\e\^G")
		    (bind-to-key "change-time-&-abort" "\^X\^G")
		    (bind-to-key "change-time-&-abort" "\^Z\^G")
		    (setq char (- ' ' 1))
		    (while (<= (setq char (+ char 1)) '~')
			   (if (= (global-binding-of char) "self-insert")
			       (bind-to-key "change-time-&-self-insert" char)))))
	 (change-time)
	 (novalue))
    
    (time-compute-local-delta buf
	(setq buf (arg 1))
	(setq &&time-local_delta
	      (save-excursion
		  (temp-use-buffer buf)
		  (erase-buffer)
		  (set-mark)
		  (fast-filter-region "date")
		  (beginning-of-file)
		  (search-forward ":")
		  (backward-character)
		  (set-mark)
		  (backward-word)
		  (- (region-to-string buf)
		     (substr (current-time) 12 2))))
	(delete-buffer buf))
    
    (change-time now hour string
	(if (& &&time-is_on
	       (> (setq now (current-numeric-time)) &&time-is_on)
	       (!= (substr global-mode-string 1 5)
		   (progn
			 (setq string (substr (current-time) 12 5))
			 (setq hour (+ &&time-local_delta (substr string 1 2)))
			 (if (!= &&time-local_delta 0)
			     (setq string (concat
						 (if (< hour 10) "0" "")
						 hour
						 (substr string 3 3)))
			     string))))
	    (progn min
		   (setq &&time-is_on (+ now 30))
		   (if (= hour (substr global-mode-string 1 2))
		       (setq string (concat string (substr global-mode-string 6 -5)))
		       (< hour 9)
		       (setq string (concat string " (Breakfast?)"))
		       (= hour 11)
		       (setq string (concat string " (Work!)")); No kidding!
		       (= hour 12)
		       (setq string (concat string " (Lunch?)"))
		       (< hour 15)
		       (setq string (concat string " (Coffee?)"))
		       (< hour 18)
		       (setq string (concat string " (Work!)")); No kidding!
		       (= hour 18)
		       (setq string (concat string " (Home?)"))
		       (= hour 19)
		       (setq string (concat string " (Dinner?)"))
		       (>= hour 20)
		       (setq string (concat string " (Bed?)")))
		   (setq global-mode-string string)
		   (if (! Big-Ben)
		       (nothing)
		       (= (setq min (+ 0 (substr string 4 2))) 0)
		       (ring-bell (if (<= hour 12) hour (- hour 12)))
		       (= min 15)
		       (ring-bell 1)
		       (= min 30)
		       (ring-bell 2)
		       (= min 45)
		       (ring-bell 3)))))
    
    (ring-bell amount
	(setq amount 15)
	(while amount
	       (send-string-to-terminal "\^G")
	       (sit-for 1)
	       (setq amount (- amount 1)))
	(sit-for 10)
	(setq amount (arg 1))
	(while amount
	       (sit-for 10)
	       (send-string-to-terminal "\^G")
	       (setq amount (- amount 1))))
    
    (change-time-&-abort
	(if &&time-is_on
	    (change-time)
	    (bind-to-key "illegal-operation" (last-key-struck)))
	(illegal-operation))
    
    (change-time-&-self-insert
	(if &&time-is_on
	    (change-time)
	    (bind-to-key "self-insert" (last-key-struck)))
	(self-insert))
    
    (time-stop
	(setq global-mode-string &&time-global_mode_string)
	(setq &&time-is_on 0)
	(novalue)))
