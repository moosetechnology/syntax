;;; Modified Fri Aug  8 14:16:06 1986		;;;
;;; by Philippe Deschamp (PhD) of INRIA		;;;
;;; to fix various problems (see code).		;;;

; electric lisp mode

; PhD: I need that hack because neither (current-column) nor (current-indent)
; work on my station!
(defun
    (cur-column
	(save-excursion col here
	    (setq here (dot))
	    (setq col 1)
	    (beginning-of-line)
	    (while (!= (dot) here)
		   (if (= (following-char) '\^I')
		       (setq col (+ tab-size 1 (* tab-size (/ (- col 1) tab-size))))
		       (setq col (+ 1 col)))
		   (forward-character))
	    col))

    (cur-indent
	(save-excursion
	    (beginning-of-line)
	    (while (| (= (following-char) ' ') (= (following-char) '\^I'))
		   (forward-character))
	    (cur-column)))
)

; This function gets bound to ')' -- it flashes to the corresponding
; '(' and fixes the indentation of the ')'.
(defun
    (paren-pause dot instabs
	(if (eolp) (delete-white-space))
	(setq instabs (bolp))
	(setq dot (dot))
	(insert-character ')')
	(save-excursion
	    (backward-paren)
	    (if instabs (save-excursion descol
			    (setq descol (cur-column))
			    (goto-character dot)
			    (to-col descol)))
	    (if (dot-is-visible)
		(sit-for 5)		; PhD: I cannot make it sit for any time.
	    )
	    (progn
		  (beginning-of-line)
		  (set-mark)
		  (end-of-line)
		  (message (region-to-string))
	    )
	)
    )
)

; This function gets bound to linefeed, it inserts a newline and
; properly indents the next line.
(defun
    (nl-indent column
	(delete-white-space)			; PhD
	(save-excursion
	    (backward-balanced-paren-line)
	    (setq column
		  (if (bolp)
		      (cur-indent)
		      (progn lim here
			     (setq lim (+ (cur-column) 8))
			     (error-occurred (re-search-forward "\\=([ \t]*\\w*[ \t]*")); PhD: added "dot".
			     (if (> (setq here (cur-column)) lim)
				 (- lim 4)
				 here))))
	    (if (< column 5)
		(setq column 5)))
	(newline)
	(to-col column)
    )
)

; This function repairs the indentation of the current line.
(defun
    (re-indent-line
	(save-excursion column
	    (end-of-line)			; PhD
	    (delete-white-space)		; PhD
	    (beginning-of-line)
	    (delete-white-space)
	    (save-excursion
		(if (= (following-char) ')')
		    (progn
			  (forward-character)
			  (backward-paren)
			  (setq column (cur-column))
		    )
		    (| (looking-at "else") (looking-at "then"))
		    (progn
			  (insert-character ')')
			  (set-mark)
			  (backward-paren)
			  (setq column (+ (cur-column) 1))
			  (exchange-dot-and-mark)
			  (delete-previous-character)
		    )
		    (progn
			  (backward-character)
			  (backward-balanced-paren-line)
			  (setq column
				(if (bolp)
				    (if (| (looking-at "[ \t]*else")
					   (looking-at "[ \t]*then"))
					(+ (cur-indent) 3)
					(cur-indent)
				    )
				    (progn lim here
					   (setq lim (+ (cur-column) 8))
					   (error-occurred (re-search-forward "\\=([ \t]*\\w*[ \t]*")); PhD: added "dot".
					   (if (> (setq here (cur-column)) lim)
					       (- lim 4)
					       here))))
			  (if (< column 5)
			      (setq column 5))
		    )
		)
	    )
	    (to-col column)
	)
	(if (bolp) (end-of-line)); PhD
    )
)

; this function fixes up the indentation of
; an entire lisp function: ^( to )
(defun
    (indent-lisp-function
	(save-excursion
	    (if (error-occurred (end-of-line) (re-search-reverse "^("))
		(error-message "Can't find function"))
	    (set-mark)
	    (forward-paren)
	    (exchange-dot-and-mark)
	    (narrow-region)
	    (error-occurred (re-replace-string "elseif\n[ \t]*" "elseif "))
	    (error-occurred (re-replace-string "\n[\t ]*then" " then"))
	    (widen-region)
	    (end-of-line)	; PhD
	    (delete-white-space)
	    (beginning-of-line)
	    (next-line)
	    (while (& (! (eobp)) (<= (dot) (mark)))
		   (re-indent-line)
		   (if (eolp) (delete-white-space)); PhD
		   (next-line)
	    )
	)
	(message "Done!")
    )
)

(defun
    (semi-electric-lisp
	(delete-white-space)			; PhD
	(if (eolp)
	    (progn (if (! (bolp)) (to-col comment-column))
		   ;(setq left-margin comment-column)
		   ;(setq right-margin 77)
		   ;(setq prefix-string "; ")
		   (insert-string "; "))
	    (insert-character ';'))
    )
)

(declare-buffer-specific shell-prompt-template)

(defun    
    (electric-lisp-mode		; electric-lisp mode initialization
	(setq shell-prompt-template "[0-9][0-9]*\. *")
	(local-bind-to-key "semi-electric-lisp" ";")
	(local-bind-to-key "paren-pause" ")")
	(local-bind-to-key "forward-paren" "\e)")
	(local-bind-to-key "backward-paren" "\e(")
	(local-bind-to-key "indent-lisp-function" "\ej")
	(local-bind-to-key "nl-indent" 10)
	(local-bind-to-key "re-indent-line" "\ei")
	(local-bind-to-key "zap-defun" "\^X\^L")
	(setq mode-string "lisp")
	(use-abbrev-table "lisp")
	(use-syntax-table "lisp")
	(error-occurred (electric-lisp-mode-hook))
	(novalue)
    )

    (ml-paren
	(insert-character '(')
	(insert-string (get-tty-command "  ("))
	(insert-character ' ')
	(novalue))

    (electric-mlisp-mode	; electric-mlisp mode initialization
	(local-bind-to-key "ml-paren" "(")
	(local-bind-to-key "expand-mlisp-variable" "\^\")
	(local-bind-to-key "semi-electric-lisp" ";")
	(local-bind-to-key "paren-pause" ")")
	(local-bind-to-key "forward-paren" "\e)")
	(local-bind-to-key "backward-paren" "\e(")
	(local-bind-to-key "indent-lisp-function" "\ej")
	(local-bind-to-key "nl-indent" 10)
	(local-bind-to-key "re-indent-line" "\ei")
	(setq mode-string "mlisp")
	(use-abbrev-table "mlisp")
	(use-syntax-table "mlisp")
	(error-occurred (electric-mlisp-mode-hook))
	(novalue)
)

    (zap-defun			; take the current "(defXX" to ")" region and
				; stuff it as input to the "lisp" process
				; then whip into the lisp buffer to interact.
	(save-excursion
	    (end-of-line)
	    (search-reverse "(def")
	    (set-mark)
	    (forward-paren)
	    (end-of-line)
	    (forward-character)
	    (region-to-process "lisp")
	)
	(pop-to-buffer "lisp")
	(end-of-file)
    )
)
(use-syntax-table "lisp")
(modify-syntax-entry "()   (")
(modify-syntax-entry ")(   )")
(modify-syntax-entry "\"    |")
(modify-syntax-entry "\"    \"")
(modify-syntax-entry "\\    /")
(modify-syntax-entry "w    -+!$%^&=_~:?*<>")

(use-syntax-table "mlisp")
(modify-syntax-entry "()   (")
(modify-syntax-entry ")(   )")
(modify-syntax-entry "\"    '")
(modify-syntax-entry "\"    \"")
(modify-syntax-entry "\\    \\")
(modify-syntax-entry "w    -+!$%^&=_~:/?*<>|a-zA-Z0-9")
(novalue)
