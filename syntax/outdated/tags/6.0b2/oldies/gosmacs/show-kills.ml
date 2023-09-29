;;; Package to see exactly what you are going to yank.
;;; Works only if you are using the kill-ring package.
;;; Written Mon Aug 18 18:48:14 1986
;;; by Philippe Deschamp of INRIA.
;;; And he knows this is dirty!!!

(defun
    (unkill-smart		; bound to ^Y
	(if prefix-argument-provided
	    (progn (setq prefix-argument (- prefix-argument 1))
		   (prefix-argument-loop (previous-killbuffer))))
	(unkill)
	(if prefix-argument-provided
	    (prefix-argument-loop (next-killbuffer))))


    (show-kills			; I bind it to ^Z^Y
	(save-excursion number
	    (pop-to-buffer "Help")
	    (erase-buffer)
	    (setq wrap-long-lines 0)
	    (insert-string "Use ESC-(number) ^Y to yank text:\n\n")
	    (setq number 1)
	    (while (<= number nrings)
		   (insert-string (substr (concat " " number) -2 2) ": ")
		   (save-excursion (unkill))
		   (error-occurred
		       (replace-string "\n" "\\012"))
		   (end-of-line)
		   (newline)
		   (previous-killbuffer)
		   (setq number (+ 1 number)))
	    (setq buffer-is-modified 0)
	    (beginning-of-file)))
)

(bind-to-key "unkill-smart"	"\^Y")
