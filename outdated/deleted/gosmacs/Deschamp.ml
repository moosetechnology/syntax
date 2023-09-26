;;; Profile for making this Emacs look like a Multics Emacs.
;;; Painfully written by Philippe Deschamp of INRIA
;;; from Fri Aug  8 1986 to Wed Aug 20 1986.

; PhD: I need that hack because neither (current-column) nor (current-indent)
; work on my SUN station!
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
	    (skip-line-indentation)
	    (cur-column)))
)


(save-excursion
    (switch-to-buffer "PhD")
    (setq mode-line-format
	  (concat "Welcome to "
		  (emacs-version)
		  " profiled by Philippe Deschamp of INRIA."))
    (message mode-line-format)
    (sit-for 0))

(progn
    (autoload "add-change-log-entry"		"add-log")
    (autoload "aton"				"aton")
    (autoload "auto-arg"			"auto-arg")
    (autoload "buffer-edit"			"buffer-edit")
    (autoload "c-mode"				"c-mode")
    (autoload "centre-line"			"centre-line")
    (autoload "compute-tag-table"		"tags")
    (autoload "Current-line-number"		"linenum")
    (autoload "dired"				"dired")
    (autoload "elec-c-mode"			"elec-c")
    (autoload "electric-mlisp-mode"		"electric-lisp")
    (autoload "electric-lisp-mode"		"electric-lisp")
    (autoload "gen-off"				"generate")
    (autoload "gen-on"				"generate")
    (autoload "goto-line"			"goto")
    (autoload "goto-tag"			"tags")
    (autoload "goto-word-as-tag"		"tags")
    (autoload "incremental-search"		"incr-search")
    (autoload "justify-paragraph"		"justify-paragraph")
    (autoload "new-undo"			"undo")
    (autoload "Occurances"			"occur")
    (autoload "rcp-visit-file"			"rcp-visit")
    (autoload "read-only"			"readonly")
    (autoload "region-to-variable"		"variables")
    (autoload "region-to-variable"		"variables")
    (autoload "reverse-incremental-search"	"incr-search")
    (autoload "search-words-forward"		"word-search")
    (autoload "search-words-backward"		"word-search")
    (autoload "Search-Words-Forward"		"word-search")
    (autoload "Search-Words-Backward"		"word-search")
    (autoload "scribe-mode"			"scribe")
    (autoload "Show-current-line-number"	"linenum")
    (autoload "show-kills"			"show-kills")
    (autoload "spell"				"spell")
    (autoload "srccom"				"srccom")
    (autoload "time"				"time")
    (autoload "transpose-line"			"transp")
    (autoload "transpose-word"			"transp")
    (autoload "variable-to-region"		"variables")
    (autoload "vi"				"vi")
    (autoload "write-region-to-file"		"writeregion")

    (auto-execute "elec-c-mode"		"*.c")
    (auto-execute "elec-c-mode"		"*.h")
    (auto-execute "electric-lisp-mode"	"*.ll")
    (auto-execute "electric-mlisp-mode"	"*.ml")
    (auto-execute "electric-mlisp-mode"	"*.emacs_pro")
    (auto-execute "scribe-mode"		"*.mss")
    (auto-execute "text-mode"		"*.n"))

(defun
    (ask-before-quit		; may be bound to ^X^C
	(save-window-excursion
	    (if (| (> (recursion-depth) 0)
		   (get-tty-yes/no (concat "Are you sure you want to leave " (emacs-version))))
		(exit-emacs)
		(error-message "I was sure that was a typing mistake!"))))

    (copy-lines-smart		; esc-k
	(if prefix-argument-provided
	    (progn magic
		   (setq magic &kill-lines-magic)
		   (setq &kill-lines-magic 1)
		   (provide-prefix-argument prefix-argument (copy-lines))
		   (setq &kill-lines-magic magic))
	    (copy-lines)))

    (date
	 (set-mark)
	 (insert-string (current-time))
	 (if prefix-argument-provided
	     (save-excursion
		 (backward-word)
		 (exchange-dot-and-mark)
		 (provide-prefix-argument 3 (forward-word))
		 (forward-character)
		 (erase-region))))

    (delete-blank-lines	; ^X^O
	(goto-character
	    (save-excursion	; saves the mark
		one-must-stay eobp
		(if (setq one-must-stay (line-is-blank))
		    (progn (skip-white-space-backward)
			   (end-of-line)
			   (if (! (bolp)) (forward-character)))
		    (progn (end-of-line)
			   (delete-white-space)
			   (newline)))
		(set-mark)
		(skip-white-space-forward)
		(setq eobp (eobp))
		(if (! eobp)
		    (beginning-of-line))
		(erase-region)
		(if eobp
		    (if (! one-must-stay)
			(delete-previous-character))
		    one-must-stay
		    (newline))
		(dot))))

    (describe-smart		; esc-?
	(if prefix-argument-provided
	    (push-back-string "\exdescribe-key ")
	    (progn command
		   (setq command (get-key-binding))
		   (push-back-character ' ')
		   (push-back-string command)
		   (describe-command))))

    (exchange-dot-and-mark-smart; ^X^X
	(if prefix-argument-provided
	    (progn mark-name
		   (setq mark-name (concat "^@-" prefix-argument))
		   (goto-character (execute-mlisp-line mark-name)))
	    (exchange-dot-and-mark)))

    (execute-ESCaped-char com	; ^C
	(if (> 0 (setq com (previous-command)))
	    (error-message (char-to-string (last-key-struck)) " cannot ESCape " com))
	(push-back-character com)
	(push-back-character '\e')
	(if prefix-argument-provided
	    (progn (push-back-string prefix-argument)
		   (push-back-character '\e')))
	(if (interactive)
	    (progn string
		   (setq string (get-key-binding (concat "\e" (char-to-string com))))
		   (if prefix-argument-provided
		       (message ": ESC " prefix-argument " " string)
		       (message ": " string))
		   (sit-for 0))))

    (get-key-binding key	; ^X?
	(if (& (! (interactive))
	       (nargs))
	    (setq key (arg 1))
	    (progn prompt
		   (setq prompt "Please enter a key: ")
		   (message prompt)
		   (sit-for 0)
		   (if (= (setq key (get-tty-character)) '\e')
		       (setq prompt (concat prompt "ESC-"))
		       (= key '\^X')
		       (setq prompt (concat prompt "^X-"))
		       (= key '\^Z')
		       (setq prompt (concat prompt "^Z-"))
		       (setq prompt ""))
		   (setq key (char-to-string key))
		   (if (!= "" prompt)
		       (progn (message prompt)
			      (sit-for 0)
			      (setq key (concat key (char-to-string (get-tty-character))))))))
	(setq key (if (= "nothing" (local-binding-of key))
		      (global-binding-of key)
		      (local-binding-of key)))
	(if (interactive)
	    (insert-string "(" key))
	key)

    (get-tty-yes/no prompt answer default
	(setq default 0)
	(setq prompt (if (> (nargs) 0)
			 (progn (if (> (nargs) 1)
				    (setq default (arg 2)))
				(arg 1))
			 "Yes or No"))
	(setq prompt (concat prompt " [" (if default "Yes" "No") "] ? "))
	(setq answer -1)
	(while (> 0 answer)
	       (setq answer (get-tty-string prompt))
	       (setq answer (if (= answer "")
				default
				(| (= answer "y")
				   (= answer "Y")
				   (= answer "yes")
				   (= answer "Yes")
				   (= answer "YES"))
				1
				(| (= answer "n")
				   (= answer "N")
				   (= answer "no")
				   (= answer "No")
				   (= answer "NO"))
				0
				(progn
				      (save-excursion
					  (pop-to-buffer "Help")
					  (erase-buffer)
					  (insert-string "Please answer Yes or No"))
				      (send-string-to-terminal "\^G")
				      -1))))
	answer)

    (help
	 (info))

    (kill-lines-smart		; ^K
	(if prefix-argument-provided
	    (progn magic
		   (setq magic &kill-lines-magic)
		   (setq &kill-lines-magic 1)
		   (provide-prefix-argument prefix-argument (kill-lines))
		   (setq &kill-lines-magic magic))
	    (kill-lines)))

    (kill-next-window		; ^X-8
	(next-window)
	(delete-window))

    (kill-prev-window		; ^X-9
	(previous-window)
	(delete-window)
	(next-window))

    (kill-to-beginning-of-line	; @
	(save-excursion
	    (set-mark)
	    (beginning-of-line)
	    (kill-region)))

    (line-count here num rest size; ^X-=
	(setq here (+ 0 (dot))) (setq num 1) (setq rest 0)
	(beginning-of-line)
	(while (! (bobp))
	       (previous-line)
	       (setq num (+ 1 num)))
	(goto-character here) (beginning-of-line)
	(while (! (eobp))
	       (next-line)
	       (setq rest (+ 1 rest)))
	(if (!= (preceding-char) '\^J')
	    (setq rest (- rest 1)))
	(setq size (- (dot) 1))
	(goto-character here)
	(message "line " num "/" (+ num rest)
	    "  col " (current-column)
	    "  char " here "/" size))
    
    (line-is-blank
	(save-excursion
	    (skip-line-indentation)
	    (eolp)))

    (merge-lines		; ESC-^
	(if prefix-argument-provided
	    (end-of-line)
	    (progn (beginning-of-line)
		   (backward-character)))
	(forward-character)
	(delete-white-space)
	(delete-previous-character))

    (move-to-screen-edge	; ESC-,
	(beginning-of-window)
	(if prefix-argument-provided
	    (progn pos
		   (if (= (setq pos prefix-argument) 0)
		       (setq pos (/ (+ (window-height) 1) 2))
		       (& (> 0 pos)
			  (>= 0 (setq pos (+ pos (window-height) 1))))
		       (setq pos (window-height))
		       (> pos (window-height))
		       (setq pos (window-height)))
		   (if wrap-long-lines
		       (progn ww
			      (setq ww (window-width))
			      (while (> (setq pos (- pos 1)) 0)
				     (next-line)
				     (end-of-line)
				     (if (> (current-column) ww))
				     (setq pos (- pos (/ (current-column) ww))))
			      (if (> 0 pos)
				  (previous-line)))
		       (provide-prefix-argument (- pos 1) (next-line)))
		   (beginning-of-line))))
    
    (new-indented-line		; ESC-CR
	(delete-white-space)
	(push-back-character '\^J'))

    (next-half-page		; ^X-^N
	(prefix-argument-loop
	    (provide-prefix-argument 0 (move-to-screen-edge))
	    (line-to-top-of-window))
	(provide-prefix-argument 0 (move-to-screen-edge)))
    
    (open-line			; ESC-^O
	(save-excursion col
	    (setq col (cur-column))
	    (newline)
	    (delete-white-space)
	    (to-col col)))

    (prev-half-page		; ^X-^P
	(prefix-argument-loop 
	    (beginning-of-window)
	    (provide-prefix-argument 0 (redisplay-command))
	    (sit-for 0)))
    
    (query-repeat function continue char
	(setq function (if (< 0 (nargs))
			   (arg 1)
			   (concat "(" (get-tty-command ": query-repeat ") ")")))
	(setq continue 1)
	(while continue
	       (message ": query-repeat " function ": SPACE,continue; CR,stop; ^G,abort")
	       (if (= (setq char (get-tty-character)) ' ')
		   (execute-mlisp-line function)
		   (setq continue (& (!= char 13) (!= char 7)))
		   (send-string-to-terminal "\^G")))
	(if (= char 7)
	    (error-message "Aborted.")))

    (redisplay-command pos	; ^L
	(if (! prefix-argument-provided)
	    (redraw-display)
	    (save-excursion
		(if (= (setq pos prefix-argument) 0)
		    (setq pos (/ (window-height) 2))
		    (& (> 0 pos)
		       (>= 0 (setq pos (+ pos (window-height) 1))))
		    (setq pos (window-height))
		    (> pos (window-height))
		    (setq pos (window-height)))
		(if wrap-long-lines
		    (progn ww
			   (setq ww (window-width))
			   (while (> (setq pos (- pos 1)) 0)
				  (previous-line)
				  (end-of-line)
				  (if (> (current-column) ww))
				  (setq pos (- pos (/ (current-column) ww))))
			   (if (> 0 pos)
			       (next-line)))
		    (provide-prefix-argument (- pos 1) (previous-line)))
		(line-to-top-of-window))))
    
    (roll-down			; ^Z^N
	(query-repeat "(next-half-page)"))

    (roll-up			; ^Z^P
	(query-repeat "(prev-half-page)"))

    (save-filename string	; ^Z^F
	(setq string (if prefix-argument-provided (current-buffer-name) (current-file-name)))
	(save-excursion
	    (switch-to-buffer "Save Filename")
	    (setq needs-checkpointing 0)
	    (erase-buffer)
	    (insert-string string)
	    (beginning-of-line)
	    (execute-string "\^K")
	    (if (interactive)
		(message "Use ^Y to insert \"" string "\""))))

    (send-to-unix ^X-^P		; ^X-^E, but only till it is called...
	(setq ^X-^P (global-binding-of "\^X\^P"))
	(shell)
	(bind-to-key (global-binding-of "\^X\^P") "\^X\^E")
	(bind-to-key ^X-^P "\^X\^P"))

    (set-mark-smart		; ^@
	(if prefix-argument-provided
	    (progn mark-name
		   (setq mark-name (concat "^@-" prefix-argument))
		   (execute-mlisp-line (concat "(declare-buffer-specific " mark-name ")"))
		   (set mark-name (+ 0 (dot))))
	    (set-mark)))

    (skip-line-indentation	; ESC-m
	(beginning-of-line)
	(while (| (= (following-char) ' ') (= (following-char) '\^I'))
	       (forward-character)))

    (skip-white-space-backward
	(error-occurred char
	    (while (| (= (setq char (preceding-char)) ' ')
		      (= char '\^I')
		      (= char '\^J'))
		   (backward-character))))

    (skip-white-space-forward
	(error-occurred char
	    (while (| (= (setq char (following-char)) ' ')
		      (= char '\^I')
		      (= char '\^J'))
		   (forward-character))))

    (suspend-emacs		; ^Z^Z
	(if (error-occurred (pause-emacs))
	    (return-to-monitor)))

    (unmodify-buffer		; esc-~
	(setq buffer-is-modified 0)
	(message "Not modified."))
)

(defun
    (electric-mlisp-mode-hook
	(local-bind-to-key "one-more-paren-level"	"\en")
	(local-bind-to-key "one-less-paren-level"	"\ep"))

    (electric-lisp-mode-hook
	(electric-mlisp-mode-hook))

    (one-more-paren-level
	(save-excursion
	    (beginning-of-line)
	    (backward-paren)
	    (forward-paren)
	    (insert-character ')')
	    (next-line)
	    (re-indent-line)))

    (one-less-paren-level
	(save-excursion
	    (beginning-of-line)
	    (backward-paren)
	    (forward-paren)
	    (delete-previous-character)
	    (next-line)
	    (re-indent-line)))
)

;;; Types des caracteres ;;;
(progn
    (use-syntax-table "text-mode")
    (modify-syntax-entry "w    \^H_")
    (modify-syntax-entry "()   (")
    (modify-syntax-entry ")(   )")
    (modify-syntax-entry "     -/")

    (use-syntax-table "electric-lisp-mode")
    (modify-syntax-entry "     -/'")
)

;;; Options ;;;
(progn
    (setq-default &Default-Transpose-Magic		1)
    (setq-default &kill-lines-magic			0)
    (setq-default &MaxKillSize				1000)
    (setq-default &Occurances-Extra-Lines		1)
    (setq-default ask-about-buffer-names		0)
    (setq-default backup-before-writing			1)
    (setq-default backup-by-copying			1)
    (error-occurred (setq-default cdpath (getenv "CDPATH")))
    (setq-default checkpoint-frequency			300)
    (setq-default comment-column			49)
    (setq-default ctlchar-with-^			1)
    (setq-default files-should-end-with-newline		1)
    (setq-default help-on-command-completion-error	1)
    (setq-default mode-line-format			"%[%M <%m> %b%<r (RO)%>  %f  %p%] %<m***%>")
    (setq-default needs-checkpointing			1)
    (setq-default nrings				12)
    (setq-default quick-redisplay			1)
    (setq-default scroll-step				1)
    (setq-default silently-kill-processes		0)
    (setq-default track-eol-on-^N-^P			0)
    (setq-default unlink-checkpoint-files		0)
    (setq-default visible-bell				1)
    (setq-default wrap-long-lines			1)
)

;;; Kill and destroy ;;;
(progn
    (load "killring")
    (bind-to-key "delete-next-character"	"\^D")
    (bind-to-key "delete-previous-character"	"\^H")
    ; "\177" remains bound to "backward-kill-character"
    (bind-to-key "kill-lines-smart"		"\^K")
    (bind-to-key "kill-to-beginning-of-line"	"@")
    (bind-to-key "copy-lines-smart"		"\ek")
    (bind-to-key "copy-lines-smart"		"\eK")
    (bind-to-key "variable-to-region"		"\^Xg")
    (bind-to-key "delete-buffer"		"\^Xk")
    (bind-to-key "region-to-variable"		"\^Xx")
    (bind-to-key "show-kills"			"\^Z\^Y")
)

;;; Case modifications ;;;
(progn
    (bind-to-key "case-word-capitalize" "\ec")
)

;;; Tampons ;;;
(progn
    (bind-to-key "buffer-edit"		"\^Z\^B")
    (bind-to-key "use-old-buffer"	"\^Xb")
    (bind-to-key "unmodify-buffer"	"\e~")
)

;;; Fenetres ;;;
(progn
    (bind-to-key "delete-window"	"\^X0")
    (bind-to-key "next-window"		"\^X4")
    (bind-to-key "kill-next-window"	"\^X8")
    (bind-to-key "kill-prev-window"	"\^X9")
    (bind-to-key "previous-window"	"\^Xo"))

;;; Fichiers ;;;
(progn
    (bind-to-key "dired"		"\^Xd")
    (bind-to-key "insert-file"		"\^Xi")
    (bind-to-key "visit-file"		"\^X\^F")
    (bind-to-key "write-current-file"	"\^X\^S")
    (bind-to-key "write-named-file"	"\^X\^W")
)

;;; Recherches ;;;
(progn
    (bind-to-key "query-replace-string" "\e%")
    (bind-to-key "incremental-search"	"\^S")
    (bind-to-key "reverse-incremental-search" "\^R")
    (bind-to-key "re-search-forward"	"\e/")
    (bind-to-key "re-search-reverse"	"\e\\")
    (bind-to-key "Occurances"		"\^Xs")
    (bind-to-key "aton"			"\^XS")
    (bind-to-key "Search-Words-Forward"	"\^Xw")
    (bind-to-key "Search-Words-Backward" "\^XW")
)

;;; Communication avec Unix ;;;
(progn
    (bind-to-key "compile-it"		"\e\^C")
    (bind-to-key "next-error"		"\^X\^D")
    (bind-to-key "send-to-unix"		"\^X\^E")
    (bind-to-key "execute-monitor-command"	"\^X\^M")
    (bind-to-key "suspend-emacs"		"\^Z\^Z")
)

;;; Deplacements ;;;
(progn
    (bind-to-key "set-mark-smart"	"\^@")
    (bind-to-key "exchange-dot-and-mark-smart"	"\^X\^X")
    (bind-to-key "goto-line"		"\eg")
    (bind-to-key "move-to-screen-edge"	"\e,")
    (bind-to-key "redisplay-command"	"\^L")
    (bind-to-key "skip-line-indentation"	"\em")
    (bind-to-key "next-half-page"	"\^X\^N")
    (bind-to-key "prev-half-page"	"\^X\^P")
    (bind-to-key "roll-down"		"\^Z\^N")
    (bind-to-key "roll-up"		"\^Z\^P")
    (bind-to-key "scroll-one-line-down"	"\^Zv")
    (bind-to-key "scroll-one-line-up"	"\^Z\^V")
    (bind-to-key "line-count"		"\^X=")
)

;;; Le nettoyage ;;;
(progn
    (bind-to-key "delete-white-space"	"\e\ ")
    (bind-to-key "merge-lines"		"\e^")
    (bind-to-key "open-line"		"\e\^O")
    (bind-to-key "transpose-line"	"\e\^T")
    (bind-to-key "justify-paragraph"	"\eq")
    (bind-to-key "centre-line"		"\es")
    (bind-to-key "transpose-word"	"\et")
    (bind-to-key "new-undo"		"\^X\^U")
    (bind-to-key "delete-blank-lines"	"\^X\^O")
)

;;; Les court-circuits ;;;
(progn
    (bind-to-key "execute-ESCaped-char"	"\^C")
    (bind-to-key "describe-smart"	"\e?")
    (bind-to-key "describe-word-in-buffer" "\e\^D")
    (bind-to-key "backward-paren"	"\e(")
    (bind-to-key "forward-paren"	"\e)")
    (bind-to-key "new-indented-line"	"\e\^M")
    (bind-to-key "get-key-binding"	"\^X?")
    (bind-to-key "gen-on"		"\^X[")
    (bind-to-key "gen-off"		"\^X]")
    (bind-to-key "goto-tag"		"\e:")
    (bind-to-key "save-filename"	"\^Z\^F")
)

;;; Quelques necessites bizarres ;;;
(save-window-excursion
    (temp-use-buffer "Command execution")	(setq needs-checkpointing 0)
    (temp-use-buffer "shell")			(setq needs-checkpointing 0)
)

;;; On laisse tout bien propre... ;;;
(setq mode-line-format default-mode-line-format)

(time)
