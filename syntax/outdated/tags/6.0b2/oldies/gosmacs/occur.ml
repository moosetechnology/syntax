; This was last modified just before Thu Aug  7 15:06:17 1986
; by Philippe Deschamp of INRIA to clean up the code (no more
; "region-to-string"!) and correct a spelling mistake...
;
; this code was modified on or about Thu Apr  9 06:36:37 1981
; by Doug Philips to add the &Occurances-Extra-Lines variable.
;
; this code was written on or about Mon Feb  2 06:11:03 1981
; by Doug Philips to imitate the $XOccurances command in Twenex Emacs
; Unfortunately, this emacs doesn't allow one to scrible on the screen, and
; then have emacs restore the screen, which would be the way to go with
; this command.
;
; What the global variable is used for:
;
; &Occurances-Extra-Lines is a global variable that controls how many extra
; surrounding lines are printed in addition to the line containing the
; string found.  If this variable is 0 then NO additional lines are printed.
; If this variable is greater than 0 then it will print that many lines
; above and below the line on which the string was found.  When printing
; more than one line per match in this fashion, it will also print a
; seperator of '----------------' so you can tell where the different
; matches begin and end.  At the end of the buffer it prints
; '<<<End of Occur>>>'.

(if (! (is-bound &Occurances-Extra-Lines))
    (setq-default &Occurances-Extra-Lines 0))

(defun
    (Occurances occ-string c here o-count temp-pos
	(setq temp-pos (setq here (dot)))
	(setq o-count 0)
	(if (> 0 &Occurances-Extra-Lines)
	    (setq &Occurances-Extra-Lines 0))
	(setq occ-string (arg 1 "Search for all occurrences of: "))
	(switch-to-buffer "Occur")
	(setq needs-checkpointing 0)
	(erase-buffer)
	(switch-to-buffer temp-pos)
	(while (! (error-occurred
		      (goto-character temp-pos)
		      (search-forward occ-string)
		      (setq o-count (+ 1 o-count))))
	       (beginning-of-line)
	       (setq temp-pos (dot))
	       (provide-prefix-argument &Occurances-Extra-Lines (previous-line))
	       (set-mark)
	       (goto-character temp-pos)
	       (error-occurred (end-of-line) (forward-character))
	       (setq temp-pos (dot))
	       (if (< 0 &Occurances-Extra-Lines)
		   (progn
			 (switch-to-buffer "Occur")
			 (insert-string "----------------\n")
			 (switch-to-buffer temp-pos)
			 (provide-prefix-argument &Occurances-Extra-Lines (next-line))))
	       (append-region-to-buffer "Occur"))
	(switch-to-buffer "Occur")
	(insert-string "<<<End of Occur>>>\n")
	;	(setq user-mode-line 1)
	(setq mode-line-format
	      (concat "Found " o-count " occurrences of \"" occ-string
		      "\" after position " (+ 0 here) " in " here))
	(beginning-of-file)
	(while (! (eobp))
	       (message "--More?-- (' '=yes, ^L, ^G, anything else is executed)")
	       (setq c (get-tty-character))	 ; grab a character
	       (if (= c ' ')
		   (error-occurred
		       (end-of-window)
		       (forward-character)
		       (next-page))
		   (= c '\^L')
		   (redraw-display)
		   (progn (end-of-file)
			  (!= c '\^G'))
		   (push-back-character c)))
	(switch-to-buffer here)
	(goto-character here)
	o-count))
